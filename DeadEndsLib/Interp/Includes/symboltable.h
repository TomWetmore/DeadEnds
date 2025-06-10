//
//  DeadEnds Library
//
//  symboltable.h is the header file for the symbol tables that hold the values of variables in
//  DeadEnds programs. Symbol tables are implented with hash tables.
//
//  Created by Thomas Wetmore on 23 March 2023.
//  Last changed on 6 June 2025.
//

#ifndef symboltable_h
#define symboltable_h

typedef struct PValue PValue;
typedef struct PNode PNode;
typedef struct Frame Frame;
typedef struct Context Context;
typedef struct HashTable HashTable;

// A SymbolTable holds DeadEnds script variables and their PValues.
typedef HashTable SymbolTable;

// A Symbol is an element in a SymbolTable; ident is the name of the symbol, and value is its PValue.
typedef struct Symbol {
	String ident;
	PValue *value;
} Symbol;

//  Interface to SymbolTable.
SymbolTable *createSymbolTable(void);
void deleteSymbolTable(SymbolTable*);
void assignValueToSymbol(Context*, String, PValue);
void assignValueToSymbolTable(SymbolTable*, String, PValue);
PValue getValueOfSymbol(Context*, String);
PValue getValueFromSymbolTable(SymbolTable*, String);
void showSymbolTable(SymbolTable*); // Debug.

#endif // symboltable_h
