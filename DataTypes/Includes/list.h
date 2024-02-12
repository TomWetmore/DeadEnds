//
//  DeadEnds
//
//  list.h -- List type. By providing an optional compare function, lists can be sorted and
//    searched. They grow when necessary. The data part of a list is a contiguous block of
//    pointers. They can point to anything. The compare function, if needed, deals with the
//    details. Provide a delete function if the list's elements need to be removed or otherwise
//    disposed of when the they are removed from the list or the list is removed.
//
//    MNOTE: Lists don't know what their elements are. Memory management must be done by the
//    caller. This leads to two cases. First, the user allocates the element and the list's
//    delete function frees them. Or the list's delete function does not delete them and the
//    caller is responsible.
//
//  Created by Thomas Wetmore on 22 November 2022.
//  Last changed on 22 November 2023.
//

#ifndef list_h
#define list_h

#include "standard.h"

// The initial size of a List's data block.
#define INITIAL_SIZE_LIST_DATA_BLOCK 32
//#define INITIAL_SIZE_LIST_DATA_BLOCK 4  // For testing.

//  Macros for stack and queue operations.
#define enqueueList prependListElement
#define dequeueList removeLastListElement
#define pushList prependListElement
#define popList removeFirstListElement

//  List -- Data structure for an automatically growing list. Can be sorted, and uniqued if a
//    compare function is available. The keepSorted field specifies that the list is a sorted
//    list. Some functions are lazy so sorted lists are not always sorted. Sorted lists may not
//    be sorted until a size threshold is met.
//--------------------------------------------------------------------------------------------------
typedef struct List {
    bool keepSorted;    // True if the list is a sorted list.
    bool isSorted;      // True when the list is actually sorted.
    int sortThreshold;  // A sorted list need not be sorted until this threshold.
    int length;         // Current length of the list.
    int maxLength;      // Maximum length the list can be before reallocation.
    Word *data;         // The data making up the list.
    int (*compare)(const Word, const Word);  // Function to compare elements, if needed.
    void (*delete)(Word);    // Function to delete elements, if needed.
    String (*getKey)(Word);  // Function to get the keys of elements, if needed.
} List;

// User interface to the List type.
//--------------------------------------------------------------------------------------------------
List *createList(int(*comp)(Word, Word), void(*del)(Word), String(*getKey)(Word));  // Create and
                                                                        // return a list.
void setSortThreshold(int);  // Set the sort threshold property for the list.
void setKeepSorted(bool);  // Set the keep sorted property for the list.
void deleteList(List*);  // Delete the list.
void emptyList(List*);  // Make the List empty.
void appendListElement(List*, Word);  // Append an element to the list.
void prependListElement(List*, Word);  // Prepends an element to the list.
bool insertListElement(List*, int, Word);  // Insert an element at a fixed location in the list.
Word getListElement(List*, int index);        //  Get an element from a fixed location in a list.
void setListElement(List*, int index, Word);  //  Set an element in a list.
bool insertSortedListElement(List*, Word);  // Insert an element in a sorted List.

bool removeListElementWithKey(List*, String);  // Remove the first list element with the given key.
Word removeListElement(List*, int);  // Remove the ith element from the list.
Word removeFirstListElement(List*);  // Remove the first element from the list.
Word removeLastListElement(List*);  // Remove the last element from the list.

void sortList(List*, bool force);  // Sort a list. Force the sort if below threshold.
void uniqueList(List*);  // Remove duplicate elements (have the same key) from a list.
Word isInList(List*, Word);  // Check if an element is in the list; only check key.
void showList(List*, String(*describe)(Word));  // Show the contents of a list.
void iterateList(List*, void(*iterate)(Word));  // Iterate the list elements running a function.
int lengthList(List*);  // Return the length of the list.
bool isEmptyList(List*);  // Check if a list is empty.

Word searchList(List*, Word element, int *index);
// TODO: These four functions should be static.
Word linearSearchList(List*, Word element, int *index);
Word binarySearchList(List*, Word element, int *index);
Word linearSearchListWithKey(List*, String key, int *index);
Word binarySearchListWithKey(List*, String key, int *index);

#define FORLIST(list, value)\
{\
    Word value;\
    Word *_values = (Word*) list->data;\
    for (int _i = 0; _i < list->length; _i++) {\
        value = _values[_i];\
        {
#define ENDLIST\
        }\
    }\
}

#endif // list_h
