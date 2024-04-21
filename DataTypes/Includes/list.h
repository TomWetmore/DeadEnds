// DeadEnds
//
// list.h defines the types and interface that implement a Block-based list. Lists grow
// automatically. Lists can be sorted or unsorted. Sorted lists require a compare function.
//
// Created by Thomas Wetmore on 22 November 2022.
// Last changed on 20 April 2024.

#ifndef list_h
#define list_h

#include "block.h"

#define enqueueList prependToList
#define dequeueList getAndRemoveLastListElement
#define pushList prependToList
#define popList getAndRemoveFirstListElement

typedef struct List {
	Block block; // Block of elements.
	bool sorted; // This is a sorted list.
	bool isSorted; // This sorted list is currently sorted.
	void (*delete)(void*); // Function to delete an element.
	String (*getKey)(void*); // Function to retrieve a string key from an element.
	int (*compare)(String, String); // Compare function needed by sorted lists.
} List;

List* createList(String(*g)(void*), int(*c)(String, String), void (*d)(void*), bool sorted);
void initList(List*, String(*g)(void*), int(*c)(String, String), void(*d)(void*), bool sorted);
void deleteList(List*);
int lengthList(List*);
void emptyList(List*);
bool isEmptyList(List*);
List* copyList(List*, void*(*c)(void*));
bool isInList(List*, String, int*);
void* findInList(List*, String, int* index);
// Array access.
void setListElement(List*, void*, int);
void *getListElement(List *list, int index);

void appendToList(List*, void*);
void prependToList(List*, void*);
void insertInList(List*, void*, int);
bool isSortedList(List*); // Uses sorted flag.
bool isListSorted(List*); // Really checks.
void sortList(List*);
void uniqueList(List*);
bool removeFromList(List*, int);
bool removeFirstListElement(List*);
bool removeLastListElement(List*);
void iterateList(List*, void(*perform)(void*));

void* getFromList(List*, int);
void* getFirstListElement(List*);
void* getLastListElement(List*);

void* getAndRemoveLastListElement(List*);
void* getAndRemoveFirstListElement(List*);



Block* blockOfList(List*);

void showList(List *list, String(*describe)(void*));

#define FORLIST(list, value)\
{\
	void *value;\
	Block *block = &(list->block);\
	void **_values = (void**) block->elements;\
	for (int _i = 0; _i < block->length; _i++) {\
		value = _values[_i];\
		{
#define ENDLIST\
		}\
	}\
}

#endif
