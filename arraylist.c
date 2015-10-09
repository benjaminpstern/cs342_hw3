/*
* an Arraylist.
* Authors Ben Stern and Shiying Zheng
* functions:
* arraylist arraylist_init(int sizeOfType, int initialCapacity)
	this creates an arraylist that stores anything with size sizeOfType
	with an initial capacity of initialCapacity.  Takes array space from malloc
	so you should free it with arraylist_free(arraylist *list) later
* void arraylist_addEnd(arraylist *list, void* item)
	adds item to the end by creating a copy. Constant time.
* void arraylist_add(arraylist *list, int index, void* item)
	inserts item into index by copying the element. Insertion to the front is O(size of list)
	so with big lists you should avoid doing that too much.
* void* arraylist_set(arraylist *list, int index, void* item)
	sets the space at index to be item by copying.  Returns the item that was previously in that space,
	with allocated memory from malloc.  You should always free the return value when you're done with it.
	if you just want to set and don't care about the element that was previously there, say
	free(arraylist_set(list, index, item));
* void* arraylist_get(arraylist *list, int index)
	gets the item at space index.
* void* arraylist_removeEnd(arraylist *list)
	removes and returns the element at the end. The element returned comes out of malloc
	so you should free it when you're done with it. This takes constant time.
* void* arraylist_remove(arraylist *list,int index)
	removes and returns the element at whatever index you want. The element returned comes out of malloc
	so you should free it when you're done with it. Removing from anywhere but the end takes linear time.
* int arraylist_size(arraylist *list)
	returns the number of elements in the arraylist
* int arraylist_isEmpty(arraylist *list)
	returns 1 if the arraylist is empty(size is 0) otherwise returns 0
* arraylist* arraylist_subList(arraylist *list, int startIndex, int endIndex)
	returns a sublist by copying the data.  If you create a sublist and change it,
	it won't affect the original list. As the sublist is another arraylist, you should free it
	when you're done with it with arraylist_free
* void arraylist_clear(arraylist *list)
	clears all the elements in the arraylist.
* void arraylist_free(arraylist *list)
	frees the array allocated by malloc and also frees the memory taken up by the structure.
	if you free an arraylist you won't be able to use it again, you'll have to initialize a new one with
	arraylist_init
* void arraylist_swap(arraylist *list, int index1, int index2)
	swaps 2 elements in the arraylist.  If I have an arraylist that is [3,6,9] and I swap elements 0 and 2 then I have
	[9,6,3]
* void arraylist_print(arraylist *list, void (*printelem)(void *))
	takes in a function that prints one element.
* void arraylist_printInt(void* intPtr)
	prints an integer pointed to by intPtr
*/
#include "arraylist.h"
/*
 * Takes the size of the item to store, and the initial capacity
 * creates the arraylist, returns a pointer to it.
 * its space is malloc'd and should be freed with arraylist_free later
 */
arraylist* arraylist_init(int sizeOfType, int initialCapacity){
	//printf("SizeofType is %d\n",sizeOfType);
	void* array=malloc(initialCapacity*sizeOfType);
	arraylist *list= (arraylist *) malloc(sizeof(arraylist));
	list->array = array;
	list->element_size=sizeOfType;
	list->size=0;
	list->capacity=initialCapacity;
	//printf("element_size is %d\n",list->element_size);
	return list;
}
/*
 * adds a copy of the item pointed to by itemPtr to the end of list
 * constant time average
 */
void arraylist_addEnd(arraylist *list, void* itemPtr){
	arraylist_add(list,list->size,itemPtr);
}
/*
 * adds a copy of the item at itemPtr to the list at position index
 * Adding to the end takes constant time
 * Adding to the beginning takes linear time
 */
void arraylist_add(arraylist *list,int index, void* itemPtr){
	if(index>list->size||index<0)
		fprintf(stderr,"your index is out of bounds. size:%d index %d\n",list->size,index);
	int elem_size=list->element_size;
	if(list->size==list->capacity){
		arraylist_resize(list);
	}
	void *position=list->array+index*elem_size;
	memmove(position+elem_size,position, elem_size*(list->size-index));
	memcpy(position, itemPtr, elem_size);
	++list->size;
}
/*
 * removes the item at the end and returns a pointer to it.
 * the pointer is malloc'd and should be freed.
 * removing from the end takes constant time.
 */
