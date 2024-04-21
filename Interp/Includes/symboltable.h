//
// DeadEnds
//
// symboltable.h -- Header file for the symbol tables that hold the values of variables in
//    DeadEnds programs. Symbol tables are implented with hash tables.
//
// Created by Thomas Wetmore on 23 March 2023.
// Last changed on 2 April 2024.
//

#ifndef symboltable_h
#define symboltable_h

typedef struct PValue PValue; // Forward reference.

#include "standard.h"
#include "hashtable.h"
#include "pvalue.h"

// A SymbolTable holds DeadEnds script variables and their values.
typedef HashTable SymbolTable;

// A Symbol is an element of a SymbolTable. ident is the name of a variable/identifier, and
// value is its value.
typedef struct {
	String ident;
	PValue *value;
} Symbol;

//  User interface to symbol tables.
SymbolTable *createSymbolTable(void);
void assignValueToSymbol(SymbolTable*, String, PValue);
PValue getValueOfSymbol(SymbolTable*, String);
void showSymbolTable(SymbolTable*);

#endif // symboltable_h
