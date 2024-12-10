// DeadEnds
//
// gnodeindex.c implemements the GNodeIndex data type. A GNodeIndex is a HashTable whose
// elements are tuples of a GNode root and an arbitrary data object. The keys are the keys of
// the root GNodes. The objects are arbitrary; it they should be deleted when the GNodeIndex
// is deleted, a delete function should be passed to the createGNodeINdex function. Otherwise
// pass null.
//
// A GNodeIndex is suited for special purpose indexes of Gedcom records/roots.
//
// Created by Thomas Wetmore on 6 October 2024.
// Last changed on 10 December 2024.

#include <stdio.h>
#include "gnodeindex.h"

// compare compares the keys from two Gnode index elements.
static int compare(String left, String right) {
	return compareRecordKeys(left, right);
}

// getKey returns the key of a GNode index element.
static String getKey(void* element) {
	return ((GNodeIndexEl*) element)->root->key;
}

// createGNodeIndexEl creates an element for a GNodeIndex hash table.
GNodeIndexEl* createGNodeIndexEl(GNode* root, void* data) {
	GNodeIndexEl*  element = (GNodeIndexEl*) stdalloc(sizeof(GNodeIndexEl));
	element->root = root;
	element->data = data;
	return element;
}

// createGNodeIndex creates a HashTable that indexes GNode roots with arbitrary data. Set delete
// to a function if the data should be deleted when the table is deleted.
GNodeIndex* createGNodeIndex(void (*delete)(void*)) {
	return createHashTable(getKey, compare, delete, 1207);
}

// addToGnodeIndex adds a GNode and arbitrary data pair to a GNodeIndex.
void addToGNodeIndex(GNodeIndex* index, GNode* gnode, void* data) {
	addToHashTable(index, createGNodeIndexEl(gnode, data), false);
}

// searchGNodeIndex searches a GNodeIndex for the GNode with the given key.
GNode* searchGNodeIndex(GNodeIndex* index, String key) {
	GNodeIndexEl* el = searchHashTable(index, key);
	if (!el || !el->root) return null;
	return el->root;
}

// showGNodeIndex shows a GNodeIndex; for debugging.
void showGNodeIndex(GNodeIndex* index, void(*show)(void*)) {
	printf("GNodeIndex:\n");
	FORHASHTABLE(index, el)
		GNodeIndexEl* element = el;
		printf("\t%s: ", element->root->key);
		if (show) (*show)(element->data);
	ENDHASHTABLE
}