void* arraylist_removeEnd(arraylist *list){
	return arraylist_remove(list,list->size-1);
}
/* removes the item at the specified index and returns a pointer to it.
 * the pointer is malloc'd and should be freed.
 * removing from the beginning takes linear time
 * removing from the end takes constant time.
 */
void* arraylist_remove(arraylist *list, int index){
	if(index>list->size||index<0)
		fprintf(stderr,"your index is out of bounds. size:%d index %d\n",list->size,index);
	void *position=list->array+index*list->element_size;
	void *element;
	if((element=malloc(list->element_size))==NULL) fprintf(stderr,"Ran out of memory");
	memcpy(element,position,list->element_size);
	memmove(position,position+list->element_size,list->element_size*(list->size-index-1));
	--list->size;
	return element;
}
/*
 * returns the location of the specified index. Always constant time.
 * This item is the item in the arraylist, not a copy.
 */
void* arraylist_get(arraylist *list, int index){
	if(index>=list->size||index<0)
		fprintf(stderr,"your index is out of bounds. size:%d index %d\n",list->size,index);
	return list->array+index*list->element_size;
}
/*
 * Sets the specified index to contain the item pointed to by itemPtr
 * returns a malloc'd pointer to what was previously in that space
 * this item should be freed at some point
 */
void* arraylist_set(arraylist *list, int index, void* itemPtr){
	if(index>=list->size||index<0)
		fprintf(stderr,"your index is out of bounds. size:%d index %d\n",list->size,index);
	void *object;
	void *position=list->array+index*list->element_size;
	if((object=malloc(list->element_size))==NULL){
		fprintf(stderr,"Ran out of memory");
	}
	memcpy(object,position,list->element_size);
	memcpy(position, itemPtr, list->element_size);
	return object;
}
/*
 * frees the array in the arraylist. However, does not free
 * other things that have been malloc'd by other functions like remove and set
 */
void arraylist_free(arraylist *list){
	free(list->array);
	free(list);
}
/*
 * returns the number of elements in the list
 */
int arraylist_size(arraylist *list){
	return list->size;
}
/*
 * returns 1 if the list is empty, 0 if the list is not
 */
int arraylist_isEmpty(arraylist *list){
	return list->size==0;
}
/*
 * makes the list size 0. Keeps the same capacity.
 */
void arraylist_clear(arraylist *list){
	list->size=0;
}
/*
 * returns a pointer to another arraylist that is the sublist
 * from the start index to the end index, inclusive.
 * as this is also an arraylist it should be freed with arraylist_free
 */
arraylist* arraylist_subList(arraylist *list, int startIndex, int endIndex){
	int i;
	if(startIndex>=list->size||startIndex<0||endIndex>=list->size||endIndex<0||endIndex<startIndex)
		fprintf(stderr,"your index is out of bounds. size:%d startIndex %d endIndex %d\n",list->size,startIndex,endIndex);
	int newSize=endIndex-startIndex+1;
	arraylist *sublist=arraylist_init(list->element_size,newSize);
	for(i=startIndex;i<=endIndex;++i){
		arraylist_addEnd(sublist,arraylist_get(list,i));
	}
	return sublist;
}
/*
 * swaps two elements in the list. constant time.
 */
void arraylist_swap(arraylist *list, int firstIndex, int secondIndex){
	void* a=arraylist_set(list,firstIndex,arraylist_get(list,secondIndex));
	void* b=arraylist_set(list,secondIndex,a);
	free(a);
	free(b);
}
/*
 * a "private" method. Don't use this outside of this file
 */
void arraylist_resize(arraylist *list){
	void *sparePointer=list->array;
	list->capacity*=2;
	if((list->array=malloc(list->capacity*list->element_size))==NULL){
		fprintf(stderr,"Ran out of memory");
	}
	memcpy(list->array, sparePointer, list->size*list->element_size);
	free(sparePointer);
}
/*
 * Takes a function pointer to print one element,
 * and prints the list using that function
 */
void arraylist_print(arraylist *list, void (*printelem)(void *)){
	int i;
	printf("[");
	for(i=0;i<list->size;++i){
		if(i!=0) printf(",");
		printelem(arraylist_get(list,i));
	}
	printf("]\n");
}
/*
 * if the arraylist is full of ints, this prints the arraylist.
 * otherwise it prints a bunch of nonsense.
 */
void arraylist_printInt(void* i){
	int j=*((int*)i);
	printf("%d",j);
}
