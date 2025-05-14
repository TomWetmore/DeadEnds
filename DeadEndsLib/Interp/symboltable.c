// DeadEnds
//
// symboltable.c holds the functions that implement SymbolTables.
//
// Created by Thomas Wetmore on 23 March 2023.
// Last changed on 11 May 2025.

#include "standard.h"
#include "symboltable.h"

// globalTable holds the Symbols defined in the global scope.
extern SymbolTable* globalTable;

// compare compares two Symbols by ident fields.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// delete deletes a Symbol.
static void delete(void* a) {
	Symbol* symbol = (Symbol*) a;
	stdfree(symbol->value);
}

// getKey returns the Symbol's identifier.
static String getKey(void *symbol) {
	return ((Symbol*) symbol)->ident;
}

// createSymbol creates a Symbol.
static Symbol *createSymbol(String iden, PValue *ppvalue) {
	Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
	symbol->ident = iden;
	symbol->value = ppvalue;
	return symbol;
}

// createSymbolTable creates a SymbolTable.
static int numBucketsInSymbolTable = 37;
SymbolTable* createSymbolTable(void) {
	return createHashTable(getKey, compare, delete, numBucketsInSymbolTable);
}

// assignValueToSymbol assigns a value to a Symbol. If the Symbol isn't in the local table, check
// the global table.
void assignValueToSymbol(SymbolTable* symtab, String ident, PValue pvalue) {
    // Determine symbol table to use.
    SymbolTable* table = symtab;
    if (!isInHashTable(symtab, ident) && isInHashTable(globalTable, ident)) {
        table = globalTable;
    }
    // Prepare the value to put in the symbol table.
    PValue* copy = clonePValue(&pvalue);
    // If the symbol exists free its old value.
    Symbol* symbol = searchHashTable(table, ident);
    if (symbol) {
        freePValue(symbol->value);
        symbol->value = copy;
    // Else add a new symbol with the new value.
    } else {
        addToHashTable(table, createSymbol(ident, copy), true);
    }
}

// getValueOfSymbol gets the value of a Symbol from a SymbolTable; PValue is returned on stack.
PValue getValueOfSymbol(SymbolTable* symtab, String ident) {
    Symbol *symbol = searchHashTable(symtab, ident);
    if (!symbol) symbol = searchHashTable(globalTable, ident);
    if (!symbol || !symbol->value) return nullPValue;

    // Return a clone of the PValue (deep enough)
    return cloneAndReturnPValue(symbol->value);
}

// showSymbolTable shows the contents of a SymbolTable. For debugging.
void showSymbolTable(SymbolTable* table) {
	printf("Symbol Table contents:\n");
	for (int i = 0; i < table->numBuckets; i++) {
		Bucket *bucket = table->buckets[i];
		if (!bucket) continue;
		Block *block = &(bucket->block);
		if (block->length <= 0) continue;
		for (int j = 0; j < block->length; j++) {
			Symbol *symbol = (Symbol*) block->elements[j];
			String pvalue = pvalueToString(*(symbol->value), false);
			printf("  %s = %s\n", symbol->ident, pvalue);
		}
	}
}
