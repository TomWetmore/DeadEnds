//
//  DeadEnds
//
//  nameindex.h -- Implements the name index type used by the DeadEnds database to index all
//    the gedcom names in person records. A name index is a specialization of hash table.
//
//  Created by Thomas Wetmore on 26 November 2022.
//  Last changed on 14 July 2023.
//

#ifndef nameindex_h
#define nameindex_h

#include "set.h"
#include "hashtable.h"

//  NameElement -- An element in a name index bucket.
//--------------------------------------------------------------------------------------------------
typedef struct {
    String nameKey;   // The key of a name.
    Set *recordKeys;  // A Set of record keys, sorted by recordKeys.
} NameElement;

//  NameIndex -- Synonym for a hash table being used as a name table.
//--------------------------------------------------------------------------------------------------
typedef HashTable NameIndex;

// Interface to NameIndex.
//--------------------------------------------------------------------------------------------------
NameIndex *createNameIndex(void);
void deleteNameIndex(NameIndex *index);
void insertInNameIndex(NameIndex *index, String nameKey, String personKey);
void showNameIndex(NameIndex *index);
Set *searchNameIndex(NameIndex *index, String name);

#endif // nameindex_h
