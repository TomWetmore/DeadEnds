//
// DeadEnds
//
// nameindex.h is the header file for the NameIndex data type used by the DeadEnds database
// index the Gedcom names in person records. A NameIndex is a specialization of HashTable.
//
// Created by Thomas Wetmore on 26 November 2022.
// Last changed on 26 March 2024.
//

#ifndef nameindex_h
#define nameindex_h

#include "set.h"
#include "hashtable.h"

// NameElement is an element in a NameIndex bucket.
typedef struct NameElement {
    String nameKey;
    Set *recordKeys;
} NameElement;

//  NameIndex is a synonym for HashTable.
typedef HashTable NameIndex;

// Interface to NameIndex.
NameIndex *createNameIndex(void);
void deleteNameIndex(NameIndex *index);
void insertInNameIndex(NameIndex *index, String nameKey, String personKey);
void showNameIndex(NameIndex *index);
Set *searchNameIndex(NameIndex *index, String name);

#endif // nameindex_h
