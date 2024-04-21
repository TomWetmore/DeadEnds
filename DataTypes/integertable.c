//
// DeadEnds
//
// integertable.c
//
// Created by Thomas Wetmore on 23 April 2023.
// Last changed on 17 April 2024.
//

#include "integertable.h"

String integerGetKey(void* element) { return ((IntegerElement*) element)->key; }

// createIntegerTable creates and returns an IntegerTable.
IntegerTable *createIntegerTable(int numBuckets) {
    return createHashTable(integerGetKey, null, null, numBuckets);
}

// searchIntegerTable searches for a key in an IntegerTable and return its integer value.
int searchIntegerTable(IntegerTable *table, String key) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    return element ? element->value : NAN;
}

// insertInIntegerTable -- Insert a string key and integer value to an integer table.
void insertInIntegerTable(IntegerTable *table, String key, int value) {
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    if (element) { // If there change value.
        element->value = value;
        return;
    }
    element = (IntegerElement*) malloc(sizeof(IntegerElement)); // Create new element.
    element->key = key;
    element->value = value;
    addToHashTable(table, element, false);
}


