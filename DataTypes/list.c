//
//  DeadEnds
//
//  list.c -- Implements an array-based list. Lists grow automatically when needed. Lists can
//    be sorted or unsorted. New elements can be appended to the either end or inserted at
//    a specific location. Sorted lists require a compare function to be provided to when the
//    list is created.
//
//  Created by Thomas Wetmore on 22 November 2022.
//  Last changed on 1 November 2023.
//

#include "list.h"
#include "sort.h"  // sortList.

static bool debugging = false;

//  Local function.
//--------------------------------------------------------------------------------------------------
static void growList(List*);

// createList -- Create an array-based List.
//--------------------------------------------------------------------------------------------------
List *createList(int (*compare)(Word, Word), void (*delete)(Word), String(*getKey)(Word))
//  compare -- Compare function for elements; required for sorted lists.
//  delete -- Optional delete function for elements.
//  getkey -- Optional get key function for elements.
{
	List *list = (List*) stdalloc(sizeof(List));
	list->isSorted = true;  // An empty List is a sorted List.
	list->keepSorted = false;  // By default don't keep the list sorted.
	list->sortThreshold = 30;  // A reasonable value if the list is sorted.
	list->length = 0;
	list->maxLength = INITIAL_SIZE_LIST_DATA_BLOCK;
	list->data = (Word) stdalloc(INITIAL_SIZE_LIST_DATA_BLOCK*sizeof(Word));
	list->compare = compare;
	// If there is a compare function assume the list should be sorted.
	if (compare) list->keepSorted = true;
	list->delete = delete;
	list->getKey = getKey;
	return list;
}

//  deleteList -- Delete a list. If there is a delete function call it on each element.
//--------------------------------------------------------------------------------------------------
void deleteList(List *list)
{
	ASSERT(list);
	if (list->delete) {
		for (int i = 0; i < list->length; i++) {
			list->delete(list->data[i]);
		}
	}
	stdfree(list->data);  // Free the list's elements.
	stdfree(list);  // Free the list.
}

//  emptyList -- Make a list empty. If there is a delete function call it on each element.
//--------------------------------------------------------------------------------------------------
void emptyList(List *list)
{
	if (!list) return;
	if (list->delete) {
		for (int i = 0; i < list->length; i++) {
			list->delete(list->data[i]);
		}
	}
	list->length = 0;
}

//  isEmptyList -- See if a list is empty.
//--------------------------------------------------------------------------------------------------
bool isEmptyList(List *list)
{
	return list->length <= 0;
}

//  appendListElement -- Add a new value to the end of the list.
//--------------------------------------------------------------------------------------------------
void appendListElement(List *list, Word value)
{
	ASSERT(list);
	list->isSorted = false;
	if (list->length >= list->maxLength) growList(list);
	list->data[(list->length)++] = value;
}

//  prependListElement -- Add a new element to the start of the list.
//--------------------------------------------------------------------------------------------------
void prependListElement(List *list, Word value)
{
	ASSERT(list);
	list->isSorted = false;
	int length = list->length;
	if (length >= list->maxLength) growList(list);
	Word* data = list->data;
	for (int i = length; i > 0; i--) {
		data[i] = data[i - 1];
	}
	data[0] = value;
	(list->length)++;
}

//  setListElement -- Set a specific element in a list.
//--------------------------------------------------------------------------------------------------
void setListElement(List *list, int index, Word value)
{
	ASSERT(list && index >= 0 && index < list->length);
	list->data[index] = value;
}

//  getListElement -- Get a specific element from a list.
//--------------------------------------------------------------------------------------------------
Word getListElement(List *list, int index)
{
	ASSERT(list && index >= 0 && index < list->length);
	return list->data[index];
}

//  insertListElement -- Insert an element into a list at a specified location.
//--------------------------------------------------------------------------------------------------
bool insertListElement(List *list, int index, Word value)
{
	ASSERT(list && index >= 0 && index <= list->length);
	list->isSorted = false;
	int length = list->length;
	if (index == length) {
		appendListElement(list, value);
		return true;
	}
	if (length >= list->maxLength) growList(list);
	// Shift elements to make space for the new element
	for (int i = list->length; i > index; i--) {
		(list->data)[i] = (list->data)[i - 1];
	}
	(list->data)[index] = value;
	(list->length)++;
	return true;
}

//  insertSortedListElement -- Insert an element into its proper location in a sorted List.
//--------------------------------------------------------------------------------------------------
bool insertSortedListElement(List *list, Word value)
{
	//  Be sure the list exists, should be sorted, and has a compare function.
	ASSERT(list && list->keepSorted && list->compare);

	// If the list isn't sorted, sort it before inserting.
	if (!list->isSorted) sortList(list, true);

	//  Search the list for the value. If there or not there insert at the returned index. This
	//    can allow duplicates to occur.
	int index;
	searchList(list, value, &index);
	insertListElement(list, index, value);
	list->isSorted = true;
	return true;
}

