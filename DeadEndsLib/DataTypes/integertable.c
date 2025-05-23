// DeadEnds Library
//
// integertable.c implements the IntegerTable data type, a HashTable that maps Strings
// to integers.
//
// Created by Thomas Wetmore on 23 April 2023.
// Last changed on 20 May 2025.

#include "integertable.h"

static String getKey(void* element) { return ((IntegerElement*) element)->key; }

// createIntegerTable creates and returns an IntegerTable.
IntegerTable* createIntegerTable(int numBuckets) {
	// basicDelete deletes the element but not objects the element points to.
    return createHashTable(getKey, null, basicDelete, numBuckets);
}

// searchIntegerTable searches for a key in an IntegerTable and return its integer value.
int searchIntegerTable(IntegerTable* table, String key) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    return element ? element->value : NAN;
}

// insertInIntegerTable inserts a string key with integer value into an integer table.
// mnote: The string key is not saved.
void insertInIntegerTable(IntegerTable* table, String key, int value) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    if (element) { // If there change value. mnote: possible leak of old value.
        element->value = value;
        return;
    }
    element = (IntegerElement*) stdalloc(sizeof(IntegerElement));
    element->key = key;
    element->value = value;
    addToHashTable(table, element, false);
}

// incrIntegerTable increments the value of an element in the IntegerTable.
void incrIntegerTable(IntegerTable* table, String key) {
	IntegerElement* element = searchHashTable(table, key);
	if (element) (element->value)++;
	else insertInIntegerTable(table, key, 1);
}

static void show(void* element) {
	IntegerElement* el = (IntegerElement*) element;
	printf(" %s -> %d\n", el->key, el->value);
}

// showIntegerTable show the contents of an IntegerTable; for debugging.
void showIntegerTable(IntegerTable* table) {
	showHashTable(table, show);
}
