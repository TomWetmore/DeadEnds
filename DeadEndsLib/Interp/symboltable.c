// DeadEnds
//
// symboltable.c holds the functions that implement SymbolTables.
//
// Created by Thomas Wetmore on 23 March 2023.
// Last changed on 24 May 2024.

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
	free(symbol->value);
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
void oldassignValueToSymbol(SymbolTable* symtab, String ident, PValue pvalue) {
	SymbolTable* table = symtab;
	if (!isInHashTable(symtab, ident) && isInHashTable(globalTable, ident)) table = globalTable;
	PValue* ppvalue = (PValue*) malloc(sizeof(PValue)); // Heapify.
	memcpy(ppvalue, &pvalue, sizeof(PValue));
	if (pvalue.type == PVString)
		ppvalue->value.uString = strsave(pvalue.value.uString); // TODO: Required?
	Symbol *symbol = searchHashTable(table, ident);
	if (symbol) { // Change value.
		freePValue(symbol->value);
		symbol->value = ppvalue;
		return;
	}
	//if (debugging) printf("assignValueToSymbol: %s = %s in %p\n", ident, pvalueToString(*ppvalue, true), symtab);
	addToHashTable(table, createSymbol(ident, ppvalue), true); // Else add symbol.
}

void assignValueToSymbol(SymbolTable* symtab, String ident, PValue pvalue) {
    // Determine symbol table to use.
    SymbolTable* table = symtab;
    if (!isInHashTable(symtab, ident) && isInHashTable(globalTable, ident)) {
        table = globalTable;
    }
    // Prepare the value to put in the symbol table.
    // If the pvalue holds a string the symbol table now takes over ownership of that string. No other
    // code can change it or free it.
    PValue* copy = malloc(sizeof(PValue)); // Copy pvalue to the heap.
    *copy = pvalue;
    // See if the symbol already exists.
    Symbol* symbol = searchHashTable(table, ident);
    // If the symbol exists free its old value (but retain the symbol)
    if (symbol) {
        freePValue(symbol->value);
        symbol->value = copy;
    // Else add a new symbol with the new value.
    } else {
        addToHashTable(table, createSymbol(ident, copy), true);
    }
}

// getValueOfSymbol gets the value of a Symbol from a SymbolTable; PValue is returned on stack.
PValue oldgetValueOfSymbol(SymbolTable* symtab, String ident) {
	Symbol *symbol = searchHashTable(symtab, ident); // Local.
	if (symbol) {
		PValue *ppvalue = symbol->value;
		if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
	}
	symbol = searchHashTable(globalTable, ident); // Global.
	if (symbol) {
		PValue *ppvalue = symbol->value;
		if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
	}
	return nullPValue; // Undefined.
}

PValue notAsOldgetValueOfSymbol(SymbolTable* symtab, String ident) {
    Symbol* symbol = searchHashTable(symtab, ident); // Local
    if (!symbol) {
        symbol = searchHashTable(globalTable, ident); // Global
    }
    if (symbol && symbol->value) {
        PValue* ppvalue = symbol->value;
        switch (ppvalue->type) {
            case PVString:
                // Return a new, heap-owned string PValue
                return createStringPValue(ppvalue->value.uString);
            // Add similar logic for other types that require deep copy
            default:
                // For scalar types like int, float, etc., shallow copy is fine
                return *ppvalue;
        }
    }
    return nullPValue;
}

PValue getValueOfSymbol(SymbolTable* symtab, String ident) {
    Symbol *symbol = searchHashTable(symtab, ident);
    if (!symbol) symbol = searchHashTable(globalTable, ident);
    if (!symbol || !symbol->value) return nullPValue;

    PValue* stored = symbol->value;
    if (stored->type == PVString && stored->value.uString) {
        return createStringPValue(stored->value.uString); // copy string
    }
    return *stored; // shallow copy for ints, floats, etc.
}

// showSymbolTable shows the contents of a SymbolTable.
void showSymbolTable(SymbolTable* table) {
	printf("Symbol Table at Location %p\n", table);
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
