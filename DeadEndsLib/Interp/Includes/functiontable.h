//
//  DeadEnds Library
//  functiontable.h
//
//  Created by Thomas Wetmore on 9 April 2023.
//  Last changed on 4 June 2023.
//

#ifndef functiontable_h
#define functiontable_h

#include "standard.h"

typedef struct HashTable HashTable;
typedef struct PNode PNode;

// FunctionTable is the hash table that holds DeadEnds procedures and user-defined functions.
typedef HashTable FunctionTable;

// FuncElement is an element of a FunctionTable.
typedef struct {
	String name;      //  Name of the procedure or function.
	PNode *function;  //  Root program node of the procedure or function.
} FunctionElement;

FunctionTable *createFunctionTable(void);
PNode *searchFunctionTable(FunctionTable *table, String name);
void addToFunctionTable(FunctionTable *table, String name, PNode *pnode);

#endif // functiontable_h
