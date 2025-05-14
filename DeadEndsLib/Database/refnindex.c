// DeadEnds
//
// refnindex.c has the functions that implement the reference index. Gedcom records can have
// 1 REFN nodes whose values give records unique identifiers.
//
// Created by Thomas Wetmore on 16 December 2023.
// Last changed on 11 May 2025.

#include "refnindex.h"
#include "gedcom.h"

static int numRefnIndexBuckets = 1024;

// searchRefnIndex searches a RefnIndex for a 1 REFN value and returns the key of the record with
// that value.
String searchRefnIndex(RefnIndex* index, String refn) {
	if (!index || !refn) return null;
	RefnIndexEl* el = (RefnIndexEl*) searchHashTable(index, refn);
	return el ? el->key : null;
}

// createRefnIndexEl creates a new reference index entry.
RefnIndexEl *createRefnIndexEl(String refn, String key) {
	RefnIndexEl *el = (RefnIndexEl*) malloc(sizeof(RefnIndexEl));
	el->refn = refn;
	el->key = key;
	return el;
}

// showRefnIndex show a RefnIndex, for debugging.
void showRefnIndex(RefnIndex* index) {
	printf("showRefnIndex: Write me\n");
}

// compare compares two record keys.
static int compare (String a, String b) {
	return strcmp(a, b);
}

// getKey returns the key of a RefnIndexEl, a 1 REFN value.
static String getKey(void* a) {
	return ((RefnIndexEl*) a)->key;
}

// delete frees a RefnIndexEl.
static void delete(void* element) {
	stdfree(element);
}

// createRefnIndex creates a RefnIndex.
RefnIndex *createRefnIndex(void) {
	return (RefnIndex*) createHashTable(getKey, compare, delete, numRefnIndexBuckets);
}

// deleteRefnIndex deletes a RefnIndex.
void deleteRefnIndex(RefnIndex *index) {
	deleteHashTable(index);
}

// addToRefnIndex adds a new RefnIndexEl to a RefnIndex. Returns true on success; returns false
// if the REFN value is already in the table.
bool addToRefnIndex(RefnIndex *index, String refn, String key) {
	RefnIndexEl* element = createRefnIndexEl(strsave(refn), strsave(key));
	bool added = addToHashTableIfNew(index, element);
	if (added) return true;
	delete(element);
	return false;
}
