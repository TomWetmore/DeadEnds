//
//  DeadEnds
//
//  integertable.c
//
//  Created by Thomas Wetmore on 23 April 2023.
//  Last changed on 29 May 2023.
//

#include "integertable.h"

String integerGetKey(Word element) { return ((IntegerElement*) element)->key; }

// compareIntegerKeys
//--------------------------------------------------------------------------------------------------
static int compareIntegerEls(Word leftElement, Word rightElement)
{
    String a = ((IntegerElement*) leftElement)->key;
    String b = ((IntegerElement*) rightElement)->key;
    return strcmp(a, b);
}

//  createIntegerTable -- Create an integer table. Delete function not required.
//--------------------------------------------------------------------------------------------------
IntegerTable *createIntegerTable(void)
{
    return createHashTable(compareIntegerEls, null, integerGetKey);
}

//  searchIntegerTable -- Search for a key in an integer table and return its integer value.
//--------------------------------------------------------------------------------------------------
int searchIntegerTable(IntegerTable *table, String key)
{
    IntegerElement *element = (IntegerElement*) searchHashTable(table, key);
    return element ? element->value : NAN;
}

//  insertInIntegerTable -- Insert a string key and integer value into an integer table.
//--------------------------------------------------------------------------------------------------
void insertInIntegerTable(IntegerTable *table, String key, int value)
{
    IntegerElement* element = (IntegerElement*) searchHashTable(table, key);
    //  If the element is already there change its integer value.
    if (element) {
        element->value = value;
        return;
    }
    //  Otherwise create a new element.
    element = (IntegerElement*) stdalloc(sizeof(IntegerElement));
    element->key = key;
    element->value = value;
    insertInHashTable(table, element);
}


