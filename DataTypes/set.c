//
//  DeadEnds
//
//  set.c -- Functions that implement Set objects. A Set consists of sortable List. The elements
//  can be anything that can be referred to as a Word. When a set is created there must be a
//  compare function that knows the structure of the Set's elements. The elements in the Set are
//  kept unique via the compare function.
//
//  Created by Thomas Wetmore on 22 November 2022.
//  Last changed 4 November 2023.
//

#include "set.h"
#include "list.h"
#include "sort.h"

// createSet -- Create a set.
//--------------------------------------------------------------------------------------------------
Set *createSet(int (*compare)(Word, Word), void (*delete)(Word), String(*getKey)(Word))
//  compare -- Function that compares pairs of elements; manadatory.
//  delete -- Function to call on elements when they are removed; optional.
{
	ASSERT(compare);
	Set *set = (Set*) stdalloc(sizeof(Set));
	set->list = createList(compare, delete, getKey);
	return set;
}

// deleteSet -- Remove a set.
//--------------------------------------------------------------------------------------------------
void deleteSet(Set *set)
//  set -- Set to remove.
{
	deleteList(set->list);
	stdfree(set);
}

//  addToSet -- Add an element to a set if it is not already there.
//    TODO: SHOULD NAME BE CHANGED TO INSERTSET TO BE CONSISTENT WITH OTHER TYPES?
//--------------------------------------------------------------------------------------------------
void addToSet(Set *set, Word element)
//  set -- Set to add the element to.
//  element -- Element to add to the set if it is not already there.
{
	int index;
	Word entry = searchList(set->list, element, &index);
	if (!entry) insertListElement(set->list, index, element);
}

// Check if an element is in a set. Delegate to the list. Delegate to the list.
//--------------------------------------------------------------------------------------------------
bool isInSet(Set *set, Word element)
{
	return isInList(set->list, element);
}

//  removeFromSet -- Remove an element from a set.
//    NOTE: Shouldn't this use removeFromList?
//--------------------------------------------------------------------------------------------------
void removeFromSet(Set *set, Word element)
{
	int index;
	Word entry = searchList(set->list, element, &index);
	if (!entry) return;
	for (int i = index; i < set->list->length - 1; i++) {
		set->list[i] = set->list[i + 1];
	}
	set->list->length--;
}

// iterateSet -- Iterate the elements of a set, calling a function on each. Delegate to the list.
//--------------------------------------------------------------------------------------------------
void iterateSet(Set *set, void (*iterate)(Word))
{
	iterateList(set->list, iterate);
}

// lengthSet -- Return the number of elements in a set. Delegate to the list.
//--------------------------------------------------------------------------------------------------
int lengthSet(Set *set)
{
	return lengthList(set->list);
}

// showSet -- Show the contents of a set using a describe function. Delegate to the list.
//--------------------------------------------------------------------------------------------------
void showSet(Set *set, String (*describe)(Word))
{
	showList(set->list, describe);
}
