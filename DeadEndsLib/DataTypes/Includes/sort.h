//
// DeadEnds
// sort.h -- Lists are kept sorted if a compare function is provided when they are created.
// Sort.c provides a sort function on Lists using quick sort, and a search function that uses
// binary search.
//
// Created by Thomas Wetmore on 21 November 2022.
// Last changed on 22 March 2024.

#ifndef sort_h
#define sort_h

#include "standard.h"

void sortElements(void**, int, String(*g)(void*), int(*c)(String, String));
void* linearSearch(void**, int, String, String(*)(void*), int*);
void* binarySearch(void**, int, String, String(*)(void*), int(*c)(String, String), int*);

void insertAtIndex(void**, int len, void*, int index);

#endif // sort_h
