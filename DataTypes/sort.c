//
// DeadEnds
//
// sort.c has the functions that implement the low level sort and search operations on arrays
// of elements.
//
// Created by Thomas Wetmore on 21 November 2022.
// Last changed on 12 April 2024.
//

#include "standard.h"
#include "sort.h"

static bool sortDebugging = false;

//  Internal quick sort functions.
static void quickSort(int, int);
static int getPivot(int left, int right);
static int partition(int left, int right, void *pivot);

// State variables used by the sort functions.
static void** lelements;  // lelements is the array of elements.
static String (*lgetKey)(void*); // static getKey function.
static int (*lcompare)(String, String); // static compare function.

// sortElements is the external interface for sorting an array of elements.
void sortElements(void** elements, int length, String(*getKey)(void*), int(*compare)(String, String))
{
	lelements = elements;
	lcompare = compare;
	lgetKey = getKey;
	quickSort(0, length - 1);
}

static int magicCompare(void* a, void* b) {
	return lcompare(lgetKey(a), lgetKey(b));
}
#define LNULL -1
// quickSort is the recursive function that sorts a partition.
void quickSort(int left, int right) {
	int pivotIndex = getPivot(left, right);
	if (sortDebugging) printf("quickSort: left=%d, right=%d, pivot=%d\n", left, right, pivotIndex);
	if (pivotIndex != LNULL) {
		void *pivot = lelements[pivotIndex];
		int midIndex = partition(left, right, pivot);
		quickSort(left, midIndex-1);
		quickSort(midIndex, right);
	}
}

// partition partitions around a pivot.
static int partition(int left, int right, void* pivot) {
	int i = left, j = right;
	do {
		void* tmp = lelements[i];
		lelements[i] = lelements[j];
		lelements[j] = tmp;
		while (magicCompare(lelements[i], pivot) < 0) i++;
		while (magicCompare(lelements[j], pivot) >= 0) j--;
	} while (i <= j);
	return i;
}

// getPivot chooses the pivot element.
static int getPivot(int left, int right) {
	void* pivot = lelements[left];
	int left0 = left, rel;
	for (++left; left <= right; left++) {
		void* next = lelements[left];

		if ((rel = magicCompare(next, pivot)) > 0) return left;
		if (rel < 0) return left0;
	}
	return LNULL;  // Elements between left and right are equal.
}

// linearSearch searches a list of elements for the one with a matching key.
void* linearSearch(void** elements, int length, String key, String(*getKey)(void*), int* index) {
	ASSERT(elements && key && getKey);
	if (index) *index = 0;
	if (!elements || !key || !getKey) return null;
	for (int i = 0; i < length; i++) {
		if (eqstr(key, getKey(elements[i]))) {
			if (index) *index = i;
			return elements[i];
		}
	}
	return null;
}

// binarySearch search a sorted list of elements for an element with given key.
void* binarySearch(void** elements, int length, String key, String(*getKey)(void*),
				   int(*compare)(String, String), int* index) {
	ASSERT(elements && key && getKey && compare);
	if (index) *index = -1;
	int lo = 0;
	int hi = length - 1;
	while (lo <= hi) {
		int md = (lo + hi)/2;
		int rel = compare(key, getKey(elements[md]));
		if (rel < 0) {
			hi = --md;
		} else if (rel > 0) {
			lo = ++md;
		} else {
			if (index) *index = md;
			return elements[md];
		}
	}
	if (index) *index = lo;
	return null;
}

// insertAtIndex inserts a new element at a specified index. The array must have enough room.
void insertAtIndex(void** elements, int length, void* element, int index) {
	for (int i = length; i > index; i--) {
		elements[i] = elements[i - 1];
	}
	elements[index] = element;
}

