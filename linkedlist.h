/*
 * A doubly linked list
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* A type definition of the node struct */
typedef struct node{
	void* data; //a pointer to the data stored in the node
	unsigned int datasize; //the size of the data
	struct node *next, *prev; //pointers to the next and the previous node
} node;
/* A type definition of the linkedlist struct*/
typedef struct linkedlist{
	unsigned int datasize; //the size of the data in each element
	unsigned int size; //the number of elements in the linkedlist
	struct node *head, *tail; //the first and last element in the linkedlist
} linkedlist;
/* A type definition of the iterator struct */
typedef struct iterator{
	linkedlist* list; //the list iterated on
	int position; //the index of the next element that will be returned by calling iteratornext(iterator* iter)
	node* current; //the element that is before the element that will be returned by calling iteratornext(iterator* iter), 
	               //or the element that will be returned by calling linkedlist_iteratorprev(iterator* iter)
} iterator;
extern linkedlist* linkedlist_init(int datasize);
extern void linkedlist_addfront(linkedlist* list, void* data);
extern void linkedlist_addend(linkedlist* list, void* data);
extern void* linkedlist_getfront(linkedlist* list);
extern void* linkedlist_getend(linkedlist* list);
extern void* linkedlist_rmfront(linkedlist* list);
extern void* linkedlist_rmend(linkedlist* list);
extern int linkedlist_size(linkedlist* list);
extern iterator* linkedlist_iterator(linkedlist* list);
extern iterator* linkedlist_iteratorend(linkedlist* list);
extern int linkedlist_iteratorhasnext(iterator* iter);
extern int linkedlist_iteratorhasprev(iterator* iter);
extern void* linkedlist_iteratornext(iterator* iter);
extern void* linkedlist_iteratorprev(iterator* iter);
//iteratoradd and iteratorrm not implemented
extern void linkedlist_iteratoradd(iterator* iter);
extern void* linkedlist_iteratorrm(iterator* iter);
extern void linkedlist_free(linkedlist* list);
extern void linkedlist_freeiter(iterator* iter);
