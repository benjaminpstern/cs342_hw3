/*
 * an Arraylist.
 Authors Ben Stern and Shiying Zheng
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
typedef struct{
    void *array;
    int element_size;
    int size;
    int capacity;
} arraylist;

extern arraylist* arraylist_init(int sizeOfType, int initialCapacity);
extern void arraylist_addEnd(arraylist *list,void* itemPtr);
extern void arraylist_add(arraylist *list, int index,void* itemPtr);
extern void* arraylist_set(arraylist *list, int index,void* itemPtr);
extern void* arraylist_get(arraylist *list, int index);
extern void* arraylist_removeEnd(arraylist *list);
extern void* arraylist_remove(arraylist *list,int index);
extern int arraylist_size(arraylist *list);
extern int arraylist_isEmpty(arraylist *list);
extern arraylist* arraylist_subList(arraylist *list, int startIndex, int endIndex);
extern void arraylist_clear(arraylist *list);
extern void arraylist_free(arraylist *list);
extern void arraylist_resize(arraylist *list);
extern void arraylist_swap(arraylist *list, int firstIndex, int secondIndex);
extern void arraylist_print(arraylist *list, void (*printelem)(void *));
extern void arraylist_printInt(void* intPtr);
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
