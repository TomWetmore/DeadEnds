// DeadEnds
//
// integertable.c
//
// Created by Thomas Wetmore on 23 April 2023.
// Last changed on 1 May 2024.

#include "integertable.h"

String integerGetKey(void* element) { return ((IntegerElement*) element)->key; }

// createIntegerTable creates and returns an IntegerTable.
IntegerTable *createIntegerTable(int numBuckets) {
    return createHashTable(integerGetKey, null, null, numBuckets);
}

// searchIntegerTable searches for a key in an IntegerTable and return its integer value.
int searchIntegerTable(IntegerTable* table, String key) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    return element ? element->value : NAN;
}

// insertInIntegerTable inserts a string key with integer value into an integer table.
void insertInIntegerTable(IntegerTable *table, String key, int value) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    if (element) { // If there change value.
        element->value = value;
        return;
    }
    element = (IntegerElement*) malloc(sizeof(IntegerElement));
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


