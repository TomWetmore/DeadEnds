// DeadEnds
//
// integertable.h
//
// Created by Thomas Wetmore on 23 April 2023.
// Last changed on 1 May 2024.

#ifndef integertable_h
#define integertable_h

#include "hashtable.h"

// IntegerTable is a hash table that maps Strings to integers.
typedef HashTable IntegerTable;

// IntegerElement is an element in an IntegerTable.
typedef struct {
    String key;
    int value;
} IntegerElement;

// User interface.
IntegerTable *createIntegerTable(int numBuckets);
void insertInIntegerTable(IntegerTable*, String key, int value);
int searchIntegerTable(IntegerTable*, String key);
void incrIntegerTable(IntegerTable*, String key);

#endif // integertable_h
