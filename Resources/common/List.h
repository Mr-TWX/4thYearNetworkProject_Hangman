#ifndef List_h
#define List_h

#include <stdlib.h>

struct ListNode;

// structure for each node in the list
typedef struct ListNode {
	struct ListNode *next;	// next node in the list
	struct ListNode *prev;	// previous node in the list
	void *value;			// the value stored 
}ListNode;

// structure for whole list
typedef struct List {
	int count;			// total number of nodes in the list
	ListNode *first;	// the head of the list
	ListNode *last;		// the tail of the list
}List;

List *List_create();	// basic list constructor
void List_destroy(List *list);	// basic list destructor
void List_clear(List *list);	// empties the list
void List_clear_destroy(List *list); // empties the list and destroys it

// macro functions for getting count, first and last values
#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void List_push(List *list, void *value); // add an element into the list at last position
void *List_pop(List *list);	// remove last element

void List_unshift(List *list, void *value);
void *List_shift(List *list);

void *List_remove(List *list, ListNode *node); // remove a particlar node

// macro for list iteration
#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL;\
    ListNode *V = NULL;\
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif