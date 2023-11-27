//
//  DeadEnds
//
//  integertable.h
//
//  Created by Thomas Wetmore on 23 April 2023.
//  Last changed on 29 May 2023.
//

#ifndef integertable_h
#define integertable_h

#include "hashtable.h"

//  IntegerTable -- The integer table is based on the generic hash table. The element's values
//    are integers.
//--------------------------------------------------------------------------------------------------
typedef HashTable IntegerTable;

//  IntegerElement -- Elements for String to Integer mappings. This variant is quite good for
//    sets of strings by ignoring the integer value.
//--------------------------------------------------------------------------------------------------
typedef struct {
    String key;
    int value;
} IntegerElement;

//  User interface.
//--------------------------------------------------------------------------------------------------
IntegerTable *createIntegerTable(void);
int searchIntegerTable(IntegerTable *table, String key);
void insertInIntegerTable(IntegerTable *table, String key, int value);

#endif // integertable_h
