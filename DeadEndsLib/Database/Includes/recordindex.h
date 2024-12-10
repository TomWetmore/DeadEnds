// DeadEnds Project
//
// recordindex.h defines RecordIndex as a HashTable.
//
// Created by Thomas Wetmore on 29 November 2022.
// Last changed on 4 December 2024.

#ifndef recordindex_h
#define recordindex_h

#include "gnode.h"
#include "hashtable.h"

// A RecordIndex is a HashTable where the elements are GNodes pointers.
typedef HashTable RecordIndex;

// Interface to RecordIndex.
RecordIndex *createRecordIndex(void);
void deleteRecordIndex(RecordIndex*);
void addToRecordIndex(RecordIndex*, GNode* root);
GNode* searchRecordIndex(RecordIndex*, String);
void showRecordIndex(RecordIndex*);

#define FORRECORDINDEX(table, element, type) {\
		int __i = 0, __j = 0;\
		HashTable *__table = table;\
		GNode* gnode = null;\
		GNode* __gnode = (GNode*) firstInHashTable(__table, &__i, &__j);\
		for(; __gnode; __gnode = (GNode*) nextInHashTable(__table, &__i, &__j)) {\
		if (recordType(__gnode) != type) continue;\
			gnode = __gnode;
#define ENDRECORDINDEX }}

#endif // recordindex_h


