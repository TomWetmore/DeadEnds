//
//  DeadEnds Library
//
//  nameindex.h is the header file for the NameIndex data type used by the DeadEnds database
//  index the Gedcom names in person records. A NameIndex is a specialization of HashTable.
//
//  Created by Thomas Wetmore on 26 November 2022.
//  Last changed on 3 June 2025.
//

#ifndef nameindex_h
#define nameindex_h

#include "standard.h"

typedef struct HashTable HashTable;
typedef struct List List;
typedef struct Set Set;

typedef List RootList;

// NameElement is an element in a NameIndex bucket.
typedef struct NameIndexEl {
    String nameKey;
    Set* recordKeys;
} NameIndexEl;

// NameIndex is a synonym for HashTable.
typedef HashTable NameIndex;

// Interface to NameIndex.
NameIndex *createNameIndex(void);
void deleteNameIndex(NameIndex*);
void insertInNameIndex(NameIndex*, String nameKey, String personKey);
NameIndex* getNameIndex(RootList*);
void showNameIndex(NameIndex*);
void showNameIndexStats(NameIndex*);
Set* searchNameIndex(NameIndex*, String);
void getNameIndexStats(NameIndex*, int*, int*);

#endif // nameindex_h
