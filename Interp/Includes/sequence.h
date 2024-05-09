// DeadEnds
//
// sequence.h is the header file for the Sequence datatype.
//
// Created by Thomas Wetmore on 1 March 2023.
// Last changed on 4 May 2024.

#ifndef sequence_h
#define sequence_h

#include "standard.h"
#include "block.h"
#include "gnode.h"

// SortType holds the possible sorted states of the elements in a Sequence.
typedef enum {
	SequenceNotSorted,
	SequenceKeySorted,
	SequenceNameSorted,
} SortType;

// SequenceEl is the type of Sequence elements.
typedef struct SequenceEl {
	GNode* root;
	String name; // If element is a person.
	void* value;
} SequenceEl;

// Sequence is a data type that holds sequences/sets/arrays of records.
typedef struct Sequence {
	Block block;
	SortType sortType;
	bool unique;
	Database *database;
} Sequence;

SequenceEl* createSequenceEl(Database*, String key, void* value);
Sequence* createSequence(Database*);
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

Sequence* personToChildren(GNode* person, Database*);
Sequence* personToFathers(GNode* person, Database*);
Sequence* personToMothers(GNode* person, Database*);
Sequence* familyToChildren(GNode* family, Database*);
Sequence* familyToFathers(GNode* family, Database*);
Sequence* familyToMothers(GNode* family, Database*);
Sequence* personToSpouses(GNode* person, Database*);
Sequence* personToFamilies(GNode* person, bool, Database*);
Sequence* nameToSequence(String, Database*);
Sequence* refnToSequence(String refn, Database*);

Sequence* unionSequence(Sequence*, Sequence*);
Sequence* intersectSequence(Sequence*, Sequence*);
Sequence* differenceSequence(Sequence*, Sequence*);
Sequence* childSequence(Sequence*);
Sequence* parentSequence(Sequence*);
Sequence* spouseSequence(Sequence*);
Sequence* ancestorSequence(Sequence*);
Sequence* descendentSequence(Sequence*);
Sequence* siblingSequence(Sequence*, bool);
bool elementFromSequence(Sequence*, int index, String* key, String* name);
void renameElementInSequence(Sequence* sequence, String key);
void sequenceToGedcom(Sequence*, FILE*);
void showSequence(Sequence*);

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
