//
//  DeadEnds
//
//  sequence.h -- Header file for the Sequence datatype.
//
//  Created by Thomas Wetmore on 1 March 2023.
//  Last changed on 16 November 2023.
//

#ifndef sequence_h
#define sequence_h

#include "standard.h"
#include "gnode.h"
#include "pnode.h"
#include "pvalue.h"

//  SequenceEl -- Data type for Sequence elements.
//    MNOTE: keys are always present and belong to the element; names are present for persons and
//    belong to the element; values do not belong to the element.
//--------------------------------------------------------------------------------------------------
typedef struct SequenceEl {
	String key;     // Person or family key.  (TODO: Would it be better to use root GNodes here?)
	String name;    // Name of person.
	PValue *value;  // Any program value.
}
*SequenceEl;

//#define spri(s) ((s)->keyInt)  // Key as integer.

//  Sequence -- Data type for a person or family sequence (or list or set).
//--------------------------------------------------------------------------------------------------
typedef struct Sequence Sequence;
struct Sequence {
	int size;          // Current length of sequence.
	int max;           // Max length before resizing.
	int flags;         // Attribute flags.
	SequenceEl *data;  // Sequence of items. TODO: Should this be replaced with the new List type?
	Database *database;  // Database the sequence comes from.
};

#define IData(s)  ((s)->data)

//  Values for the attribute flag.
//--------------------------------------------------------------------------------------------------
#define KEYSORT   (1<<0)
#define NAMESORT  (1<<1)
#define UNIQUED   (1<<2)
#define VALUESORT (1<<3)

Sequence *createSequence(Database*);  // Create a sequence.
void deleteSequence(Sequence*, bool fval);  //  Delete a sequence.
Sequence *copySequence(Sequence*);  //  Copy a sequence.
int lengthSequence(Sequence*);  //  Return the length of a sequence.
void appendToSequence(Sequence*, String key, String name, PValue *val);
bool isInSequence(Sequence*, String key);
bool removeFromSequence(Sequence*, String key, String name, int index);
void nameSortSequence(Sequence*);  //  Sort a sequence by name.
void keySortSequence(Sequence*);  //  Sort a sequence by key.
void valueSortSequence(Sequence*); //  Sort a sequence by value (not properly implemented).
Sequence *uniqueSequence(Sequence*);  //  Return sequence uniqueued from another.

Sequence *personToChildren(GNode *person, Database*);  //  Return sequence of a person's children.
Sequence *personToFathers(GNode *person, Database*);   //  Return sequence of a person's fathers.
Sequence *personToMothers(GNode *person, Database*);   //  Return sequence of a person's mothers.
Sequence *familyToChildren(GNode *family, Database*);  //  Return sequence of a family's children.
Sequence *familyToFathers(GNode *family, Database*);  //  Return sequence of a family's fathers.
Sequence *familyToMothers(GNode *family, Database*);  //  Return sequence of a family's mothers.
Sequence *personToSpouses(GNode *person, Database*);  //  Return sequence of a person's spouses.
Sequence *personToFamilies(GNode *person, bool, Database*);  //  Return sequence of a person's families.
Sequence *nameToSequence(String, Database*);  //  Return sequence of persons who match a name.
Sequence *refn_to_indiseq(String refn);

Sequence *unionSequence(Sequence*, Sequence*);
Sequence *intersectSequence(Sequence*, Sequence*); 
Sequence *differenceSequence(Sequence*, Sequence*);
Sequence *childSequence(Sequence*);
Sequence *parentSequence(Sequence*);
Sequence *spouseSequence(Sequence*);
Sequence *ancestorSequence(Sequence*);
Sequence *descendentSequence(Sequence*); 
Sequence *siblingSequence(Sequence*, bool);

void sequenceToGedcom(Sequence*, FILE*);

// Kind of for debugging.
void showSequence(Sequence*); // Show the contents of a sequence by pringing keys and names.for

//  FORSEQUENCE -- Macro that iterates over a sequence in its current order.
//--------------------------------------------------------------------------------------------------
#define FORSEQUENCE(s,e,i)\
{   int i;\
	SequenceEl e, *_d;\
	_d = IData((Sequence*)s);\
	for (int _i = 0, _n = ((Sequence*)s)->size; _i < _n; _i++) {\
		e = _d[_i]; \
		i = _i + 1;

#define ENDSEQUENCE }}

#endif // sequence_h
