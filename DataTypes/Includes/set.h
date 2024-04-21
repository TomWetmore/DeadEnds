//
// DeadEnds
//
// set.h is the header file for the Set type.
//
// Created by Thomas Wetmore on 22 November 2022.
// Last changed on 10 April 2024.
//

#ifndef set_h
#define set_h

#include "list.h"

// Set implements a set using a sorted List. Its elements are structures with String keys.
// The getkey and compare functions are used to extract and compare keys.
typedef struct Set {
	List list;
} Set;

// Public interface.
Set* createSet(String(*get)(void*), int(*cmp)(String, String), void(*del)(void*));
void deleteSet(Set*);
int lengthSet(Set*);
bool isInSet(Set*, String);
void addToSet(Set*, void*);
void removeFromSet(Set*, String);
void iterateSet(Set*, void(*iter)(void*));
void showSet(Set*, String(*show)(void*));

List* listOfSet(Set*); // Underlying sorted List.

// FORSET/ENDSET are macros that allow general Set iteration.
#define FORSET(set, element)\
{\
	void* element;\
	List* _list = &(set->list);\
	Block* _block = &(_list->block);\
	void** _elements = (void**) _block->elements;\
	for (int _i = 0; _i < _block->length; _i++) {\
		element = _elements[_i];\
		{

#define ENDSET\
		}\
	}\
}

#endif // set_h
