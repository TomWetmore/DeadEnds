//
//  DeadEnds
//
//  sort.c -- Implements quick sort on an array of pointers to any kind of elements. A function
//    that compares pairs of elements must be provided.
//
//  Created by Thomas Wetmore on 21 November 2022.
//  Last changed on 4 November 2023.
//

#include "standard.h"
#include "sort.h"

static bool debugging = false;

#define LNULL -1

// ldata and lcmp -- State variables that simplify the interfaces to the sort functions.
//--------------------------------------------------------------------------------------------------
Word *ldata;              // The data to be sorted.
int (*lcmp)(Word, Word);  // The compare function.

//  Prototypes for the quick sort functions.
//--------------------------------------------------------------------------------------------------
void quickSort(int left, int right);   // External interface (after ldata and lcmp are set).
static int getPivot(int left, int right);  //  Internal sort support function.
static int partition(int left, int right, Word pivot);  // Internal sort support function.

//  quickSort -- Recursive core of quick sort.
//--------------------------------------------------------------------------------------------------
void quickSort(int leftIndex, int rightIndex)
// int leftIndex -- Index of left element in current partition.
// int rightIndex -- Index of right element in currnet partition.
{
	int pivotIndex = getPivot(leftIndex, rightIndex);
	if (debugging)
		printf("quickSort: left=%d, right=%d, pivot=%d\n", leftIndex, rightIndex, pivotIndex);
	if (pivotIndex != LNULL) {
		Word pivot = ldata[pivotIndex];
		int midIndex = partition(leftIndex, rightIndex, pivot);
		quickSort(leftIndex, midIndex-1);
		quickSort(midIndex, rightIndex);
	}
}

//  partition -- Partition around pivot.
//--------------------------------------------------------------------------------------------------
static int partition(int left, int right, Word pivot)
{
	int i = left, j = right;
	do {
		Word tmp = ldata[i];
		ldata[i] = ldata[j];
		ldata[j] = tmp;
		while ((*lcmp)(ldata[i], pivot) < 0) i++;
		while ((*lcmp)(ldata[j], pivot) >= 0) j--;
	} while (i <= j);
	return i;
}

//  getPivot -- Choose the pivot element.
//--------------------------------------------------------------------------------------------------
static int getPivot(int left, int right)
{
	Word pivot = ldata[left];
	int left0 = left, rel;
	for (++left; left <= right; left++) {
		Word next = ldata[left];

		if ((rel = (*lcmp)(next, pivot)) > 0) return left;
		if (rel < 0) return left0;
	}
	return LNULL;  // All elements between left and right are the same so no sorting needed.
}

//  searchList -- Search a list for a value. The List must have a compare function. If the list
//    is not yet sorted and its size is less than the sort threshold linear search is used.
//    If the the list is a sorted list beyond the threshold, it is sorted if need be, and
//    binary search is used.
//
//    If the output index argument is non-null it is set to the index of the found element if
//    there. Otherwise the index will be the location where the element would have been if there.
//--------------------------------------------------------------------------------------------------
Word searchList(List *list, Word element, int* index)
//  list -- List to search.
//  value -- Value to search for.
//  index -- (out) Index of entry, if found; where it would be, if not. Optional.
{
	if (list == null) return null;
	//return linearSearchList(list, element, index);
	// Check whether to use linear search.
	if (!list->keepSorted || (!list->isSorted && list->length < list->sortThreshold)) {
		return linearSearchList(list, element, index);
	}

	// Otherwise sort the list and use binary search..
	sortList(list, true);
	return binarySearchList(list, element, index);
}

Word searchListWithKey(List *list, String key, int* index)
//  list -- List to search.
//  key -- Key of the element to find.
//  index -- Optional index of entry, if found; where it would be, if not.
{
	if (list == null) return null;
	// Check whether to use linear search.
	if (!list->keepSorted || (!list->isSorted && list->length < list->sortThreshold)) {
		return linearSearchListWithKey(list, key, index);
	}
	// Otherwise sort the list and use binary search..
	sortList(list, true);
	return binarySearchListWithKey(list, key, index);
}

// linearSearchList -- Use linear search to search for a value in a list.
//--------------------------------------------------------------------------------------------------
Word linearSearchList(List *list, Word element, int *index)
{
	ASSERT(list && list->getKey);
	if (index) *index = 0;
	String key = list->getKey(element);
	Word *data = list->data;
	for (int i = 0; i < list->length; i++) {
		if (eqstr(key, list->getKey(data[i]))) {
			if (index) *index = i;  // The key was found in the list.
			return data[i];        }

	}
	return null;  // The key was not found in the list.
}

// linearSearchListWithKey -- Use linear search to search for a value in a list.
//--------------------------------------------------------------------------------------------------
Word linearSearchListWithKey(List *list, String key, int *index)
{
	if (index) *index = 0;
	if (!list || !list->getKey) return null;  // TODO: Is it okay for the list to be null?
	//String key = list->getKey(element);
	Word *data = list->data;
	for (int i = 0; i < list->length; i++) {
		if (eqstr(key, list->getKey(data[i]))) {
			if (index) *index = i;  // The key was found in the list.
			return data[i];

		}
	}
	return null;  // The key was not found in the list.
}

// binarySearchList -- Use binary search to search for a value in a list.
//--------------------------------------------------------------------------------------------------
Word binarySearchList(List *list, Word value, int *index)
{
	int lo = 0;
	int hi = list->length - 1;
	while (lo <= hi) {
		int md = (lo + hi)/2;
		int rel = list->compare(value, list->data[md]);
		if (rel < 0) hi = --md;
		else if (rel > 0) lo = ++md;
		else {
			if (index) *index = md;
			return list->data[md];
		}
	}
	// If the element isn't in the list, set index to where it would have been.
	if (index) *index = lo;
	return null;
}

Word binarySearchListWithKey(List *list, String key, int *index)
{
	int lo = 0;
	int hi = list->length - 1;
	while (lo <= hi) {
		int md = (lo + hi)/2;
		int rel = strcmp(key, list->getKey(list->data[md]));
		if (rel < 0) hi = --md;
		else if (rel > 0) lo = ++md;
		else {
			if (index) *index = md;
			return list->data[md];
		}
	}
	// If the element isn't in the list, set index to where it would have been.
	if (index) *index = lo;
	return null;
}

