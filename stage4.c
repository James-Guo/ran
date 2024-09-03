#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "data.h"

#define MAX_FIELD_LEN 127
#define STAGE_FOUR 4
#define NUM_ARG 4

int processArgs(int argc, char *argv[]);
void readRecordtoTree(char *filename, ptc_tree_t *tree);
void processQueries(ptc_tree_t *tree, FILE *inputfile, FILE *outputfile);
int editDistance(char *str1, char *str2, int n, int m);
int min(int a, int b, int c);

void inorderTraversal(ptc_node_t* root) {
    if (root == NULL) {
        return;
    }

    inorderTraversal(root->left);        // Visit left subtree
    printf("%s\n", root->key);           // Visit current node
    inorderTraversal(root->right);       // Visit right subtree
}

int main(int argc, char *argv[]) {

    // process arguments
    processArgs(argc, argv);

    // create a patricia tree
    ptc_tree_t *tree = treeCreate();

    // read record from inputfile 
    readRecordtoTree(argv[2], tree);

    printf("*************************************\n");
    printf("In Order Traversal\n");
    inorderTraversal(tree->root);
    printf("*************************************\n");

    // create and write output file
    FILE *outputfile = fopen(argv[3], "w");
    assert(outputfile != NULL);

    // process input queries and write to output file
    processQueries(tree, stdin, outputfile);

    fclose(outputfile);
    treeFree(tree, NULL);       // Pass NULL if not used

    return 0;
}

// make sure number of arguments is 4 and return stage 
int processArgs(int argc, char *argv[]) {
    if (argc != NUM_ARG) {
        fprintf(stderr, "Usage: $s <stage> <input CSV file> <output file>\n");
    }
    
    int stage = atoi(argv[1]);          // convert string to integer
    if (stage != STAGE_FOUR) {
        fprintf(stderr, "Invalid stage!\n");
        exit(EXIT_FAILURE);
    }
    return stage;
}

// reads record from file
void readRecordtoTree(char *filename, ptc_tree_t *tree) {
    FILE *inputfile = fopen(filename, "r");
    assert(inputfile);

    if (!inputfile) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // skip header line
    recordSkipHeaderLine(inputfile);

    // read and insert records to tree
    char line[MAX_FIELD_LEN + 1];
    // printf("Inside readRecordtoTree\n");
    while (fgets(line, sizeof(line), inputfile) != NULL) {
        printf("Processing line: %s\n", line);

        
        // process every line to extract data for tree insertion
        record_t *record = recordParse(line);
        comparisons_t cmps = {0, 0, 0};
        // printf("\tBEFORE TREEINSERT\n");
        // printf("\t LINE: %s\n", line);
        // printf("\t SuburbName: %s\n", record->nameSuburb);
        treeInsert(tree, record->nameSuburb, record, &cmps);
    }

    fclose(inputfile);
}

// querying with suburb name on patricia tree
void processQueries(ptc_tree_t *tree, FILE *inputfile, FILE *outputfile) {
    char query[MAX_FIELD_LEN + 1];
    comparisons_t queryComparisons;
    //match_list_t *matches = NULL;
    //closest_match_t closest;

    printf("PROCESSQUERIES\n");

    while (fgets(query, sizeof(query), inputfile) != NULL) {

        // Remove newline character
        query[strcspn(query, "\n")] = '\0';
        trimWhitespace(query);

        // Initialize query comparisons
        queryComparisons.bit_cmps = 0;
        queryComparisons.node_access = 0;
        queryComparisons.str_cmps = 0;

        fprintf(outputfile, "%s -->\n", query);

        // printf("********************\n");
        // printf("QUERY %s\n", query);
        // printf("********************\n");

        // Perform search
        match_list_t *matches = NULL;
        int exactfound = treeSearch(tree, query, &queryComparisons, &matches);



        // Output results
        int matchCount = getMatchCount(matches);
        if (exactfound) {
            //recordPrint(outputfile, tree->root->data, query);
            printf("\tExact FOUND\n");

            fprintf(stdout, "%s --> %d records - comparisons: b%d n%d s%d\n", query, matchCount, queryComparisons.bit_cmps, queryComparisons.node_access, queryComparisons.str_cmps);

            // Print each matching record to the output file
            match_list_t *curr = matches;
            while (curr != NULL) {
                recordPrint(outputfile, curr->node->data, query);
                curr = curr->next;
            }

            freeMatchList(matches);
        } else {
            printf("\tExact NOT FOUND\n");

            // Get closest match if no exact match is found
            //closest = getCloseMatch(tree, query, &queryComparisons);
            closest_match_t closest;
            closest.edit_distance = INT_MAX;
            closest.key = malloc((MAX_FIELD_LEN + 1)* sizeof(char));
            //closest.key = malloc(MAX_FIELD_LEN + 1); // Allocate memory for key
            if (closest.key == NULL) {
                perror("Failed to allocate memory for closest.key");
                exit(EXIT_FAILURE);
            }
            memset(closest.key, 0, MAX_FIELD_LEN + 1);
            closest.node = NULL;
            // Perform tree traversal to find the closest match
            treeIterate(tree, query, &queryComparisons, &closest);

            if (closest.node != NULL) {
                //fprintf(stdout, "%s --> Closest match: %s (edit distance: %d)\n", query, closest.key, closest.edit_distance);
                printf("\tClosest key is %s (edit distance: %d)\n", closest.key, closest.edit_distance);
                recordPrint(outputfile, closest.node->data, query);
                fprintf(stdout, "%s --> 1 records - comparisons: b%d n%d s%d\n", query, queryComparisons.bit_cmps, queryComparisons.node_access, queryComparisons.str_cmps);
            } else {
                fprintf(stdout, "%s --> NOTFOUND\n", query);
            }

            // Free the matches list after processing each query
            free(closest.key);
        }
    }
}
