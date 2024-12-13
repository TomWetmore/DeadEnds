// DeadEnds
//
// sequence.h is the header file for the Sequence datatype.
//
// Created by Thomas Wetmore on 1 March 2023.
// Last changed on 13 December 2024.

#ifndef sequence_h
#define sequence_h

#include "standard.h"
#include "block.h"
#include "gnode.h"
#include "recordindex.h"
#include "nameindex.h"
#include "refnindex.h" 

// SortType holds the possible sorted states of the elements in a Sequence.
typedef enum {
	SequenceNotSorted,
	SequenceKeySorted,
	SequenceNameSorted,
} SortType;

// SequenceEl is the type of Sequence elements.
typedef struct SequenceEl {
	GNode* root; // Root of record. MNOTE: do not free on delete.
	String name; // If element is a person. MNOTE: do not free on delete.
	void* value; // MNOTE: what do we do with this? Scenarios exists for do or not do free.
} SequenceEl;

// Sequence is a data type that holds sequences/sets/arrays of records.
typedef struct Sequence {
	Block block;
	SortType sortType;
	bool unique;
	RecordIndex* index;
} Sequence;

SequenceEl* createSequenceEl(RecordIndex*, String key, void* value);
Sequence* createSequence(RecordIndex*);
void deleteSequence(Sequence*);
Sequence* copySequence(Sequence*);
int lengthSequence(Sequence*);
void emptySequence(Sequence*);

void appendToSequence(Sequence*, String key, void*);
void appendSequenceToSequence(Sequence* dst, Sequence* src);
bool isInSequence(Sequence*, String key);
bool removeFromSequence(Sequence*, String key);
void nameSortSequence(Sequence*);
void keySortSequence(Sequence*);
Sequence* uniqueSequence(Sequence*);
void uniqueSequenceInPlace(Sequence*);

Sequence* personToChildren(GNode* person, RecordIndex*);
Sequence* personToFathers(GNode* person, RecordIndex*);
Sequence* personToMothers(GNode* person, RecordIndex*);
Sequence* familyToChildren(GNode* family, RecordIndex*);
Sequence* familyToFathers(GNode* family, RecordIndex*);
Sequence* familyToMothers(GNode* family, RecordIndex*);
Sequence* personToSpouses(GNode* person, RecordIndex*);
Sequence* personToFamilies(GNode* person, bool, RecordIndex*);
Sequence* nameToSequence(String name, RecordIndex*, NameIndex*);
Sequence* refnToSequence(String refn, RecordIndex*, RefnIndex*);

Sequence* unionSequence(Sequence*, Sequence*);
Sequence* intersectSequence(Sequence*, Sequence*);
Sequence* differenceSequence(Sequence*, Sequence*);
Sequence* childSequence(Sequence*);
Sequence* parentSequence(Sequence*);
Sequence* spouseSequence(Sequence*);
Sequence* ancestorSequence(Sequence*, bool close);
Sequence* descendentSequence(Sequence*, bool close);
Sequence* siblingSequence(Sequence*, bool close);
bool elementFromSequence(Sequence*, int index, String* key, String* name);
void renameElementInSequence(Sequence* sequence, String key);
void sequenceToGedcom(Sequence*, FILE*);
void showSequence(Sequence*, String title);

// FORSEQUENCE and ENDSEQUENCE iterate a Sequence.
#define FORSEQUENCE(sequence, element, count) {\
	SequenceEl* element;\
	int count;\
	Block* ___block = &(sequence->block);\
	SequenceEl** __elements = (SequenceEl**) ___block->elements;\
	for (int __i = 0; __i < ___block->length; __i++){\
		element = __elements[__i];\
		count = __i + 1;

#define ENDSEQUENCE }}

#endif // sequence_h
