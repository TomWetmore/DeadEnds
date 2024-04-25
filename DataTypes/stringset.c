// DeadEnds
//
// stringset.c
//
// Created by Thomas Wetmore on 20 April 2024.
// Last changed on 22 April 2024.

#include "stringset.h"

// getKey is the getKey function for StringSets.
static String getKey(void* element) {
	return (String) element;
}

// compare is the compare function for StringSets.
static int compare(String element1, String element2) { return strcmp(element1, element2); }

// createStringSet creates a StringSet.
StringSet* createStringSet(void) {
	return createSet(getKey, compare, null);
}
