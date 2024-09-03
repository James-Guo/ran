/* Skeleton for W4.6, comp20003 workshop Week 4 */


/*-------------------------------------------------------------- 
..Project: Assignment 1
  data.c :  
         = the implementation of module data of the project
----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>
#include "data.h"
#include "list.h"

#define MAX_LINE_LEN 511
#define MAX_FIELD 10

// skip header line of csv.file
void recordSkipHeaderLine(FILE *f) {
    while (fgetc(f) != '\n');
}

// deal with double quotes
record_t * recordParse(char *line) {
    record_t *r = (record_t *)malloc(sizeof(record_t));
    assert(r != NULL);

    char *field[MAX_FIELD];
    int i = 0;

    char *start = line;
    char *end;

    while (*start) {
        if (*start == '"') {
            start++;
            end = strchr(start, '"');
            if (end == NULL) {
                free(r);
                return NULL;
            }
        } else {
            end = strchr(start, ',');
            if (end == NULL) {
                end = start + strlen(start);
            }
        }

        field[i] = (char *)malloc(end - start + 1);

        if (field[i] == NULL) {
            perror("Failed to allocate memory for field");
            for (int j = 0; j < i; j++) free(field[j]);
            free(r);
            return NULL;
        }
        strncpy(field[i], start, end - start);
        field[i][end - start] = '\0';
        i++;

        start = end;
        if (*start == '"') start++;
        if (*start == ',') start++;
        while (*start == ' ') start++;
    }

    if (i >= 10) {
        r->codeSubject = atoi(field[0]);
        r->codeSuburb = atoi(field[1]);
        strncpy(r->nameSuburb, field[2], sizeof(r->nameSuburb));
        r->year = atoi(field[3]);
        strncpy(r->codeState, field[4], sizeof(r->codeState));
        strncpy(r->nameState, field[5], sizeof(r->nameState));
        strncpy(r->codeGovt, field[6], sizeof(r->codeGovt));
        strncpy(r->nameGovt, field[7], sizeof(r->nameGovt));
        r->lat = atof(field[8]);
        r->lon = atof(field[9]);
    }

    for (int j = 0; j < i; j++) free(field[j]);

    return r;
}

// read the clean data into linked list
node_t *recordRead(FILE *f) {
    char line[MAX_LINE_LEN];
    node_t *head = NULL;
    node_t *tail = NULL;
    record_t *r;

    // Read a line from the file
    while (fgets(line, sizeof(line), f) != NULL) {

        r = recordParse(line);

        // Parse the line
        if (r != NULL) {
            node_t *node = (node_t *)malloc(sizeof(node_t));
            if (node == NULL) {
                perror("Failed to allocate memory for node");
                free(r);
                return NULL;
            }
            node->data = r;
            node->next = NULL;
            if (tail == NULL) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        } else {
            free(r);
        }
    }
    return head;
}

// prints record *r to file "stage1" and "stage3"
void recordPrint(FILE *outputfile, record_t *r, char *query) {
    fprintf(outputfile, "COMP20003 Code: %d, Official Code Suburb: %d, Official Name Suburb: %s, Year: %d, Official Code State: %s, Official Name State: %s, Official Code Local Government Area: %s, Official Name Local Government Area: %s, Latitude: %.7lf, Longitude: %.7lf\n", r->codeSubject, r->codeSuburb, r->nameSuburb, r->year, r->codeState, r->nameState, r->codeGovt, r->nameGovt, r->lat, r->lon);
}

// find matching suburb
record_t *recordCmpName(record_t *n1, record_t *n2) {
    if (strcmp(n1->nameSuburb, n2->nameSuburb) == 0) {
        return n1;
    } else {
        return NULL;
    }
}

// free memory allocated to a suburb "r"
void recordFree(void *r) {
    free(r);
}

// return the matched record
char *recordGetName(void *r) {
    return ((record_t *)r)->nameSuburb;
}

// Function to write escaped strings
void writeEscapedString(FILE *outputfile, const char *str) {
    // Check if the string contains a comma or double quote
    int needsQuotes = strchr(str, ',') != NULL || strchr(str, '"') != NULL;

    if (needsQuotes) {
        fprintf(outputfile, "\"");
        while (*str) {
            if (*str == '"') {
                fprintf(outputfile, "\"\"");
            } else {
                fputc(*str, outputfile);
            }
            str++;
        }
        fprintf(outputfile, "\"");
    } else {
        fprintf(outputfile, "%s", str);
    }
}

// print for stage2
void recordWriteToCSV(FILE *outputfile, record_t *r) {
    fprintf(outputfile, "%d,%d,%s,%d,%s,%s,", r->codeSubject, r->codeSuburb, r->nameSuburb, r->year, r->codeState, r->nameState);

    // field with double quotes
    writeEscapedString(outputfile, r->codeGovt);
    fprintf(outputfile, ",");
    writeEscapedString(outputfile, r->nameGovt);
    fprintf(outputfile, ",");

    fprintf(outputfile, "%.7lf,%.7lf\n", r->lat, r->lon);
}

// function to create a new patricia tree
ptc_tree_t *treeCreate() {
    ptc_tree_t *tree = (ptc_tree_t *)malloc(sizeof(ptc_tree_t));
    assert(tree != NULL);
    tree->root = NULL;
    return tree;
}

// function to create a new patricia node
ptc_node_t *createNode(char *key, void *data, int bitIndex) {
    ptc_node_t *node = (ptc_node_t *)malloc(sizeof(ptc_node_t));
    assert(node != NULL);
    node->key = strdup(key);
    assert(node->key != NULL);
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->bitIndex = bitIndex;
    return node;
}

// insert a key into the patricia tree
/*void treeInsert(ptc_tree_t *tree, char *key, void *data, comparisons_t *cmps) {
    if (tree->root == NULL) {
        tree->root = createNode(key, data, 0);
        return;
    }

    ptc_node_t *curr = tree->root;
    ptc_node_t *parent = NULL;

    // Traverse the tree to find the correct place to insert
    while (curr && curr->bitIndex < strlen(key) * 8) {
        cmps->node_access++;
        parent = curr;

        int i;
        for (i = curr->bitIndex; i < strlen(key) * 8; i++) {
            cmps->bit_cmps++;
            if ((key[i / 8] & (1 << (7 - (i % 8)))) != (curr->key[i / 8] & (1 << (7 - (i % 8))))) {
                break;
            }
        }

        if (i >= strlen(key) * 8) {
            break;
        }

        curr = (key[i / 8] & (1 << (7 - (i % 8)))) ? curr->right : curr->left;
    }

    if (curr && strcmp(key, curr->key) == 0) {
        cmps->str_cmps++;
        curr->data = data;
        return;
    }

    int bitIndex = parent->bitIndex + 1;
    ptc_node_t *new_node = createNode(key, data, bitIndex);
    if ((key[bitIndex / 8] & (1 << (7 - (bitIndex % 8))))) {
        new_node->left = parent;
        new_node->right = curr;
    } else {
        new_node->left = curr;
        new_node->right = parent;
    }

    if (parent == tree->root) {
        tree->root = new_node;
    } else {
        ptc_node_t *grandparent = tree->root;
        while (grandparent->left != parent && grandparent->right != parent) {
            grandparent = (key[grandparent->bitIndex / 8] & (1 << (7 - (grandparent->bitIndex % 8)))) ? grandparent->right : grandparent->left;
        }

        if (grandparent->left == parent) {
            grandparent->left = new_node;
        } else {
            grandparent->right = new_node;
        }
    }
}*/

