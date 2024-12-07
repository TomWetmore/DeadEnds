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

#endif // recordindex_h
