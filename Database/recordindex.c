//
// DeadEnds
//
// recordindex.c holds the functions that implement RecordIndex, a table that maps Gedcom record
// keys to the roots of the GNode trees of those records. RecordIndex is built on HashTable.
//
// Created by Thomas Wetmore on 29 November 2022.
// Last changed on 29 April 2024.

#include "recordindex.h"
#include "list.h"
#include "sort.h"
#include "gedcom.h"

static int numRecordIndexBuckets = 2048;

// compare is the structure function that compares RecordIndexEl elements.
static int compare(String left, String right) {
	return compareRecordKeys(left, right);
}

// delete is the structure function that deletes RecordIndexEl elements.
static void delete(void *word) {
	RecordIndexEl* element = (RecordIndexEl*) word;
	stdfree(element);
}

// getKey returns the key of a RecordIndexEl; it is the key of the root GNode.
static String getKey(void* word) {
	return ((RecordIndexEl*) word)->root->key;
}

// createRecordIndex creates and returns a RecordIndex.
RecordIndex *createRecordIndex(void) {
	return createHashTable(getKey, compare, delete, numRecordIndexBuckets);
}

// deleteRecordIndex delete a RecordIndex.
void deleteRecordIndex(RecordIndex *index) {
	deleteHashTable(index);
}

// addToRecordIndex adds a (root, lineNumber) element to a RecordIndex.
void addToRecordIndex(RecordIndex* index, String key, GNode* root, int lineNumber) {
	RecordIndexEl* element = (RecordIndexEl*) searchHashTable(index, key);
	if (element) return;
	element = (RecordIndexEl*) malloc(sizeof(RecordIndexEl));
	element->root = root;
	element->lineNumber = lineNumber;
	addToHashTable(index, element, false);
}

// searchRecordIndex searches a RecordIndex by key and returns the associated GNode tree.
GNode* searchRecordIndex(RecordIndex *index, String key) {
	RecordIndexEl* element = searchHashTable(index, key);
	return element == null ? null : element->root;
}

// showRecordIndex show the contents of a RecordIndex.
void showRecordIndex(RecordIndex *index) {
	for (int i = 0; i < numRecordIndexBuckets; i++) {
		if (!index->buckets[i]) continue;
		Bucket *bucket = index->buckets[i];
		Block *block = &(bucket->block);
		void **elements = block->elements;
		printf("Bucket %d\n", i);
		for (int j = 0; j < block->length; j++) {
			RecordIndexEl* element = elements[j];
			printf("    Key %s\n", element->root->key);
		}
	}
}