int treeInsert(ptc_tree_t *tree, const char *key, void *data, comparisons_t *cmps) {
    if (tree == NULL || key == NULL || data == NULL) {
        fprintf(stderr, "Error: NULL pointer passed to treeInsert.\n");
        return -1;
    }

    ptc_node_t *parent = NULL;
    ptc_node_t *current = tree->root;

    printf("Trying to insert: %s\n", key);
    printf("current == NULL evaluates to: %d\n", current == NULL);


    // Create the new node
    ptc_node_t *newNode = (ptc_node_t *)malloc(sizeof(ptc_node_t));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed in treeInsert.\n");
        return -1;
    }
    memset(newNode, 0, sizeof(ptc_node_t));

    newNode->key = strdup(key);  // Allocate and copy the key
    if (newNode->key == NULL) {
        fprintf(stderr, "Memory allocation for key failed in treeInsert.\n");
        free(newNode);
        return -1;
    }

    newNode->data = data;
    newNode->bitIndex = cmps->bit_cmps;  // Set the bitIndex


    if (current == NULL) {
        // If tree is empty, insert as root
        printf("EMPTY TREE\n");
        printf("Adding as root: %s\n", key);
        tree->root = newNode;
        return 0;
    }



    printf("current != NULL evaluates to: %d\n", current != NULL);
    printf("current->bitIndex < cmps->bit_cmps evaluates to: %d\n", current->bitIndex < cmps->bit_cmps);



    

    // Find the appropriate parent node
    while (current != NULL) {
        parent = current;
        printf("THIS IS CURRENTLY PARENT: %s\n", parent->key);
        cmps->node_access++;
        if (strcmp(key, current->key) < 0) {
            cmps->str_cmps++;
            current = current->left;
        } else {
            cmps->str_cmps++;
            current = current->right;
        }
    }

    // printf("\t*************************************\n");
    // printf("\t*************************************\n");
    // printf("\t*************************************\n");
    // printf("\t*************************************\n");
    // printf("\t*************************************\n");
    // printf("\t*************************************\n");
    printf("\t Parent key: %s\n", parent->key);



    if (parent == NULL) {
        // If tree is empty, insert as root
        printf("Adding as root: %s\n", key);
        tree->root = newNode;
    } else {
        int bitIndex = parent->bitIndex + 1;
        newNode->bitIndex = bitIndex;

        if (strcmp(key, parent->key) < 0) {
            printf("Add to the left: %s\n", key);
            parent->left = newNode;
        } else {
            printf("Add to the right: %s\n", key);
            parent->right = newNode;
        }
    }

    return 0;
}

// Function to start the tree search
int treeSearch(ptc_tree_t *tree, char *query, comparisons_t *comparisons, match_list_t **matches) {
    // Ensure the tree and key are valid
    /*if (!tree || !key || !matches) {
        return;
    }


    // Initialize comparisons structure
    if (cmps) {
        cmps->bit_cmps = 0;
        cmps->node_access = 0;
        cmps->str_cmps = 0;
    }

    // Start the recursive search
    treeSearchRecursive(tree->root, (char *)key, cmps, matches);*/



    // Initialize search
    *matches = NULL;
    comparisons->bit_cmps = 0;
    comparisons->node_access = 0;
    comparisons->str_cmps = 0;

    printf("STARTING SEARCH FOR: %s\n", query);

    ptc_node_t *current = tree->root;
    int found = 0;

    if (current == NULL) {
        return 0;
    }

    while (current != NULL) {
        comparisons->node_access++;

        // Compare the query with the current node's key
        int cmp = patriciaCompare(query, current->key, comparisons);
        printf("\tCMP VALUE: %d", cmp);
        comparisons->bit_cmps += cmp;
        comparisons->str_cmps++;

        // Debug output
        printf("\tComparing '%s' with '%s': cmp=%d\n", query, current->key, cmp);

        if (cmp == 0) {
            // Exact match found
            match_list_t *new_match = malloc(sizeof(match_list_t));
            if (new_match == NULL) {
                perror("Failed to allocate memory for new_match");
                exit(EXIT_FAILURE);
            }
            new_match->node = current;
            new_match->next = *matches;
            *matches = new_match;
            found = 1;
            break;
        } else if (cmp < 0) {
            printf("\t GOING LEFT\n");
            // Move to the left child
            current = current->left;
        } else {
            // Move to the right child
            printf("\t GOING RIGHT\n");
            current = current->right;
        }
    }

    return found;

}

