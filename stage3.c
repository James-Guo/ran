#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "data.h"
#include "list.h"

#define MAX_FIELD_LEN 127
#define STAGE_THREE 3
#define NUM_ARG 4

int processArgs(int argc, char *argv[]);
list_t *getRecordList(char *filename);
void iterateList(FILE *f, list_t *recordlist);
void processQueries(list_t *recordlist, FILE *inputfile, FILE *outputfile);

int main(int argc, char *argv[]) {

    // process arguments
    processArgs(argc, argv);

    // read record from inputfile 
    list_t *recordlist = getRecordList(argv[2]);

    // create and write output file
    FILE *outputfile = fopen(argv[3], "w");
    assert(outputfile);

    if (!outputfile) {
        perror("Error opening output file\n");
        listFree(recordlist, recordFree);
        exit(EXIT_FAILURE);
    }

    // process input queries and write to output file
    processQueries(recordlist, stdin, outputfile);

    fclose(outputfile);
    listFree(recordlist, recordFree);

    return 0;
}

// make sure number of arguments is 4 and return stage 
int processArgs(int argc, char *argv[]) {
    if (argc != NUM_ARG) {
        fprintf(stderr, "Usage: $s <stage> <input CSV file> <output file>\n");
    }
    
    int stage = atoi(argv[1]);          // convert string to integer
    if (stage != STAGE_THREE) {
        fprintf(stderr, "Invalid stage!\n");
        exit(EXIT_FAILURE);
    }
    return stage;
}

// reads record from file
list_t *getRecordList(char *filename) {
    FILE *inputfile = fopen(filename, "r");
    assert(inputfile);

    if (!inputfile) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // read and create linked list to store records
    list_t *recordlist = listCreate();

    // skip header line
    recordSkipHeaderLine(inputfile);

    // iterate the linked list with records
    iterateList(inputfile, recordlist);

    fclose(inputfile);
    return recordlist;
}

// iterate linked list
void iterateList(FILE *f, list_t *recordlist) {
    node_t *nodelist = recordRead(f);
    node_t *curr = nodelist;

    while (curr != NULL) {
        record_t *r = curr->data;
        // Debug print to verify records are read correctly
        /*printf("Read record: %d, %d, %s, %d, %s, %s, %s, %s, %lf, %lf\n",
            r->codeSubject, r->codeSuburb, r->nameSuburb, r->year,
            r->codeState, r->nameState, r->codeGovt, r->nameGovt,
            r->lat, r->lon); */
        //printf("Adding record to list: %d, %s\n", r->codeSubject, r->nameSuburb);
        listAppend(recordlist, r);

        node_t *temp = curr;
        curr = curr->next;
        free(temp);
    }
}

// querying with suburb name on linked list
void processQueries(list_t *recordlist, FILE *inputfile, FILE *outputfile) {

    // interactive querying
    char query[MAX_FIELD_LEN + 1];

    while (fgets(query, sizeof(query), inputfile) != NULL) {

        // remove newline character
        query[strcspn(query, "\n")] = 0;

        // Debugging line
        //printf(stdout, "Processing query: %s\n", query);

        node_t *curr = recordlist->head;
        int nmatched = 0;
        int found = 0;

        // initialize comparisons counter
        comparisons_t cmp_count = {0, 0, 0};

        fprintf(outputfile, "%s -->\n", query);

        // search for matching record using suburb name
        while (curr != NULL) {

            // increment for node access and string comparisons
            cmp_count.node_access += 1;
            cmp_count.str_cmps += 1;

            record_t *r = curr->data;
            int i = 0;
            int mismatched_found = 0;

            // Debug output to verify content
            //printf("Comparing query: '%s' with stored record: '%s'\n", query, r->nameSuburb);

            // count the number of bit comparisons
            while (query[i] != '\0' && r->nameSuburb[i] != '\0') {
                cmp_count.bit_cmps += 8;

                if (query[i] != r->nameSuburb[i]) {
                    mismatched_found = 1;
                    break;
                }
                i++;
            }

            // Check if both strings ended (meaning they are identical in length and content)
            if (query[i] == '\0' && r->nameSuburb[i] == '\0') {
                cmp_count.bit_cmps += 8;        // Count the null terminator comparison
            } else if (!mismatched_found) {
                cmp_count.bit_cmps += 8;        // Count the last character comparison
            }

            if (strcmp(r->nameSuburb, query) == 0) {
                nmatched++;
                found = 1;
                recordPrint(outputfile, r, query);
            } 
            curr = curr->next;
        }

        if (found == 0) {
            //fprintf(outputfile, "%s --> \n", query);
            fprintf(stdout, "%s --> NOTFOUND\n", query);
        } else {
            fprintf(stdout, "%s --> %d records - comparisons: b%d n%d s%d\n", query, nmatched, cmp_count.bit_cmps, cmp_count.node_access, cmp_count.str_cmps);
        }
    }
}
