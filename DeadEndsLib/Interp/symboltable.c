//
//  DeadEnds Library
//  symboltable.c holds the functions that implement SymbolTables.
//
//  Symbols own their own memory. When a Symbol is looked up a copy is returned, including a copy of a string
//  if the Symbol is a string. The functions that look up Symbols (getSymbol...) return copies of the Symbols.
//
//  However, when Lists, Tables and Sequences are returned from a SymbolTable they are not deep copied. If they
//  are to be retained they should be copied. This is not done, so in consequence they are not freed when
//  replaced in SymbolTables. For the most part this is reasonable, but pathological scripts could cause problems.
//
//  There are two versions of the assign and get functions, one that takes a SymbolTable, and one that takes
//  a Context.
//
//  Created by Thomas Wetmore on 23 March 2023.
//  Last changed on 25 July 2025.
//

#include "block.h"
#include "standard.h"
#include "symboltable.h"
#include "hashtable.h"
#include "pvalue.h"
#include "context.h"
#include "frame.h"

#undef LISTBUG

// compare compares two Symbols by their ident fields.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// delete deletes a Symbol. It frees both PValue and the identifier before deleting the Symbol itself.
static void delete(void* a) {
	Symbol* symbol = (Symbol*) a;
	stdfree(symbol->value);
    stdfree(symbol->ident);
    stdfree(symbol);
}

// getKey returns the Symbol's identifier.
static String getKey(void *symbol) {
	return ((Symbol*) symbol)->ident;
}

// createSymbol creates a Symbol.
static Symbol *createSymbol(String iden, PValue *ppvalue) {
	Symbol* symbol = (Symbol*) stdalloc(sizeof(Symbol));
	symbol->ident = iden;
	symbol->value = ppvalue;
	return symbol;
}

// createSymbolTable creates a SymbolTable.
static int numBucketsInSymbolTable = 37;
SymbolTable* createSymbolTable(void) {
	return createHashTable(getKey, compare, delete, numBucketsInSymbolTable);
}

// deleteSymbolTable deletes a SymbolTable.
void deleteSymbolTable(SymbolTable* table) {
    deleteHashTable(table);
}

// assignValueToSymbolTable assigns a value to a Symbol.
void assignValueToSymbolTable(SymbolTable* table, String ident, PValue pvalue) {
    // Prepare the value to put in the symbol table.
    PValue* copy = clonePValue(&pvalue);
    // If the symbol exists free its old value.
    Symbol* symbol = searchHashTable(table, ident);
    if (symbol) {
        freePValue(symbol->value);
        symbol->value = copy;
    // Else add a new symbol with the new value.
    } else {
        addToHashTable(table, createSymbol(strsave(ident), copy), true);
    }
}

// assignValueToSymbol assigns a PValue to a Symbol in a Context. If the Symbol isn't in the current Frame's
// table, check the global table.
void assignValueToSymbol(Context* context, String ident, PValue pvalue) {
    // Determine symbol table to use.
    SymbolTable* table = context->frame->table;
    if (!isInHashTable(table, ident) && isInHashTable(context->globals, ident)) {
        table = context->globals;
    }
    // Prepare the value to put in the symbol table.
    PValue* copy = clonePValue(&pvalue);
    // If the symbol exists free its old value.
#ifdef LISTBUG
    if (pvalue.type == PVList) {
        printf("Assigning to list %s: %s\n", ident, valueOfPValue(pvalue));
    }
#endif
    Symbol* symbol = searchHashTable(table, ident);
    if (symbol) {
        freePValue(symbol->value);
        symbol->value = copy;
    // Else add a new symbol with the new value.
    } else {
        addToHashTable(table, createSymbol(strsave(ident), copy), true);
    }
}

// getValueFromSymbolTable gets the value of a Symbol from a SymbolTable; PValue is returned on stack.
PValue getValueFromSymbolTable(SymbolTable* symtab, String ident) {
    Symbol *symbol = searchHashTable(symtab, ident);
    if (!symbol || !symbol->value) return nullPValue;
#ifdef LISTBUG
    if (symbol->value->type == PVList) {
        printf("Retrieving list %s: %s\n", ident, valueOfPValue(*(symbol->value)));
    }
#endif
    return cloneAndReturnPValue(symbol->value);
}

// getValueOfSymbol gets the value of a symbol from a Context; the PValue is returned on the stack.
PValue getValueOfSymbol(Context* context, String ident) {
    SymbolTable* locals = context->frame->table;
    Symbol* symbol = searchHashTable(locals, ident);
    if (!symbol) symbol = searchHashTable(context->globals, ident);
    if (!symbol || !symbol->value) return nullPValue;
    return cloneAndReturnPValue(symbol->value);
}

// showSymbolTable shows the contents of a SymbolTable. For debugging.
void showSymbolTable(SymbolTable* table) {
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
