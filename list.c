/* Skeleton for W4.6, comp20003 workshop Week 4 */

/*-------------------------------------------------------------- 
..Project: Assignement 1
  list.c :  
          = the implementation of module linkedList of the project

  NOTES:
        - this module is polymorphic
        - the data component in a list node is of type void*
----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"
#include "data.h"

void error(char *name) {
    fprintf(stderr, "Function %s not yet implemented.\n", name);
    exit(EXIT_FAILURE);
};

// make an empty list
list_t *listCreate() {
    list_t *list = malloc(sizeof(*list));
    assert(list);
    list->head = list->tail = NULL;
    list->n = 0;
    return list;
}

// free memory used by list
void listFree(list_t *list, void (*dataFree)(void *)) {
    assert(list != NULL);
    node_t *current = list->head;        // current points to the first node

    while (current) {                   // while current not at the end of list
        node_t *tmp = current;
        current = current->next;        // advance current to next next node
        dataFree(tmp->data);            // free previous node with data
        free(tmp);                      // free space used by data
    }
    free(list);
}

/* insert data to list ---------------------------------------------------------- */ 

// insert new node accordingly
void listAppend(list_t *list, record_t *r) {

    // create new node, insert data
    node_t *new = (node_t *)malloc(sizeof(node_t));
    assert(new);

    if (!new) {
        perror("Failed to allocate memory for new node");
        return;
    }

    new->data = r;
    new->next = NULL;

    if (list->head == NULL) {
        list->head = new;
    } else {
        node_t *curr = list->head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new;
    }
    list->n++;

    // Debug output to verify correct storage
    //printf("Stored record: %d, %s\n", r->codeSubject, r->nameSuburb);
}

/* function processing over the list ---------------------------------------------------------- */ 

// return found data or NULL
void *listSearch(list_t *list, const char *key, char *(*dataGetKey)(void *)) {
    node_t * current = list->head;

    while (current != NULL) {
        if (strcmp(key, dataGetKey(current->data)) == 0) {
            return current->data;;
        }
        current = current->next;
    }
    return NULL;
}




/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */
