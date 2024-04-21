//
//  DeadEnds
//
//  functiontable.c -- The DeadEnds function and procedure table datatype.
//
//  Created by Thomas Wetmore on 9 April 2023.
//  Last changed on 3 March 2024.

#include "functiontable.h"

static bool debugging = false;

// compare is the compare function for FunctionTables.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// getKey is the getKey function for FunctionTables.
static String getKey(void* element)
{
	FunctionElement *funcEl = (FunctionElement*) element;
	return funcEl->name;
}

//  delete is the delete function for FunctionTables.
static void delete(void* element) {
	FunctionElement *funcEl = (FunctionElement*) element;
	stdfree(funcEl->name); // Delete; no reference to it should exist.
	freePNodes(funcEl->function); // Delete; can no longer be accessed.
}

// createFunctionTable creates a FunctionTable.
static int numBucketsInFunctionTable = 13;
FunctionTable *createFunctionTable(void) {
	return createHashTable(getKey, compare, delete, numBucketsInFunctionTable);
}

// searchFunctionTable searches a function table for the function or procedure with given name.
PNode *searchFunctionTable(FunctionTable *table, String name) {
	FunctionElement *element = (FunctionElement*) searchHashTable(table, name);
	return element ? element->function : null;
}

// addToFunctionTable adds a function or procedure to a function table. Takes the name and the
// root PNode as parameters.
void addToFunctionTable(FunctionTable *table, String name, PNode *pnode) {
	if (debugging) printf("Adding %s to a function table.\n", name);

	//  See if a function with the name already exists.
	//  TODO: Should this be an error that prevents the program from running?
	FunctionElement *element = (FunctionElement*) searchHashTable(table, name);
	if (element)
		printf("Multiply defined function or procedure named %s; new copied over old.\n", name);
	// MNOTE: The function name and root nodes are not saved as they are in the heap.
	element = (FunctionElement*) malloc(sizeof(FunctionElement)); // Create element.
	element->name = name;
	element->function = pnode;
	addToHashTable(table, element, true);
}
