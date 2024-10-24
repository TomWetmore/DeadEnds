// DeadEnds
//
// gnodeindex.c has the GNodeIndex functions. A GNodeIndex is a HashTable whose elements
// are tuples of a GNode root and an arbitrary data object. The keys are the keys of the
// root GNodes. The objects are arbitrary; it they should be deleted when the GNodeIndex
// is deleted, a delete function must be passed to the createGNodeINdex function. Otherwise
// pass null for the delete function.
//
// A GNodeIndex is well suited for creating any special purpose index of Gedcom records/roots.
//
// Created by Thomas Wetmore on 6 October 2024.
// Last changed on 22 October 2024.

#include <stdio.h>
#include "gnodeindex.h"

// compare compares the keys from to Gnode index elements.
static int compare(String left, String right) {
	return compareRecordKeys(left, right);
}

// getKey returns the key of a GNode index element.
static String getKey(void* element) {
	return ((GNodeIndexEl*) element)->root->key;
}

// delete deletes the contents of a GNode index element. By default it does nothing; if one is
// required the user must set it.
static void defaultDelete(void* word) { }

// createGNodeIndexEl creates an element for a GNodeIndex hash table.
GNodeIndexEl* createGNodeIndexEl(GNode* root, void* data) {
	GNodeIndexEl*  element = (GNodeIndexEl*) stdalloc(sizeof(GNodeIndexEl));
	element->root = root;
	element->data = data;
	return element;
}

// createGNodeIndex create a hash table that indexes GNode roots with arbitrary void* data.
GNodeIndex* createGNodeIndex(void (*delete)(void*)) {
	if (delete)	return createHashTable(getKey, compare, delete, 1207);
	return createHashTable(getKey, compare, defaultDelete, 1207);
}

// addToGnodeIndex adds GNode and DataConnect pair to a GNodeIndex.
void addToGNodeIndex(GNodeIndex* index, GNode* gnode, void* data) {
	addToHashTable(index, createGNodeIndexEl(gnode, data), false);
}

// showGNodeIndex shows a GNodeIndex; it is intended for debugging.
void showGNodeIndex(GNodeIndex* index, void(*show)(void*)) {
	printf("GNodeIndex:\n");
	if (show) {
		FORHASHTABLE(index, el)
			GNodeIndexEl* element = el;
			printf("\t%s: ", element->root->key);
			(*show)(element->data);
		ENDHASHTABLE
	}
}