// Helper function to recursively search a subtree
void treeSearchRecursive(ptc_node_t *node, char *key, comparisons_t *cmps, match_list_t **matches) {
    if (!node || !key || !matches) return;

    // Debug current node information
    //printf("Recursive search at node with key: %s\n", node->key);

    // Increment node access count
    cmps->node_access++;


    // Check if the current node has a valid key and bit index
    /*if (node->bitIndex < 0 || node->bitIndex >= strlen(key) * 8) {
        printf("Invalid bitIndex %d for key: %s\n", node->bitIndex, key);
        return;
    }

    int i;
    for (i = node->bitIndex; i < strlen(key) * 8; i++) {
        cmps->bit_cmps++;
        

        // Print bit comparison details for debugging
        printf("Comparing bit index %d: key bit = %d, node bit = %d\n", i,
               (key[i / 8] & (1 << (7 - (i % 8)))) >> (7 - (i % 8)),
               (node->key[i / 8] & (1 << (7 - (i % 8)))) >> (7 - (i % 8)));

        if ((key[i / 8] & (1 << (7 - (i % 8)))) != (node->key[i / 8] & (1 << (7 - (i % 8))))) {
            // Mismatch found, search both subtrees
            printf("Mismatch found at index %d, node key: %s\n", i, node->key);
            //match_list_t *result = NULL;
            if (node->left) {
                printf("Traversing left child with key: %s\n", node->left->key);
                treeSearchRecursive(node->left, key, cmps, matches);
            }
            if (node->right) {
                printf("Traversing right child with key: %s\n", node->right->key);
                treeSearchRecursive(node->right, key, cmps, matches);
            }
            return;
        }
    }

    // If we reach this point, the keys match up to the bitIndex
    if (strcmp(key, node->key) == 0) {
        cmps->str_cmps++;
        printf("Match found for key: %s\n", key);
        addMatch(matches, node);
        return;
    }

    // Continue traversing based on the bit value
    if ((key[i / 8] & (1 << (7 - (i % 8)))) != 0) {
        if (node->right) {
            printf("Traversing right child with key: %s\n", node->right->key);
            treeSearchRecursive(node->right, key, cmps, matches);
        }
    } else {
        if (node->left) {
            printf("Traversing left child with key: %s\n", node->left->key);
            treeSearchRecursive(node->left, key, cmps, matches);
        }
    }*/
    
    // Compare the key and node's key
    if (strcmp(node->key, key) == 0) {
        // Exact match
        match_list_t *new_match = malloc(sizeof(match_list_t));
        new_match->node = node;
        new_match->next = *matches;
        *matches = new_match;
        cmps->str_cmps++;       // increment full string comparisons count
        return;
    }

    // Traverse the left and right child nodes
    if (node->left) {
        treeSearchRecursive(node->left, key, cmps, matches);
    }
    if (node->right) {
        treeSearchRecursive(node->right, key, cmps, matches);
    }
}

// linkes list to store multiple matches
void addMatch(match_list_t **matches, ptc_node_t *node) {
    match_list_t *newMatch = (match_list_t *)malloc(sizeof(match_list_t));
    assert(newMatch != NULL);
    newMatch->node = node;
    newMatch->next = *matches;
    *matches = newMatch;
}

// function to free the tree node
void nodeFree(ptc_node_t *node) {
    if (node == NULL) {
        return;
    }
    nodeFree(node->left);
    nodeFree(node->right);
    free(node->key);
    free(node->data);
    free(node);
}

/*void findClosestMatch(ptc_node_t *node, const char *key, match_list_t **matches) {
    match_list_t *current = *matches;
    match_list_t *closestMatch = NULL;
    int minDistance = INT_MAX;

    while (current) {
        int distance = editDistance(key, current->node->key, strlen(key), strlen(current->node->key));
        if (distance < minDistance) {
            minDistance = distance;
            closestMatch = current;
        }
        current = current->next;
    }

    // Output the closest match if found
    if (closestMatch) {
        fprintf(stdout, "Closest match for '%s': %s\n", key, closestMatch->node->key);
    } else {
        printf("No close match found for '%s'\n", key);
    }
}*/

