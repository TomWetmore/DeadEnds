//
// DeadEnds
//
// set.c contains functions that implement Sets. A Set is a sortable List. The elements are
// void* pointers.  Each Set has a getKey function that returns a String that represents
// each Set element.
//
// Created by Thomas Wetmore on 22 November 2022.
// Last changed 10 April 2024.
//

#include "set.h"

// createSet creates a Set; the getKey and compare functions are required; delete is optional.
Set* createSet(String(*getKey)(void*), int(*compare)(String, String), void(*delete)(void*)) {
	Set* set = (Set*) malloc(sizeof(Set));
	initList(&(set->list), getKey, compare, delete, true);
	return set;
}

// deleteSet frees a set.
void deleteSet(Set *set) {
	deleteList(&(set->list));
	//free(set);
}

// addToSet adds an element to a Set. If an element with the same key is in the Set it is removed.
void addToSet(Set* set, void* element) {
	int index;
	List* list = &(set->list);
	String key = list->getKey(element);
	void* oldElement = findInList(list, key, &index);
	if (oldElement)
		removeFromList(list, index);
	insertInList(list, element, index);
}

// isInSet checks whether an element with given key is in a Set.
bool isInSet(Set* set, String key) {
	return isInList(&(set->list), key, null);
}

// removeFromSet removes the element with given key from a Set; if no such element does nothing.
void removeFromSet(Set* set, String key) {
	if (!set || !key) return;
	List* list = &(set->list);
	int index;
	void* element = findInList(list, key, &index);
	if (!element) return;
	removeFromList(list, index);
}

// iterateSet iterates the elements of a set, calling a function on each.
void iterateSet(Set* set, void (*action)(void*)) {
	iterateList(&(set->list), action);
}

// lengthSet returns the number of elements in a Set.
int lengthSet(Set *set) {
	return lengthList(&(set->list));
}

// showSet show the contents of a set using a describe function. Delegate to the list.
void showSet(Set *set, String (*toString)(void*)) {
	showList(&(set->list), toString);
}

// listOfSet returns the List that holds the Set's elements.
List* listOfSet(Set* set) {
	return &(set->list);
}
