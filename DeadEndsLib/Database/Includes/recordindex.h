//
// DeadEnds Library
//
//  recordindex.h defines RecordIndex as a HashTable.
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 3 June 2025.
//

#ifndef recordindex_h
#define recordindex_h

#include "standard.h"

typedef struct GNode GNode;
typedef struct HashTable HashTable;

// A RecordIndex is a HashTable where the elements are GNodes pointers.
typedef HashTable RecordIndex;

// Interface to RecordIndex.
RecordIndex *createRecordIndex(void);
void deleteRecordIndex(RecordIndex*);
void addToRecordIndex(RecordIndex*, GNode* root);
GNode* searchRecordIndex(RecordIndex*, String);
void showRecordIndex(RecordIndex*);

// FORRECORDINDEX iterates a RecordIndex returning only GNode*s of a specific type.
#define FORRECORDINDEX(table, gnode, type) {\
		int __i = 0, __j = 0;\
		HashTable *__table = table;\
		GNode* gnode = null;\
		GNode* __gnode = (GNode*) firstInHashTable(__table, &__i, &__j);\
		for(; __gnode; __gnode = (GNode*) nextInHashTable(__table, &__i, &__j)) {\
		if (recordType(__gnode) != type) continue;\
			gnode = __gnode;
#define ENDRECORDINDEX }}

#endif // recordindex_h


