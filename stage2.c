/* Skeleton for W4.6, comp20003 workshop Week 4 */


/*-------------------------------------------------------------- 
..Project: Assignment 1
  stage2.c : 
             = the main program of the project 
----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "data.h"
#include "list.h"

#define MAX_FIELD_LEN 127
#define MAX_QUERIES 100
#define STAGE_TWO 2
#define NUM_ARG 4

int processArgs(int argc, char *argv[]);
list_t *getRecordList(char *filename);
void processDelete(list_t *list, char *queries[], int numQueries, FILE *outputfile);
void iterateList(FILE *f, list_t *recordlist);

int main(int argc, char *argv[]) {

    // process arguments
    processArgs(argc, argv);

    // read record from inputfile 
    list_t *recordlist = getRecordList(argv[2]);
    char query[MAX_FIELD_LEN + 1];
    char *queries[MAX_QUERIES];
    int numQueries = 0;

    // create and write output file
    FILE *outputfile = fopen(argv[3], "w");
    assert(outputfile);

    if (!outputfile) {
        perror("Error opening output file\n");
        listFree(recordlist, recordFree);
        exit(EXIT_FAILURE);
    }
     
    // process input queries and write to output file
    while (fgets(query, sizeof(query), stdin) != NULL) {
        query[strcspn(query, "\n")] = '\0';

        if (numQueries < MAX_QUERIES) {
            queries[numQueries++] = strdup(query);
        } else {
            fprintf(stderr, "Too many queries.\n");
            break;
        }
    }

    processDelete(recordlist, queries, numQueries, outputfile);

    fclose(outputfile);
    listFree(recordlist, recordFree);

    // Free query strings
    for (int i = 0; i < numQueries; i++) {
        free(queries[i]);
    }

    return 0;
}

// make sure number of arguments is 4 and return stage 
int processArgs(int argc, char *argv[]) {
    if (argc != NUM_ARG) {
        fprintf(stderr, "Usage: $s <stage> <input CSV file> <output file>\n");
    }
    
    int stage = atoi(argv[1]);          // convert string to integer
    if (stage != STAGE_TWO) {
        fprintf(stderr, "Invalid stage!\n");
        exit(EXIT_FAILURE);
    }
    return stage;
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

void processDelete(list_t *list, char *queries[], int numQueries, FILE *outputfile) {

    // print file header
    fprintf(outputfile, "COMP20003 Code,Official Code Suburb,Official Name Suburb,Year,Official Code State,Official Name State,Official Code Local Government Area,Official Name Local Government Area,Latitude,Longitude\n");

    // track the number of deletions for each query
    int *deleteCounts = (int *)calloc(numQueries, sizeof(int));
    if (!deleteCounts) {
        perror("Error allocating memory for deletion counts");
        exit(EXIT_FAILURE);
    }

    // track records to be deleted
    bool *toDelete = (bool *)malloc(list->n * sizeof(bool));

    if (!toDelete) {
        perror("Error allocating memory for deletion tracking");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < list->n; i++) {
        toDelete[i] = false;
    }

    // Traverse the list and mark records for deletion if they match any query
    node_t *curr = list->head;
    size_t index = 0;

    while (curr != NULL) {
        record_t *r = (record_t *)curr->data;

        for (int i = 0; i < numQueries; i++) {
            if (strcmp(r->nameSuburb, queries[i]) == 0) {
                toDelete[index] = true;
                deleteCounts[i]++;
                break; // No need to check further queries for this record
            }
        }

        curr = curr->next;
        index++;
    }

    // Traverse the list again and delete marked records, write the rest to output file
    curr = list->head;
    index = 0;
    int num = 0;
    node_t *prev = NULL;

    while (curr != NULL) {
        record_t *r = (record_t *)curr->data;

        if (toDelete[index]) {
            // Record is marked for deletion
            node_t *node_to_delete = curr;

            if (prev == NULL) {
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }

            // Update tail if necessary
            if (curr->next == NULL) {
                list->tail = prev;
            }

            curr = curr->next;          // Move to next node
            recordFree(node_to_delete->data);
            free(node_to_delete);
            num++;
            list->n--;
        } else {
            // Record is not marked for deletion, write to output file
            recordWriteToCSV(outputfile, r);
            prev = curr;
            curr = curr->next;
        }

        index++;
    }

    // Free the tracking array
    free(toDelete);


    for (int i = 0; i < numQueries; i++) {
        if (deleteCounts[i] > 0) {
            fprintf(stdout, "%s --> %d records deleted\n", queries[i], deleteCounts[i]);
        } else {
            fprintf(stdout, "%s --> NOTFOUND\n", queries[i]);
        }
    }
}    




/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */
