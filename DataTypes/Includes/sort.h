//
//  DeadEnds
//
//  sort.h -- Lists are kept sorted if a compare function is provided when they are created.
//    Sort.c provides a sort function on Lists using quick sort, and a search function that uses
//    binary search.
//
//  Created by Thomas Wetmore on 21 November 2022.
//  Last changed on 10 April 2023.
//

#ifndef sort_h
#define sort_h

#include "standard.h"  // Word.
#include "list.h"  // List.

// ldata and lcmp -- State variables used to simplify the interfaces to the sorting functions.
//--------------------------------------------------------------------------------------------------
extern Word *ldata;              // The data to be sorted.
extern int (*lcmp)(Word, Word);  // The compare function.

void quickSort (int left, int right);

void sortList(List*, bool force);  // The list must have a compare function.
Word searchList(List*, Word value, int*);  // The list must have a compare function.
Word searchListWithKey(List*, String key, int*);  // The list must have a compare function.

#endif // sort_h
