//
//  DeadEnds
//
//  pvaluetable.h -- Header file for the pvalue table data type.
//
//  Created by Thomas Wetmore on 21 April 2023.
//  Last changed on 23 April 2023.
//

#include "standard.h"
#include "pvalue.h"
#include "hashtable.h"

#ifndef pvaluetable_h
#define pvaluetable_h

//  PValueTable -- Hash table mapping indentifiers to program values. Similar is this sense to
//    a symol table.
//--------------------------------------------------------------------------------------------------
typedef HashTable PValueTable;

//  PValueElement -- An element in a pvalue table. An element consists of an identifier for
//    the element and a program value.
//--------------------------------------------------------------------------------------------------
typedef struct {
    String key;
    PValue *value;
} PValueElement;

//  User interface to pvalue tables.
//--------------------------------------------------------------------------------------------------
PValueTable *createPValueTable(void);
void insertInPValueTable(PValueTable*, String, PValue);
PValue getValueOfPValueElement(PValueTable*, String);

#endif /* pvaluetable_h */
