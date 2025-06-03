//  DeadEnds Library
//
//  pvaluetable.h is the header file for the PValueTable data type. This type is used to implement
//  the table type in the DeadEnds script language.
//
//  Created by Thomas Wetmore on 21 April 2023.
//  Last changed on 2 June 2025.
//
#ifndef pvaluetable_h
#define pvaluetable_h

#include "standard.h"

typedef struct HashTable HashTable;
typedef struct PValue PValue;

// PValueTable is a hash table mapping indentifiers to PValues.
typedef HashTable PValueTable;

// PValueElement is an element in a PValueTable.
typedef struct {
    String key;
    PValue *value;
} PValueElement;

// User interface to pvalue tables.
PValueTable *createPValueTable(void);
void insertInPValueTable(PValueTable*, String, PValue);
PValue getValueOfPValueElement(PValueTable*, String);

#endif // pvaluetable_h