//  removeListElement -- Remove an indexed value from a list. This does not affect the sorted
//    state of the list. If the list is empty return the null pointer.
//  MNOTE: The caller takes responsibility to delete the removed element.
//--------------------------------------------------------------------------------------------------
Word removeListElement(List *list, int index)
{
	ASSERT(list && index >= 0);
	int length = list->length;
	if (length == 0) return null;
	Word* array = list->data;
	Word element = array[index];  // Element to remove from  list.
	for (; index < length - 1; index++)
		array[index] = array[index + 1];  // Fill the hole.
	(list->length)--;
	return element;
}

//  removeFirstListElement -- Remove the first element from a list.
//-------------------------------------------------------------------------------------------------
Word removeFirstListElement(List *list)
{
	ASSERT(list);
	if (list->length <= 0) return null;
	return removeListElement(list, 0);
}

//  removeLastListElement -- Remove the last element from a list.
//-------------------------------------------------------------------------------------------------
Word removeLastListElement(List *list)
{
	ASSERT(list);
	if (list->length <= 0) return null;
	return removeListElement(list, list->length - 1);
}

//  showList -- Show the contents of a List. Intended for debugging. If the describe function is
//    not null, call it to get the String from of the element to print. Otherwise assume the
//    elements are strings.
//--------------------------------------------------------------------------------------------------
void showList(List *list, String (*describe)(Word))
//  list -- List to show.
//  describe -- Function to the print each element in the list.
{
	printf("Show a list: curlen = %d; maxlen = %d\n", list->length, list->maxLength);
	for (int i = 0; i < list->length; i++) {
		if (describe) printf("%s\n", describe(list->data[i]));
		else printf("%s\n", (String) list->data[i]);
	}
}

//  uniqueList -- Remove duplicates from a List. The List is sorted if it is not already.
//    Uniqueness is defined by the compare function. The list will be sorted even if its keep
//    sorted flag is not set or its length is less that the sort threshold.
//--------------------------------------------------------------------------------------------------
void uniqueList(List *list)
{
	// The List must exist and must have a compare function.
	ASSERT(list && list->compare);
	if (!list->isSorted) sortList(list, true);
	Word* d = list->data;
	int i, j;
	for (j = 0, i = 1; i < list->length; i++) {
		if (list->compare(d[i], d[j]) != 0) {
			if (i != j + 1) d[++j] = d[i];
		}
	}
	list->length = j + 1;
}

//  isInList -- Check if an element is in the list. The List is sorted if it is not already. The
//    element, if found, is returned, and should be cast to its real type.
//--------------------------------------------------------------------------------------------------
Word isInList(List *list, Word value)
{
	// List must exist and be sortable.
	ASSERT(list && list->compare);
	if (!list->isSorted) sortList(list, false);
	return searchList(list, value, null);
}

//  iterateList -- Iterate the elements of a list doing something.
//--------------------------------------------------------------------------------------------------
void iterateList(List *list, void(*iterate)(Word))
{
	if (!iterate) return;  // Do nothing if there is no iteration function.
	for (int i = 0; i < list->length; i++)
		(*iterate)(list->data[i]);
}

//  lengthList -- Return the length of the list.
//--------------------------------------------------------------------------------------------------
int lengthList(List *list) { return list->length; }

//  growList -- Static function that grows the data block of a list when required. The new length
//    is three halves the previous length, for no particular reason.
//--------------------------------------------------------------------------------------------------
static void growList(List *list)
{
	int newLength = list->maxLength = (3*list->maxLength)/2;
	Word newData = stdalloc(newLength*sizeof(Word));
	memcpy(newData, list->data, (list->length)*sizeof(Word));
	stdfree(list->data);
	list->data = newData;
}

//  sortList -- Sort a list. If force is true sort the list regardless of its length. If force
//    is false, sort the list if its length is above the sort threshold.
//--------------------------------------------------------------------------------------------------
void sortList(List *list, bool force)
//  list -- List to sort.
//  force -- If true always sort; else sort if at or above threshold.
{
	if (debugging) printf("sortList: on list of length %d\n", list->length);
	ASSERT(list && list->compare);
	if (!list->keepSorted) {
		if (debugging) printf("sortList: list is not to be sorted: returning.\n");
		return;
	}
	if (list->isSorted) {
		if (debugging) printf("sortList: list is sorted so returning\n");
		return;
	}
	if (!force && list->length < list->sortThreshold) {
		if (debugging) printf("sortList: list is below threshold so not being sorted\n");
		return;
	}
	if (debugging) printf("sortList: data is being sorted.\n");
	ldata = list->data;    //  ldata and lcmp are globals used by the quickSort function.
	lcmp = list->compare;  //    TODO: Would be nice to not need this artifice.
	quickSort(0, list->length - 1);
	list->isSorted = true;
}
