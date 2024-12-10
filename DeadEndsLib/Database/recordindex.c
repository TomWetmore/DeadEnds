// DeadEnds
//
// recordindex.c holds the functions that implement RecordIndex, a HashTable that maps Gedcom
// record keys to the roots of the GNode trees with those keys.
//
// Created by Thomas Wetmore on 29 November 2022.
// Last changed on 5 December 2024.

#include "recordindex.h"
#include "list.h"
#include "sort.h"
#include "gedcom.h"

#define numRecordIndexBuckets 2047
#define brownnose true

// compare is the compare function for record keys.
static int compare(String left, String right) {
	if (brownnose) printf("compare[RecordIndex] comparing %s and %s.\n", left, right);
	return compareRecordKeys(left, right);
}

// getKey returns the key of a RecordIndex element.
static String getKey(void* element) {
	return ((GNode*) element)->key;
}

// createRecordIndex creates a RecordIndex. The delete function is null to prevent the trees
// themselves from being deleted.
RecordIndex *createRecordIndex(void) {
	return createHashTable(getKey, compare, null, numRecordIndexBuckets);
}

// deleteRecordIndex deletes a RecordIndex.
void deleteRecordIndex(RecordIndex *index) {
	deleteHashTable(index);
}

// addToRecordIndex adds a GNode record tree to a RecordIndex.
void addToRecordIndex(RecordIndex* index, GNode* root) {
	ASSERT(root && root->key);
	addToHashTable(index, root, false);
}

// searchRecordIndex searches a RecordIndex by key and returns the associated GNode tree.
GNode* searchRecordIndex(RecordIndex *index, String key) {
	return (GNode*) searchHashTable(index, key);
}

// showRecordIndex shows the contents of a RecordIndex. For debugging.
void showRecordIndex(RecordIndex* index) {
	FORHASHTABLE(index, element)
		printf("Key %s\n", ((GNode*) element)->key);
	ENDHASHTABLE
}