// Function to update the closest match if a better match is found
void updateClosestMatch(closest_match_t *currentClosest, char *key, ptc_node_t *node, int distance) {
    if (distance < currentClosest->edit_distance || 
       (distance == currentClosest->edit_distance && strcmp(key, currentClosest->key) < 0)) {
        currentClosest->edit_distance = distance;
        strncpy(currentClosest->key, key, MAX_FIELD_LEN);
        currentClosest->node = node;
    }
}

// Function to traverse the tree and find closest match
void traverseTree(ptc_node_t *node, char *query, comparisons_t *comparisons, closest_match_t *closest) {
    if (node == NULL) {
        return;
    }
    printf("Visiting node with key: %s\n", node->key);  // Debug print
    // Assume node->key contains the string key of the current node
    /*int queryLength = strlen(query);
    int keyLength = strlen(node->key);

    // Calculate the edit distance
    int distance = editDistance(query, node->key, queryLength, keyLength);

    // Update closest match if the current node has a smaller edit distance
    if (distance < closest->edit_distance) {
        closest->edit_distance = distance;
        closest->key = strdup(node->key); // Duplicate the key to closestMatch
        closest->node = node;
    }

    // Increment node access count
    comparisons->node_access++;

    // Continue traversing the tree
    traverseTree(node->left, query, comparisons, closest);
    traverseTree(node->right, query, comparisons, closest);*/

    if (!node || !query || !comparisons || !closest) {
        return;
    }

    // Increment node access count
    comparisons->node_access++;

    // Compare the key and node's key
    int bit_comparisons = calculateBitComparisons(node->key, query);
    comparisons->bit_cmps += bit_comparisons;
    comparisons->str_cmps++; // Increment full string comparison count


    // Base case: check if the node's key matches the query
    if (strcmp(node->key, query) == 0) {
        // Exact match found
        closest->node = node;
        closest->edit_distance = 0; // Exact match has zero edit distance
        strncpy(closest->key, node->key, MAX_FIELD_LEN);
        return;
    }

    // If no exact match, check if we need to continue searching
    // Update bit comparisons based on the current node's key and query
    //int bit_comparisons = 0; // Example: you need to calculate the actual bit comparison count
    // Update comparisons->bit_cmps here
    //comparisons->bit_cmps += bit_comparisons;

    // Check if we need to traverse left or right based on bit mismatches
    // For simplicity, let's assume binary tree traversal:
    if (node->left) {
        traverseTree(node->left, query, comparisons, closest);
    }
    if (node->right) {
        traverseTree(node->right, query, comparisons, closest);
    }
}

// Function to collect all candidate nodes starting from the given node
void collectCandidates(ptc_node_t *node, char *query, int mismatchIndex, match_list_t **candidates) {
    if (node == NULL) return;

    // Collect data at the node
    if (node->key != NULL) {
        match_list_t *newMatch = malloc(sizeof(match_list_t));
        newMatch->node = node;
        newMatch->next = *candidates;
        *candidates = newMatch;
    }

    // Traverse children nodes
    collectCandidates(node->left, query, mismatchIndex, candidates);
    collectCandidates(node->right, query, mismatchIndex, candidates);
}

