//
//  DeadEnds Project
//
//  recordindex.h -- Defines the RecordIndex as a HashTable.
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 27 November 2023.
//

#ifndef recordindex_h
#define recordindex_h

#include "gnode.h"
#include "hashtable.h"

//  RecordIndexEl -- An element of a RecordIndex Bucket.
//--------------------------------------------------------------------------------------------------
typedef struct RecordIndexEl {
	GNode *root;     // Root node of the record.
	int lineNumber;  // Line number in the original Gedcom file where the record begins.
}  RecordIndexEl;

//  RecordIndex -- A RecordIndex is a HashTable.
//--------------------------------------------------------------------------------------------------
typedef HashTable RecordIndex;

// User interface to RecordIndex.
//--------------------------------------------------------------------------------------------------
RecordIndex *createRecordIndex(void);                   //  Create a RecordIndex.
void deleteRecordIndex(RecordIndex*);                   //  Delete a RecordIndex.
void insertInRecordIndex(RecordIndex*, String, GNode*, int lineNo); //  Add record to RecordIndex.
GNode* searchRecordIndex(RecordIndex*, String);         //  Search for a record in a RecordIndex.
void showRecordIndex(RecordIndex*);                     //  Show the contents of RecordIndex.

#endif // recordindex_h
