//
//  DeadEnds
//
//  symboltable.c -- Functions that implement the symbol table data structure.
//
//  Created by Thomas Wetmore on 23 March 2023.
//  Last changed on 13 May 2023.
//

#include "standard.h"
#include "symboltable.h"

static bool debugging = false;

//  globalTable -- holds the symbols that are defined at the global scope.
extern SymbolTable *globalTable;

//  compareSymbols -- Compare two symbols by comparing the names of the variables.
//--------------------------------------------------------------------------------------------------
static int compareSymbols(Word a, Word b) {
	return strcmp(((Symbol*) a)->ident, ((Symbol*) b)->ident);
}

//  deleteSymbol -- Delete a symbol. For now it's a noop.
//    MNOTE: There should be a separate function for freeing pvalues that can be called in here.
//--------------------------------------------------------------------------------------------------
static void deleteSymbol(Word a)
{
	Symbol *symbol = (Symbol*) a;
	//stdfree(symbol->ident);
	stdfree(symbol->value);
}

//  getSymbolKey -- Function to extract the identifier from a symbol in a symbol table.
//--------------------------------------------------------------------------------------------------
static String getSymbolKey(Word a) { return ((Symbol*) a)->ident; }

//  createSymbol -- Create a new symbol. A symbol is an element in a symbol table bucket.
//--------------------------------------------------------------------------------------------------
static Symbol *createSymbol(String iden, PValue *ppvalue)
{
	Symbol *symbol = (Symbol*) stdalloc(sizeof(Symbol));
	symbol->ident = iden;
	symbol->value = ppvalue;
	return symbol;
}

// createSymbolTable -- Create and return a new symbol table.
//--------------------------------------------------------------------------------------------------
SymbolTable *createSymbolTable(void) {
	return createHashTable(compareSymbols, deleteSymbol, getSymbolKey);
}

//  assignValueToSymbol -- Assign a value to an identifier. If the identifier isn't in the local
//    table, look for it in the global table; if there use the global table.
//--------------------------------------------------------------------------------------------------
void assignValueToSymbol(SymbolTable *symtab, String ident, PValue pvalue)
{
	//  Determine the symbol table to use.
	SymbolTable *table = symtab;
	if (!isInHashTable(symtab, ident) && isInHashTable(globalTable, ident)) table = globalTable;

	//  Prepare the pvalue to become the symbol value; it is on the heap.
	PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue));
	memcpy(ppvalue, &pvalue, sizeof(PValue));
	if (pvalue.type == PVString) {
		ppvalue->value.uString = strsave(pvalue.value.uString);
		//  MNOTE: The key is here. Does it have to be? The stack pvalue is about to dissappear.
	}

	//  If the symbol is in the table free its old pvalue and reuse the heap area.
	//  MNOTE: Do we need to also free the string if this is a string value?
	Symbol *symbol = searchHashTable(table, ident);
	if (symbol) {
		//  MNOTE: Just removing the pvalue to reuse the symbol. Should the delete function be
		//    called and a new symbol be created?
		freePValue(symbol->value);
		symbol->value = ppvalue;
		return;
	}

	// Otherwise add a new symbol to the table; delegate to hash table.
	if (debugging) printf("assignValueToSymbol: %s = %s in %p\n", ident, pvalueToString(*ppvalue, true), symtab);
	insertInHashTable(table, createSymbol(ident, ppvalue));
}

//  getValueOfSymbol -- Get the value of a symbol from a symbol table. The program value is
//    copied onto the stack and returned as the function's return value.
//--------------------------------------------------------------------------------------------------
PValue getValueOfSymbol(SymbolTable *symtab, String ident)
//  symtab -- Symbol table to look the identifer up in.
//  ident -- Identifier to return the value of.
{
	if (debugging) printf("getValueOfSymbol %s from table\n", ident);
	if (debugging) showSymbolTable(symtab);
	//  Look for the identifier in the local table.
	Symbol *symbol = searchHashTable(symtab, ident);
	if (symbol) {
		PValue *ppvalue = symbol->value;
		if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
	}

	//  If not in the local table, look for it in the global table.
	symbol = searchHashTable(globalTable, ident);
	if (symbol) {
		PValue *ppvalue = symbol->value;
		if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
	}

	// If not found in either table the symbol isn't defined.
	return nullPValue;
}

void showSymbolTable(SymbolTable* table)
{
	ASSERT(table);
	printf("Symbol Table at Location %p\n", table);
	for (int i = 0; i < MAX_HASH; i++) {
		Bucket *bucket = table->buckets[i];
		if (!bucket || bucket->length <= 0) continue;
		for (int j = 0; j < bucket->length; j++) {
			Symbol *symbol = (Symbol*) bucket->elements[j];
			String pvalue = pvalueToString(*(symbol->value), false);
			printf("  %s = %s\n", symbol->ident, pvalue);
		}
	}
}
