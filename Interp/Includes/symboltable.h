//
//  DeadEnds
//
//  symboltable.h -- Header file for the symbol tables that hold the values of variables in
//    DeadEnds programs. Symbol tables are implented with hash tables.
//
//  Created by Thomas Wetmore on 23 March 2023.
//  Last changed on 15 May 2023.
//

#ifndef symboltable_h
#define symboltable_h

#include "standard.h"
#include "hashtable.h"
#include "pvalue.h"

//  SymbolTable -- Symbol tables hold program variables and their values. These are used by
//    the interpreter. They are an implementation of hash table.
//--------------------------------------------------------------------------------------------------
typedef HashTable SymbolTable;

//  Symbol -- Symbols are the elements in symbol tables.
//--------------------------------------------------------------------------------------------------
typedef struct {
    String ident;   //  The variable (or identifier); this is the element's the key value.
    PValue *value;  //  The variable's value; a pointer to a program value.
} Symbol;

//  User interface to symbol tables.
//--------------------------------------------------------------------------------------------------
SymbolTable *createSymbolTable(void);  //  Create a symbol table.
void assignValueToSymbol(SymbolTable*, String ident, PValue value);  //  Assign a value to an ident.
PValue getValueOfSymbol(SymbolTable*, String ident);  //  Get the value of an ident from a symbol table.

void showSymbolTable(SymbolTable*);  //  Show the contents of a symbol table. For debugging.

#endif // symboltable_h
