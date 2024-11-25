// DeadEnds Project
//
// recordindex.h defines RecordIndex as a HashTable.
//
// Created by Thomas Wetmore on 29 November 2022.
// Last changed on 22 November 2024.

#ifndef recordindex_h
#define recordindex_h

#include "gnode.h"
#include "hashtable.h"

// A RecordIndexEl in an element in a RecordIndex. It holds the root node of a Gedcom record and
// the line number where the record was defined. The element's key is the key of the root node.
typedef struct RecordIndexEl {
	GNode *root;
}  RecordIndexEl;

// A RecordIndex is a HashTable of RecordIndexEls.
typedef HashTable RecordIndex;

// Interface to RecordIndex.
RecordIndex *createRecordIndex(void);
void deleteRecordIndex(RecordIndex*);
void addToRecordIndex(RecordIndex*, String, GNode*);
GNode* searchRecordIndex(RecordIndex*, String);
void showRecordIndex(RecordIndex*);

#endif // recordindex_h
