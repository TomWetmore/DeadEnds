//
//  DeadEnds Library
//
//  pvaluetable.c holds the functions that operate on PValueTables.
//
//  Created by Thomas Wetmore on 21 April 2023.
//  Last changed on 2 June 2025.
//

#include "standard.h"
#include "hashtable.h"
#include "pvalue.h"
#include "pvaluetable.h"

// getKey gets the key from a program value element.
static String getKey(void* element) { return ((PValueElement*) element)->key; }

// compare compares two program value elements.
static int compare(String a, String b) { return strcmp(a, b); }

// delete deletes a program value element when it is removed from a table.
// MNOTE: This has to be done carefully.
static void delete(void* a) {
    PValueElement *element = (PValueElement*) a;
    free(element->key);
    PValue* pvalue = element->value;
    if (pvalue->type == PVString) free(pvalue->value.uString);
    free(pvalue);
}

// createPValueTable creates and returns a PValueTable.
static int numBucketsInPValueTable = 13;
PValueTable* createPValueTable(void) {
    return createHashTable(getKey, compare, delete, numBucketsInPValueTable);
}

// createPValueElement creates a PVaueTable element.
static PValueElement* createPValueElement(String key, PValue* ppvalue) {
    PValueElement *element = (PValueElement*) stdalloc(sizeof(PValueElement));
    element->key = key;
    element->value = ppvalue;
    return element;
}

// insertInPValueTable adds a new (key, pvalue) element to a program value table.
void insertInPValueTable(PValueTable* table, String key, PValue pvalue) {
    PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    PValueElement *element = searchHashTable(table, key);
    if (element) {
        freePValue(element->value);
        element->value = ppvalue;
        return;
    }
    addToHashTable(table, createPValueElement(key, ppvalue), true);
}

// getValueOfPValueElement gets the PValue mapped to by the key from PValueTable.
PValue getValueOfPValueElement(PValueTable* table, String key) {
    PValueElement* element = searchHashTable(table, key);
    if (element) {
        PValue* ppvalue = element->value;
        if (ppvalue) return (PValue){ppvalue->type, ppvalue->value};
    }
    return nullPValue;
}
