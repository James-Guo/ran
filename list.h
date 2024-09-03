/* Skeleton for W4.6, comp20003 workshop Week 4 */

/*-------------------------------------------------------------- 
..Project: Assignement 1
  list.h :  
          = the implementation of module linkedList of the project

  NOTES:
        - this module is polymorphic
        - the data component in a list node is of type void*
----------------------------------------------------------------*/

#ifndef _LIST_H_
#define _LIST_H_
#include <stddef.h>
#include "data.h"

// node structure
typedef struct node {
    record_t *data;     // data of current node
    struct node *next;    // pointer to next node in list
} node_t;

// define linked list as pointers
typedef struct record record_t;

typedef struct list {
    node_t *head;   // first node of list
    node_t *tail;   // last node of list
    size_t n;       // number of element in list
} list_t;

// creates & returns an empty linked list
list_t *listCreate();

// free the list, with the help of function "freeData" for freeind data component
void listFree(list_t *list, void (*dataFree)(void *));

/* functions for insertion ---------------------------------------------------------- */ 

// inserts a new node with value "data" to the end of "list" 
void listAppend(list_t *list, record_t *r); 

// performs linear search in "list", returns found data or NULL 
void *listSearch(list_t *list, const char *key, char *(*dataGetKey)(void *));

#endif




/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */
