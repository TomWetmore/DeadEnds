//
//  functiontable.h
//  JustParsing
//
//  Created by Thomas Wetmore on 9 April 2023.
//  Last changed on 1 June 2023.
//

#ifndef functiontable_h
#define functiontable_h

#include "standard.h"
#include "pnode.h"

//  FunctionTable -- Hash table that holds DeadEnds procedures and user-defined functions.
//--------------------------------------------------------------------------------------------------
typedef HashTable FunctionTable;

//  FuncElement -- Elements for the interpreter's table of procedures and user-defined functions.
//--------------------------------------------------------------------------------------------------
typedef struct {
	String name;      //  Name of the procedure or function.
	PNode *function;  //  Root program node of the procedure or function.
} FunctionElement;

FunctionTable *createFunctionTable(void);
PNode *searchFunctionTable(FunctionTable *table, String name);
void insertInFunctionTable(FunctionTable *table, String name, PNode *pnode);

#endif // functiontable_h
