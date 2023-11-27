//
//  DeadEnds
//
//  set.h -- Header file for the Set type.
//
//  Created by Thomas Wetmore on 22 November 2022.
//  Last changed on 3 November 2023.
//

#ifndef set_h
#define set_h

#include "standard.h"
#include "list.h"

//  Set -- The set type. It uses a sortable list as its elements. Elements are pointers to objects
//    the user defines. The user must provide a compare function to keep the elements sorted.
//    The user can provide a delete function to dispose of the elements when the set is removed.
//    Duplicate elements, as detected by the compare function, are not added.
//--------------------------------------------------------------------------------------------------
typedef struct Set {
    List *list;  // Sortable list holding the set's elements.
} Set;

// User interface.
// TODO: REMOVE FROM SET NOT YET WRITTEN.
//--------------------------------------------------------------------------------------------------
int lengthSet(Set*);  // Return the number of elements in the Set.
Set *createSet(int(*compare)(Word, Word), void(*delete)(Word), String(*getKey)(Word));  // Create a new Set.
void deleteSet(Set*);  // Remove a Set, calling the delete function, if any, on each element.
void addToSet(Set*, Word);  // Add an element (if new) to the Set.
bool isInSet(Set*, Word);  // Check if an element is in the Set.
void iterateSet(Set*, void(*iterate)(Word));  // Iterate over the Set doing something.
void showSet(Set*, String(*describe)(Word));  // Show the contents of the Set.
void removeFromSet(Set*, Word);  //  Remove an element from the set.

#endif // set_h
