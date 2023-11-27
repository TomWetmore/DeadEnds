//
//  DeadEnds
//
//  functiontable.c -- The DeadEnds function and procedure table datatype.
//
//  Created by Thomas Wetmore on 9 April 2023.
//  Last changed on 3 June 2023.
//

#include "functiontable.h"

static bool debugging = false;

//  compareFunctionKeys -- The compare function for function table.
//--------------------------------------------------------------------------------------------------
static int compareFunctionKeys(Word leftElement, Word rightElement)
{
	String a = ((FunctionElement*) leftElement)->name;
	String b = ((FunctionElement*) rightElement)-> name;
	return strcmp(a, b);
}

// getFunctionKey -- The getKey function for function tables.
//--------------------------------------------------------------------------------------------------
static String getFunctionKey(Word element)
{
	FunctionElement *funcEl = (FunctionElement*) element;
	return funcEl->name;
}

//  deleteFunctionElement -- The delete function for function tables.
//  MNOTE: The name should be deleted, but keep the node for the database to delete.
//--------------------------------------------------------------------------------------------------
static void deleteFunctionElement(Word element)
//  element -- An element in a function table.
{
	FunctionElement *funcEl = (FunctionElement*) element;
	//  Delete the name since no reference to it should exist.
	stdfree(funcEl->name);
	//  Delete the function or procedure since it can no longer be accessed.
	freePNodes(funcEl->function);
}

//  createFunctionTable --  Create a new function table.
//--------------------------------------------------------------------------------------------------
FunctionTable *createFunctionTable(void)
{
	return createHashTable(compareFunctionKeys, deleteFunctionElement, getFunctionKey);
}

//  searchFunctionTable -- Search a function table for the function or procedure that has a
//    given name. Return null if the function is not found.
//--------------------------------------------------------------------------------------------------
PNode *searchFunctionTable(FunctionTable *table, String name)
//  table -- Function table to search.
//  name -- Name of the function or procedure to search for.
{
	FunctionElement *element = (FunctionElement*) searchHashTable(table, name);
	return element ? element->function : null;
}

//  insertInFunctionTable -- Insert a new function or procedure into a function table.
//--------------------------------------------------------------------------------------------------
void insertInFunctionTable(FunctionTable *table, String name, PNode *pnode)
//  table -- Function table to add the element to.
//  name -- Name of the function or procedure.
//  pnode -- Root program node of the function or procedure.
{
	ASSERT(table && name && pnode);
	if (debugging) printf("Adding %s to a function table.\n", name);

	//  See if a function with the name already exists.
	//  TODO: Should this be an error that prevents the program from running?
	FunctionElement *element = (FunctionElement*) searchHashTable(table, name);
	if (element) {
		printf("Multiply defined function or procedure named %s; new copied over old.\n", name);
		removeElement(table, element);
	}

	//  Create a new element for the table and add it.
	//  MNOTE: The function name and root nodes are not saved as they are in the heap.
	element = (FunctionElement*) stdalloc(sizeof(FunctionElement));
	element->name = name;
	element->function = pnode;
	insertInHashTable(table, element);
}