// Main treeIterate function
void treeIterate(ptc_tree_t *tree, char *query, comparisons_t *comparisons, closest_match_t *closest) {
    // Initialize the result with maximum possible distance
    /*result->node = NULL;
    result->edit_distance = INT_MAX;
    memset(result->key, 0, MAX_FIELD_LEN);

    // Start traversal from the root
    traverseTree(tree->root, query, comparisons, result);*/

    ptc_node_t *current = tree->root;

    if (current == NULL) {
        return;
    }

    // Initialize the closest match
    closest->edit_distance = INT_MAX;
    closest->key[0] = '\0';
    closest->node = NULL;

    while (current != NULL) {
        comparisons->node_access++;

        // Calculate edit distance
        int distance = editDistance(query, current->key, strlen(query), strlen(current->key));
        comparisons->bit_cmps++;
        comparisons->str_cmps++;

        // Debug output
        printf("\tComparing '%s' with '%s': distance=%d\n", query, current->key, distance);

        if (distance < closest->edit_distance) {
            closest->edit_distance = distance;
            strncpy(closest->key, current->key, MAX_FIELD_LEN);
            closest->key[MAX_FIELD_LEN] = '\0';  // Ensure null termination
            closest->node = current;
        }

        // Traverse to the left or right child
        if (strcmp(query, current->key) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
}

// function to free the patricia tree
void treeFree(ptc_tree_t *tree, match_list_t **matches) {
    if (tree) {
        nodeFree(tree->root);
        free(tree);
    }

    // Free the match list if matches is not NULL
    if (matches && *matches) {
        match_list_t *current = *matches;
        while (current) {
            match_list_t *next = current->next;
            free(current);
            current = next;
        }
    }
}

// Function to free the match list
void freeMatchList(match_list_t *matches) {
    match_list_t *curr = matches;
    while (curr != NULL) {
        match_list_t *next = curr->next;
        free(curr);
        curr = next;
    }
}

// Function to get the count of records in the match list
int getMatchCount(match_list_t *matches) {
    int count = 0;
    /*match_list_t *current = matches;

    while (current != NULL) {
        count++;
        current = current->next;
    }*/

    while (matches) {
        count++;
        matches = matches->next;
    }

    return count;
}

// Function to get the closest match
closest_match_t getCloseMatch(ptc_tree_t *tree, char *query, comparisons_t *queryComparisons) {
    closest_match_t closest = {NULL, INT_MAX, NULL};
    match_list_t *matches = NULL;

    // Perform search to gather all potential matches
    treeSearch(tree, query, queryComparisons, &matches);

    // Check each match for the closest one
    match_list_t *curr = matches;
    while (curr != NULL) {
        record_t *record = (record_t *)curr->node->data;
        int distance = editDistance(query, record->nameSuburb, strlen(query), strlen(record->nameSuburb));
        if (distance < closest.edit_distance) {
            closest.edit_distance = distance;
            closest.key = record->nameSuburb;
            closest.node = curr->node;
        }
        curr = curr->next;
    }

    // Free the matches list after processing
    freeMatchList(matches);

    return closest;
}

/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

int patriciaCompare(const char *query, const char *key, comparisons_t *comparisons) {
    int i = 0;
    int bit_comparisons = 0;

    // Initialize comparisons
    if (comparisons != NULL) {
        comparisons->bit_cmps = 0;
        comparisons->node_access = 0;
        comparisons->str_cmps = 0;
    }

    // If one string is a prefix of the other, count remaining bits
    while (query[i] != '\0') {
        for (int bit = 7; bit >= 0; --bit) {
            bit_comparisons++;
        }
        i++;
    }

    while (key[i] != '\0') {
        for (int bit = 7; bit >= 0; --bit) {
            bit_comparisons++;
        }
        i++;
    }

    // Update comparison counters
    if (comparisons != NULL) {
        comparisons->bit_cmps += bit_comparisons;
        comparisons->node_access++;
    }

    // Return the number of bits compared
    return bit_comparisons;
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m) {
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = min(1 + dp[i - 1][j], 1 + dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}

int calculateBitComparisons(char *key1, char *key2) {
    int bit_comparisons = 0;
    int length = (strlen(key1) < strlen(key2)) ? strlen(key1) : strlen(key2);

    for (int i = 0; i < length; i++) {
        unsigned char c1 = key1[i];
        unsigned char c2 = key2[i];
        // Count differing bits between c1 and c2
        unsigned char diff = c1 ^ c2;
        while (diff) {
            bit_comparisons += diff & 1;
            diff >>= 1;
        }
    }
    
    // Count extra bits in the longer string
    int extra_bits = 0;
    if (strlen(key1) > length) {
        for (int i = length; i < strlen(key1); i++) {
            extra_bits += __builtin_popcount((unsigned char)key1[i]);
        }
    }
    if (strlen(key2) > length) {
        for (int i = length; i < strlen(key2); i++) {
            extra_bits += __builtin_popcount((unsigned char)key2[i]);
        }
    }
    return bit_comparisons + extra_bits;
}

// Function to trim whitespace from a string
char *trimWhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator character
    *(end + 1) = '\0';

    return str;
}

/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */

