// DeadEnds
//
// stringset.c
//
// Created by Thomas Wetmore on 20 April 2024.
// Last changed on 14 July 2024.

#include "stringset.h"

// getKey is the getKey function for StringSets.
static String getKey(void* element) {
	return (String) element;
}

// delete is the optional delete function for StringSets.
static void delete(void* element) {
	stdfree(element);
}

// compare is the compare function for StringSets.
static int compare(String element1, String element2) { return strcmp(element1, element2); }

// createStringSet creates a StringSet.
StringSet* createStringSet(void) {
	return createSet(getKey, compare, null);
}

// deleteStringSet deletes (frees) a string set. If the boolean is set the strings are freed also.
void deleteStringSet(StringSet* set, bool del) {
	if (del) {
		set->list.delete = delete;
		deleteSet(set);
	} else {
		set->list.delete = null;
		deleteSet(set);
	}
}
