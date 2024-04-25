// DeadEnds
//
// pvaluetable.c holds the functions that operate on PValueTables.
//
// Created by Thomas Wetmore on 21 April 2023.
//  Last changed on 18 April 2024.

// IS THIS DATA TYPE USED ANYMORE???

#include "standard.h"
#include "pvaluetable.h"

// getKey gets the key from a program value element.
static String getKey(void* element) { return ((PValueElement*) element)->key; }

// compare compares two program value elements.
static int compare(String a, String b) { return strcmp(a, b); }

// delete deletes a program value element when it is removed from a table.
//  MNOTE: This has to be done carefully.
static void delete(void* a) {
    PValueElement *element = (PValueElement*) a;
    free(element->key);
    PValue* pvalue = element->value;
    if (pvalue->type == PVString) free(pvalue->value.uString);
    free(pvalue);
}

// createPValueTable creates and returns a PValueTable.
static int numBucketsInPValueTable = 13;
PValueTable *createPValueTable(void) {
    return createHashTable(getKey, compare, delete, numBucketsInPValueTable);
}

//  createPValueElement creates a PVaueTable element.
static PValueElement *createPValueElement(String key, PValue *ppvalue) {
    PValueElement *element = (PValueElement*) stdalloc(sizeof(PValueElement));
    element->key = key;
    element->value = ppvalue;
    return element;
}

//  insertInPValueTable -- Add a new (key, pvalue) element to a program value table.
//--------------------------------------------------------------------------------------------------
void insertInPValueTable(PValueTable *table, String key, PValue pvalue)
{
    //  Prepare the heap pvalue that will mapped to by the key.
    PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));

    //  If the symbol is in the table replace the old value for the new.
    PValueElement *element = searchHashTable(table, key);
    if (element) {
        freePValue(element->value);
        element->value = ppvalue;
        return;
    }

    // Otherwise add a new element to the table.
    addToHashTable(table, createPValueElement(key, ppvalue), true);
}

//  getValueOfPValueElement -- Get the program value mapped to by the key from a table of
//    program values.
//--------------------------------------------------------------------------------------------------
PValue getValueOfPValueElement(PValueTable *table, String key)
{
    //  Look for the element.
    PValueElement *element = searchHashTable(table, key);
    if (element) {
        PValue *ppvalue = element->value;
        if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
    }

    // If not found return null.
    return nullPValue;
}
