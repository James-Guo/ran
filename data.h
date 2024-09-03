/* Skeleton for W4.6, comp20003 workshop Week 4 */


/*-------------------------------------------------------------- 
..Project: Assignment 1
  data.h :  
         = the implementation of module data of the project
----------------------------------------------------------------*/

#ifndef _DATA_H_
#define _DATA_H_
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

// constant defnitions
#define MAX_FIELD_LEN 127
#define MAX_INPUT_LEN 511
#define MAX_FIELD 10

//detail of "struct record"
typedef struct record {
    struct record *next;     // pointer to the next record
    int year;
    int codeSubject;
    int codeSuburb;
    double lon;              // longitude
    double lat;              // latitude
    char nameSuburb[MAX_FIELD_LEN + 1];           // 1 for null terminator
    char codeState[MAX_FIELD_LEN + 1];
    char nameState[MAX_FIELD_LEN + 1];
    char codeGovt[MAX_FIELD_LEN + 1];           // govt for official local government area
    char nameGovt[MAX_FIELD_LEN + 1];
} record_t;

typedef struct node node_t;

typedef struct list list_t;

// detail of "struct comparisons"
typedef struct comparisons {
    int bit_cmps;       // number of bit comparisons
    int str_cmps;       // number of full string comparisons
    int node_access;        // number of node accesses
} comparisons_t;

// structure for patricia tree node
typedef struct ptc_node {
    char *key;          // key string to store in this node
    void *data;         // data associated with this key
    struct ptc_node *left;         // left child
    struct ptc_node *right;        // right child
    int bitIndex;
} ptc_node_t;

// patricia tree structure
typedef struct ptc_tree {
    ptc_node_t *root;       // root node of the tree
} ptc_tree_t;

typedef struct match_list {
    //record_t *data;  // Pointer to the matching record
    ptc_node_t *node;       // Node in the tree that matches
    struct match_list *next;       // Pointer to the next match in the list
} match_list_t;

// Define a structure for the closest match result
typedef struct closest_match {
    char *key;        // Key of the closest match
    int edit_distance; // Edit distance to the query
    ptc_node_t *node; // Node of the closest match
} closest_match_t;

/* function definitions -------------------------------------------*/

// skip the header line of .csv file "f"
void recordSkipHeaderLine(FILE *f);

// reads cleaned data to rercord_t 
node_t *recordRead(FILE *f);

// data cleaning
record_t * recordParse(char *line);

// prints for stage 1
void recordPrint(FILE *outputfile, record_t *r, char *query);

// print for stage 2
void recordWriteToCSV(FILE *outputfile, record_t *r);

// compares suburb name and find matching record  
record_t *recordCmpName(record_t *n1, record_t *n2);

// free the memory allocated to a record "r"
void recordFree(void *r);

// returns suburb name same as query
char *recordGetName(void *data);

// Function to write escaped strings
void writeEscapedString(FILE *outputfile, const char *str);

// Function to create a new Patricia tree
ptc_tree_t *treeCreate(void);

// function to create a new node
ptc_node_t *createNode(char *key, void *data, int bitIndex);

// Insert a key into the Patricia tree
/*void treeInsert(ptc_tree_t *tree, char *key, void *data, comparisons_t *cmps);*/

int treeInsert(ptc_tree_t *tree, const char *key, void *data, comparisons_t *cmps);

// function to search for a key in the Patricia tree
/*match_list_t *treeSearch(ptc_tree_t *tree, char *key, comparisons_t *cmps, match_list_t **matches);*/

int treeSearch(ptc_tree_t *tree, char *query, comparisons_t *comparisons, match_list_t **matches);

void treeSearchRecursive(ptc_node_t *node, char *key, comparisons_t *cmps, match_list_t **matches);

// function to free the Patricia tree node
void nodeFree(ptc_node_t *node);

// function to free the Patricia tree
void treeFree(ptc_tree_t *tree, match_list_t **matches);

// Function to free the match list
void freeMatchList(match_list_t *matches);

void addMatch(match_list_t **matches, ptc_node_t *node);

int getMatchCount(match_list_t *matches);

// Function to get the closest match
closest_match_t getCloseMatch(ptc_tree_t *tree, char *query, comparisons_t *queryComparisons);

int editDistance(char *str1, char *str2, int n, int m);

int min(int a, int b, int c);

// Function to update the closest match if a better match is found
void updateClosestMatch(closest_match_t *currentClosest, char *key, ptc_node_t *node, int distance);

// Function to traverse the tree and find closest match
void traverseTree(ptc_node_t *node, char *query, comparisons_t *comparisons, closest_match_t *closest);

// Function to collect all candidate nodes starting from the given node
void collectCandidates(ptc_node_t *node, char *query, int mismatchIndex, match_list_t **candidates);

// Main treeIterate function
void treeIterate(ptc_tree_t *tree, char *query, comparisons_t *comparisons, closest_match_t *closest);

int calculateBitComparisons(char *key1, char *key2);

// Function to trim whitespace from a string
char *trimWhitespace(char *str);

int patriciaCompare(const char *query, const char *key, comparisons_t *comparisons);

#endif




/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */
