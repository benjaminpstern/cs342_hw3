/*
 * A doubly linked list
 */
#include "linkedlist.h"
/*
 * Initialize a node
 * Takes in an integer datasize, which is the size of the data we are storing, 
 * and a void pointer to the element stored.
 * Copies the data into the node.
 * Returns a pointer to the initialized node.
 */
node* node_init(int datasize, void* data){
	node* n=malloc(sizeof(node));
	if(!n) exit(EXIT_FAILURE);;
	n->data=malloc(datasize);
	if(!n->data) exit(EXIT_FAILURE);;
	if(data) memcpy(n->data, data, datasize);
	n->datasize=datasize;
	return n;
}
/*
 * Frees a node. Takes in a node pointer.
 */
void node_free(node* n){
	free(n->data);
	free(n);
}
/*
 * Initializes a linkedlist.
 * Takes in an integer datasize, which is the size of each element.
 * Returns a pointer to the list.
 * You should use linkedlist_free(linkedlist* list) to free the list when you are done with it.
 */
linkedlist* linkedlist_init(int datasize){
	linkedlist* list=malloc(sizeof(linkedlist));
	if(!list) exit(EXIT_FAILURE);;
	list->size=0;
	list->datasize=datasize;
	list->head=node_init(datasize,NULL);
	list->tail=node_init(datasize,NULL);
	list->head->prev=NULL;
	list->head->next=list->tail;
	list->tail->prev=list->head;
	list->tail->next=NULL;
	return list;
}
/*
 * Adds an element to the front of the list.
 * Takes a pointer to the list and a void pointer to the element.
 */
void linkedlist_addfront(linkedlist* list, void* data){
	if(!data) fprintf(stderr,"Nullpointer; trying to add NULL data");
	node* newnode=node_init(list->datasize,data);
	node* tmp=list->head->next;
	list->head->next=newnode;
	newnode->prev=list->head;
	newnode->next=tmp;
	tmp->prev=newnode;
	list->size++;
}
/*
 * Adds an element to the end of the list.
 * Takes a pointer to the list and a void pointer to the element.
 */
void linkedlist_addend(linkedlist* list, void* data){
	if(!data) fprintf(stderr,"Nullpointer; trying to add NULL data");
        node* newnode=node_init(list->datasize,data);
        node* tmp=list->tail->prev;
        list->tail->prev=newnode;
        newnode->next=list->tail;
        newnode->prev=tmp;
        tmp->next=newnode;
	list->size++;
}
/*
 * Gets the front element of the linkedlist.
 * Takes in a pointer to the list.
 * Returns a void pointer to the element. 
 * It is the actual element stored in the list instead of a copy of the element.
 */
void* linkedlist_getfront(linkedlist* list){
	if(!list->size){ 
		fprintf(stderr,"Cannot get from an empty list");
		return NULL;	
	}
	void* data=list->head->next->data;
	return data;
}
/*
 * Gets the end element of the linkedlist.
 * Takes in a pointer to the list.
 * Returns a void pointer to the element. 
 * It is the actual element stored in the list instead of a copy of the element.
 */
void* linkedlist_getend(linkedlist* list){
	if(!list->size){ 
		fprintf(stderr,"Cannot get from an empty list");
		return NULL;	
	}
	void* data=list->tail->prev->data;
	return data;
}
/*
 * Removes the element at the front of the list.
 * Takes in a pointer to the list.
 * If there is no element in the list, returns NULL.
 * Otherwise, returns the data of the removed element.
 * You should free the element when you are done with it.
 */
void* linkedlist_rmfront(linkedlist* list){
	if(!list->size){
		fprintf(stderr,"can't remove from a list of size 0");
		return NULL;
	}
	node* removethis=list->head->next;
	void* value=malloc(list->datasize);
	if(!value) exit(EXIT_FAILURE);;
	memcpy(value,removethis->data,list->datasize);
	list->head->next=removethis->next;
	removethis->next->prev=list->head;
	node_free(removethis);
	list->size--;
	return value;
}
/*
 * Removes the element at the end of the list.
 * Takes in a pointer to the list.
 * If there is no element in the list, returns NULL.
 * Otherwise, returns the data of the removed element.
 * You should free the element when you are done with it.
 */
void* linkedlist_rmend(linkedlist* list){
	if(!list->size){
		fprintf(stderr,"can't remove from a list of size 0");
		return NULL;
	}
	node* removethis=list->tail->prev;
	void* value=malloc(list->datasize);
	if(!value) exit(EXIT_FAILURE);;
	memcpy(value,removethis->data,list->datasize);
	list->tail->prev=removethis->prev;
	removethis->prev->next=list->tail;
	node_free(removethis);
	list->size--;
	return value;
}
/*
 * Takes in a pointer to the list.
 * Returns the size of the linkedlist as an integer.
 */
int linkedlist_size(linkedlist* list){
	return list->size;
}
/*
 * Generates an iterator over a list.
 * Takes in a pointer to the list.
 * Returns a pointer to the iterator.
 * You should call linkedlist_freeiter(iterator* iter) when you are done with the iterator.
 */
iterator* linkedlist_iterator(linkedlist* list){
	iterator* iter=malloc(sizeof(iterator));
	if(!iter) exit(EXIT_FAILURE);
	iter->list=list;
	iter->position=0; //position is the index of the next element element that will be returned by calling iteratornext(iterator* iter)
	iter->current=list->head;
	return iter;
}
/*
 * Generates an iterator over a list at the end of the list.
 * Takes in a pointer to the list.
 * Returns a pointer to the iterator.
 * You should call linkedlist_freeiter(iterator* iter) when you are done with the iterator.
 */
iterator* linkedlist_iteratorend(linkedlist* list){
	iterator* iter=malloc(sizeof(iterator));
	if(!iter) exit(EXIT_FAILURE);
	iter->list=list;
	iter->position=list->size; //position is the index of the next element element that will be returned by calling iteratornext(iterator* iter),
								//which is NULL at the end of the list.
	iter->current=list->tail->prev;
	return iter;
}
/*
 * Returns 0 if there is not a next element in the iterator.
 * Otherwise, returns non-zero value.
 */
int linkedlist_iteratorhasnext(iterator* iter){
	return (iter->position < iter->list->size);
}
/*
 * Returns 0 if there is not a previous element in the iterator.
 * Otherwise, returns non-zero value.
 */
int linkedlist_iteratorhasprev(iterator* iter){
	return (iter->position > 0);
}
/*
 * Returns a pointer to the next element in the iterator.
 */
void* linkedlist_iteratornext(iterator* iter){
	void* value=iter->current->next->data;
	if(!value) fprintf(stderr,"I don't have a next");
	iter->current=iter->current->next;
	iter->position++;	
	return value;
}
/*
 * Returns a pointer to the previous element in the iterator.
 */
void* linkedlist_iteratorprev(iterator* iter){
	void* value=iter->current->data;
	if(!value) fprintf(stderr,"I don't have a prev");
	else iter->current=iter->current->prev;	
	iter->position--;	
	return value;
}
void linkedlist_iteratoradd(iterator* iter){
	//not implemented yet
}
void* linkedlist_iteratorrm(iterator* iter){
	//not implemented yet
	void* a;
	return a;
}
/*
 * Frees the linkedlist and all the elements in it.
 * Takes a pointer to the list.
 */
void linkedlist_free(linkedlist* list){
	node* freethis=list->head;
	node* tmp;
	while(freethis!=list->tail){
		tmp=freethis->next;
		node_free(freethis);
		freethis=tmp;
	}
	node_free(list->tail);
	free(list);
}	
/*
 * Free the linkedlist iterator.
 * Takes a pointer to the iterator.
 */
void linkedlist_freeiter(iterator* iter){
	free(iter);
}
