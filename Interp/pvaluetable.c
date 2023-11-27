//
//  DeadEnds
//
//  pvaluetable.c -- Hash table for holding program values. Used to implement the TABLE type
//    in the DeadEnds programming language.
//
//  Created by Thomas Wetmore on 21 April 2023.
//  Last changed on 21 April 2023.
//

#include "standard.h"
#include "pvaluetable.h"

//  getPValueTableKey -- Get the key from a program value element.
//--------------------------------------------------------------------------------------------------
static String getPValueTableKey(Word element)
{
    return ((PValueElement*) element)->key;
}

//  comparePValueElements -- Compare two program value elements.
//--------------------------------------------------------------------------------------------------
static int comparePValueElements(Word a, Word b)
{
    String left = ((PValueElement*) a)->key;
    String right = ((PValueElement*) b)->key;
    return strcmp(left, right);
}

//  deletePValueElement -- Delete a program value element when it is removed from a table.
//  MNOTE: This has to be done carefully.
//--------------------------------------------------------------------------------------------------
static void deletePValueElement(Word a)
{
    PValueElement *element = (PValueElement*) a;
    stdfree(element->key);
    PValue* pvalue = element->value;
    if (pvalue->type == PVString) stdfree(pvalue->value.uString);
    stdfree(pvalue);
}

//  createPValueTable -- Create a program value table.
//--------------------------------------------------------------------------------------------------
PValueTable *createPValueTable(void)
{
    return createHashTable(comparePValueElements, deletePValueElement, getPValueTableKey);
}

//  createPValueElement -- Create a PVaue element.
//--------------------------------------------------------------------------------------------------
static PValueElement *createPValueElement(String key, PValue *ppvalue)
{
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
    insertInHashTable(table, createPValueElement(key, ppvalue));
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
