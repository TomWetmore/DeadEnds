// DeadEnds
//
// list.c has the functions that implement a Block-based list. Lists grow automatically when
// needed. Lists can be sorted or unsorted. Sorted lists require a compare function.
//
// Created by Thomas Wetmore on 22 November 2022.
// Last changed on 20 April 2024.

#include <stdlib.h>
#include "list.h"
#include "sort.h"

// createList creates and returns a List.
List* createList(String(*getKey)(void*), int(*compare)(String, String),
				 void(*delete)(void*), bool sorted) {
	List *list = (List *) malloc(sizeof(List));
	initList(list, getKey, compare, delete, sorted);
	return list;
}

// initList initializes a List.
void initList(List *list, String(*getKey)(void*), int(*compare)(String, String),
			  void (*delete)(void*), bool sorted) {
	initBlock(&(list->block));
	list->compare = compare;
	list->delete = delete;
	list->getKey = getKey;
	list->sorted = sorted;
}

// deleteList frees a List and its elements.
void deleteList(List *list) {
	deleteBlock(&(list->block), list->delete);
	free(list);
}

// lengthList returns the length of a List.
int lengthList(List *list) {
	return (&(list->block))->length;
}

// emptyList removes all the elements from a List.
void emptyList(List *list) {
	emptyBlock(&(list->block), list->delete);
}

// isEmptyList returns true if the List is empty.
bool isEmptyList(List *list) {
	return (&(list->block))->length == 0;
}

// copyList creates a copy of a List; requires a copy function to make copies of elements.
List* copyList(List* list, void* (*copyFunc)(void*)) {
	List* copy = createList(list->getKey, list->compare, list->delete, list->sorted);
	copy->isSorted = list->isSorted;
	Block* oblock = &list->block;
	Block* nblock = &copy->block;
	nblock->length = oblock->length;
	nblock->maxLength = oblock->maxLength;
	nblock->elements = malloc(oblock->maxLength*sizeof(void*));
	for (int i = 0; i < oblock->length; i++) {
		nblock->elements[i] = copyFunc(oblock->elements[i]);
	}
	return copy;
}

// appendToList appends an element to the end of a List.
void appendToList(List *list, void *element) {
	appendToBlock(&(list->block), element);
	list->isSorted = false;
}

// prependToList adds an element to the front of a List.
void prependToList(List* list, void* element) {
	prependToBlock(&(list->block), element);
	list->isSorted = false;
}

// getFromList returns an indexed element from a List; the element is not removed from the List.
void* getFromList(List* list, int index) {
	return getFromBlock(&(list->block), index);
}

// getFirstListElement returns the first element from a List; it is not removed from the List.
void* getFirstListElement(List* list) {
	return getFirstBlockElement(&(list->block));
}

// getLastListElement returns the last element from a List; it is not removed from the List.
void* getLastListElement(List* list) {
	return getLastBlockElement(&(list->block));
}

// removeFromList removes an element from a List at a specified index.
bool removeFromList(List* list, int index) {
	return removeFromBlock(&(list->block), index, list->delete);
}

// removeFirstListElement removes the first element from a List; uses delete function.
bool removeFirstListElement(List *list) {
	return removeFirstBlockElement(&(list->block), list->delete);
}

// removeLastListElement removes the last element from a List; uses delete function.
bool removeLastListElement(List* list) {
	return removeLastBlockElement(&(list->block), list->delete);
}

// getAndRemoveFirstListElement removes and returns the first element from a List; caller must free.
void* getAndRemoveFirstListElement(List* list) {
	void* element = getFirstListElement(list);
	removeFirstBlockElement(&(list->block), null); // null == don't delete
	return element;
}

// getAndRemoveLastListElement removes and returns the last element from a List; caller must free.
void* getAndRemoveLastListElement(List* list) {
	void* element = getLastListElement(list);
	removeLastBlockElement(&(list->block), null);
	return element;
}

// isSortedList returns true if the List's sorted flag is set.
bool isSortedList(List* list) {
	return list->isSorted;
}

// isListSorted returns true if the List is REALLY sorted.
bool isListSorted(List* list) {
	return isSorted(&(list->block), list->getKey, list->compare);
}

// sortList sorts a list using the list's compare function.
void sortList(List* list) {
	if (list->isSorted) return;
	sortBlock(&(list->block), list->getKey, list->compare);
	list->isSorted = true;
}

// searchList searches a List for a given Key. If sorted is true the List must be sorted.
void* searchList(List* list, String key, int* index, bool sorted) {
	if (sorted)
		return searchSortedBlock(&(list->block), key, list->getKey, list->compare, index);
	else
		return searchBlock(&(list->block), key, list->getKey, index);
}

// uniqueList removes duplicates from a List.
void uniqueList(List* list) {
	sortList(list);
	uniqueBlock(&(list->block), list->getKey, list->delete);
}

// setListElement sets a specific element in a list.
void setListElement(List *list, void* element, int index) {
	Block *block = &(list->block);
	setBlockElement(block, element, list->delete, index);
	list->isSorted = false;
}

// getListElement gets a specific element from a list. It is not a copy.
void* getListElement(List *list, int index) {
	Block *b = &(list->block);
	ASSERT(list && index >= 0 && index < b->length);
	return b->elements[index];
}

// insertInList inserts an element into a list at a given location, moving followers down. The
// isSorted flag is not changed. If an insertion can cause a sorted list to become unsorted the
// the caller should change the flag to false.
void insertInList(List *list, void *element, int index) {
	insertInBlock(&(list->block), element, index);
}

// showList shows the contents of a List. The describe function it is called on each element to
// get the String to print.
void showList(List *list, String (*describe)(void*)) {
	if (!list || !describe) return;
	Block *block = &(list->block);
	printf("showList: len = %d; maxlen = %d\n", block->length, block->maxLength);
	for (int i = 0; i < block->length; i++)
		printf("%s ", describe(block->elements[i]));
	printf("\n");
}

// isInList checks if an element with given key is in the List.
bool isInList(List* list, String key, int* index) {
	if (list->sorted) {
		sortList(list);
		return isInSortedBlock(&(list->block), key, list->getKey, list->compare, index);
	}
	return isInBlock(&(list->block), key, list->getKey, index);
}

// iterateList iterates the elements of a List calling a function on each.
void iterateList(List *list, void(*iterate)(void*)) {
	iterateBlock(&(list->block), iterate);
}

// findInList finds the element with given key in a list; the List may be sorted or unsorted.
// If the element is in the List it is returned. The returned element also remains in the List
// so the calling function must not modify it.
void* findInList(List* list, String key, int* index) {
	if (list->sorted) {
		sortList(list);
		return findInSortedBlock(&(list->block), key, list->getKey, list->compare, index);
	}
	return findInBlock(&(list->block), key, list->getKey, index);
}
