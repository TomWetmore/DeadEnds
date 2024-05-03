// DeadEnds
//
// sequence.c holds the functions that implement the Sequence data type that handles sets of
// persons and sometimes families. It underlies the indiseq data type of DeadEnds Script.
//
// Created by Thomas Wetmore on 1 March 2023.
// Last changed on 1 May 2024.

#include "standard.h"
#include "sequence.h"
#include "gnode.h"
#include "lineage.h"
#include "gedcom.h"
#include "interp.h"
#include "name.h"
#include "list.h"
#include "splitjoin.h"
#include "stringtable.h"
#include "sort.h"
#include "writenode.h"

//static bool debugging = false;
static int numBucketsInSequenceTables = 359;

// keyGetKey is the getKey function that returns the key of a SequenceEl.
static String keyGetKey(void* element) {
	SequenceEl* el = (SequenceEl*) element;
	return el->key;
}

// nameGetKey is the getKey function that returns the name of a SequenceEl.
static String nameGetKey(void* element) {
	SequenceEl* el = (SequenceEl*) element;
	return el->name;
}

// compareKeyFunc is the compare function that compares SequenceEl's by key.
static int keyCompare(String a, String b) {
	return compareRecordKeys(a, b);
}

// compareNameFunc is the compare function that compares SequenceEl's by name.
static int nameCompare(String a, String b) {
	return compareNames(a, b);
}

// deleteFunc is the function that deletes a SequenceEl.
static void delete(void* element) {
	SequenceEl *el = (SequenceEl*) element;
	free(el->key);
	if (el->name) free(el->name);
	free(el);
}

void baseFree(void *word) { free(word); }

#define key_to_name(key, database)  (NAME(keyToPerson(key, database))->value)

// sequenceElements returns the array of elments in a Sequence.
static SequenceEl** sequenceElements(Sequence *sequence) {
	return (SequenceEl**) (&(sequence->block))->elements;
}

// createSequenceEl creates a SequenceEl.
SequenceEl* createSequenceEl(String key, String name, void* value) {
	SequenceEl* el = (SequenceEl*) malloc(sizeof(SequenceEl));
	el->key = key ? strsave(key) : null;
	el->name = name ? strsave(name) : null;
	el->value = value;
	return el;
}

// createSequence creates a Sequence.
Sequence* createSequence(Database* database) {
	Sequence* sequence = (Sequence*) malloc(sizeof(Sequence));
	initBlock(&(sequence->block));
	sequence->database  = database;
	sequence->unique = false;
	sequence->sortType = SequenceNotSorted;
	return sequence;
}

// lengthSequence returns the length of a Sequence.
int lengthSequence(Sequence* sequence) {
	return (&(sequence->block))->length;
}

// deleteSequence deletes a Sequence.
void deleteSequence(Sequence* sequence) {
	Block *block = &(sequence->block);
	SequenceEl **elements = (SequenceEl**) block->elements;
	for (int i = 0; i < block->length; i++) {
		SequenceEl *element = elements[i];
		free(element->key);
		if (element->name) free(element->name);
		// Handle the value.
	}
	free(&(sequence->block));
	//free(sequence); // HELP HELP HELP
}

// emptySequence removes the elements from a Sequence.
void emptySequence(Sequence *sequence) {
	Block *b = &(sequence->block);
	emptyBlock(b, delete);
}

// appendToSequence creates and appends a SequenceEl to a Sequence.
void appendToSequence(Sequence* sequence, String key, String name, void* value) {
	if (!sequence || !key) return;
	SequenceEl *element = (SequenceEl*) malloc(sizeof(SequenceEl));
	element->key = strsave(key);
	if (name) element->name = strsave(name);
	element->value = (void*) value;
	appendToBlock(&(sequence->block), element);
}

// renameElementInSequence updates an element in a Sequence with a new name.
// TODO: That *kep != 'I' check doesn't make sense any more.
void renameElementInSequence(Sequence* sequence, String key) {
	if (!sequence || !key || *key != 'I') return;
	Block *block = &(sequence->block);
	SequenceEl** elements = (SequenceEl**) block->elements;
	for (int i = 0; i < block->length; i++) {
		if (eqstr(key, (elements[i])->key)) {
			if ((elements[i])->name) free((elements[i])->name);
			(elements[i])->name = strsave(key_to_name(key, sequence->database));
		}
	}
}

// isInSequence checks if a SequenceEl with given key is in a Sequence. NOTE: Consider
// what could be done if the sequence were sorted.
bool isInSequence(Sequence *seq, String key) {
	if (!seq || !key) return false;
	Block *block = &(seq->block);
	SequenceEl **elements = (SequenceEl**) block->elements;
	for (int i = 0; i < block->length; i++) {
		if (eqstr(key, (elements[i])->key)) return true;
	}
	return false;
}

// removeFromSequence removes the SequenceEl with the given key from the Sequence.
bool removeFromSequence(Sequence* sequence, String key) {
	ASSERT(sequence && key);
	if (!sequence || !key) return false;
	Block *block = &(sequence->block);
	if (sequence->sortType == SequenceNotSorted) {
		return removeFromUnsortedBlock(block, key, keyGetKey, delete);
	} else if (sequence->sortType == SequenceKeySorted) {
		return removeFromSortedBlock(block, key, keyGetKey, keyCompare, delete);
	} else if (sequence->sortType == SequenceNameSorted) {
		return removeFromSortedBlock(block, key, nameGetKey, nameCompare, delete);
	} else {
		printf("NO OTHER SORT TYPE SUPPORTED YET\n");
	}
	return false;
}

// elementFromSequence returns the key and name values of an indexed Sequence element.
bool elementFromSequence (Sequence* sequence, int index, String* pkey, String* pname) {
	ASSERT(sequence);
	Block *block = &(sequence->block);
	if (index < 0 || index >= block->length) return false;
	if (pkey) *pkey = keyGetKey((block->elements)[index]);
	if (pname) *pname = nameGetKey((block->elements)[index]);
	return true;
}

//// nameCompare compares two person sequence elements by their name fields.
//static int nameCompare(SequenceEl *el1, SequenceEl *el2) {
//	int rel = compareNames(el1->name, el2->name);
//	if (rel) return rel;  // If names are not equal return their relationship.
//	return compareRecordKeys(el1->key, el2->key);
//}

////  keyCompare compares two sequence elements by their record key fields.
//static int keyCompare(SequenceEl *el1, SequenceEl *el2) {
//	return compareRecordKeys(el1->key, el2->key);
//}

// valueCompare compares two sequence elements by their values. TODO: This is incomplete.
static int valueCompare (SequenceEl *el1, SequenceEl *el2) {
	/* WARNING: this is not correct as sval() is a PVALUE structure */
	return (int) (long) el1->value - (int) (long) el2->value;
}

// nameSortSequence sorts a sequence by the first first names of the persons.
void nameSortSequence(Sequence *sequence)
{
	if (sequence->sortType == SequenceNameSorted) return;
	sortBlock(&(sequence->block), nameGetKey, nameCompare);
	sequence->sortType = SequenceNameSorted;
}

// keySortSequence sorts a Sequence by key.
void keySortSequence(Sequence *sequence)
{
	if (sequence->sortType == SequenceKeySorted) return;
	sortBlock(&(sequence->block), keyGetKey, keyCompare);
	sequence->sortType = SequenceKeySorted;
}

// copySequence creates a copy of the given Sequence.
Sequence *copySequence(Sequence* sequence) {
	Sequence *copy = createSequence(sequence->database);
	FORSEQUENCE(sequence, element, count)
		appendToSequence(copy, element->key, element->name, element->value);
	ENDSEQUENCE
	return copy;
}

// uniqueSequence return a Sequence with the unique elements from the given Sequence.
Sequence *uniqueSequence(Sequence *sequence) {
	ASSERT(sequence);
	Block *block = &(sequence->block);
	Sequence *unique = createSequence(sequence->database);
	if (block->length == 0) return unique;
	int n = block->length;
	Block *uBlock = &(unique->block);
	if (sequence->sortType != SequenceKeySorted) keySortSequence(sequence);
	SequenceEl **els = (SequenceEl**) block->elements;

	// Add a copy of the first input element into the unique list.
	SequenceEl* el = els[0];
	appendToBlock(uBlock, createSequenceEl(el->key, el->name, el->value));

	int i, j;
	for (j = 0, i = 1; i < n; i++) {
		if (nestr(els[i]->key, els[j]->key)) {
			appendToBlock(uBlock, createSequenceEl(els[i]->key, els[i]->name, els[i]->value));
			j = i;
		}
	}
	return unique;
}

// uniqueSequenceInPlace removes duplicate (have the same key) elements from a Sequence.
// MNOTE: This has a memory leak; the elements removed are not managed properly.
//--------------------------------------------------------------------------------------------------
void uniqueSequenceInPlace(Sequence *sequence) {
	if (!sequence) return;
	Block *block = &(sequence->block);
	int n = block->length;
	if (n <= 1) return;

	SequenceEl **els = (SequenceEl**) block->elements;
	if (sequence->sortType != SequenceKeySorted) keySortSequence(sequence);
	int i, j;
	for (j = 0, i = 1; i < n; i++)
		if (nestr(els[i]->key, els[j]->key)) els[++j] = els[i];
	block->length = j + 1;
}

// unionSequence returns the union Sequence of two Sequences.
Sequence* unionSequence(Sequence* one, Sequence* two) {
	if (!one || !two || one->database != two->database) return null;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->database);
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	int i = 0, j = 0, rel;
	while (i < n && j < m) {
		if ((rel = keyCompare(u[i]->key, v[j]->key)) < 0) {
			appendToSequence(three, u[i]->key, u[i]->name, u[i]->value);
			i++;
		} else if (rel > 0) {
			appendToSequence(three, v[j]->key, v[j]->name, v[j]->value);
			j++;
		} else {
			appendToSequence(three, u[i]->key, u[i]->name, u[i]->value);
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, u[i]->key, u[i]->name, u[i]->value);
		i++;
	}
	while (j < m) {
		appendToSequence(three, v[j]->key, v[j]->name, v[j]->value);
		j++;
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// intersectSequence returns the intersection Sequence of two Sequences.
Sequence* intersectSequence(Sequence* one, Sequence* two) {
	ASSERT(one && two);
	ASSERT(one->database == two->database);
	if (!one || !two || one->database != two->database) return null;
	int rel;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->database);
	int i = 0, j = 0;
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->key, v[j]->key)) < 0) {
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			appendToSequence(three, (u[i])->key, u[i]->name, u[i]->value);
			i++; j++;
		}
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// differenceSequence returns the difference Sequence of two Sequences.
Sequence* differenceSequence(Sequence* one, Sequence* two) {
	ASSERT(one && two);
	ASSERT(one->database == two->database);
	if (!one || !two) return null;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->database);
	int i = 0, j = 0;
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	int rel;
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->key, v[j]->key)) < 0) {
			appendToSequence(three, u[i]->key, u[i]->name, u[i]->value);
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, u[i]->key, null, u[i]->value);
		i++;
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// parentSequence creates a Sequence with the parents of the persons in given Sequence.
Sequence* parentSequence(Sequence* sequence) {
	ASSERT(sequence && sequence->database);
	if (!sequence) return null;
	Database *database = sequence->database;
	StringTable *table = createStringTable(numBucketsInSequenceTables); // Track of added parents.
	Sequence *parents = createSequence(sequence->database);
	String key;
	FORSEQUENCE(sequence, el, count)
		GNode* indi = keyToPerson(el->key, database);
		GNode* fath = personToFather(indi, database);
		GNode* moth = personToMother(indi, database);
		if (fath && !isInHashTable(table, key = personToKey(fath))) {
			appendToSequence(parents, key, null, el->value);
			addToStringTable(table, key, null);
		}
		if (moth && !isInHashTable(table, key = personToKey(moth))) {
			appendToSequence(parents, key, null, el->value);
			addToStringTable(table, key, null);
		}
	ENDSEQUENCE
	deleteHashTable(table);
	return parents;
}

// childSequence creates a Sequence with the children of the persons in another Sequence.
Sequence* childSequence(Sequence* sequence) {
	if (!sequence) return null;
	StringTable* table = createStringTable(numBucketsInSequenceTables);
	Database* database = sequence->database;
	Sequence* children = createSequence(database);
	FORSEQUENCE(sequence, el, num)
		GNode *person = keyToPerson(el->key, database);
		FORFAMSS(person, fam, key, database)
			FORCHILDREN(fam, chil, childKey, num2, database)
				String key = personToKey(chil);
				if (!isInHashTable(table, key)) {
					appendToSequence(children, key, null, 0);
					addToStringTable(table, key, null);
				}
			ENDCHILDREN
		ENDFAMSS
	ENDSEQUENCE
	deleteHashTable(table);
	return children;
}

// personToChildren creates the Sequence of a person's children.
Sequence *personToChildren(GNode* person, Database *database) {
	if (!person) return null;
	Sequence *children = createSequence(database);

	FORFAMSS(person, family, key, database)
		FORCHILDREN(family, child, childKey, count, database)
			appendToSequence(children, personToKey(child), null, 0);
		ENDCHILDREN
	ENDFAMSS
	if (lengthSequence(children)) return children;
	deleteSequence(children);
	return null;
}

// personToSpouses creates the Sequence of a person's spouses; must be male or female; spouses
// must have the opposite sex; if there is more than one sex in a family the first is used.
Sequence *personToSpouses(GNode *person, Database *database) {
	SexType sex = SEXV(person);
	if (sex != sexMale && sex != sexFemale) return null;
	Sequence *spouses = createSequence(database);
	FORFAMSS(person, family, key, database)
		GNode *spouse = (sex == sexMale) ? familyToWife(family, database) : familyToHusband(family, database);
		if (spouse) appendToSequence(spouses, personToKey(spouse), null, 0);
	ENDFAMSS
	if (lengthSequence(spouses)) return spouses;
	deleteSequence(spouses);
	return null;
}

// personToFathers creates the Sequence of a person's fathers.
Sequence *personToFathers(GNode *person, Database *database) {
	if (!person) return null;
	Sequence *fathers = createSequence(database);
	FORFAMCS(person, family, key, database)  // For each family the person is a child in...
		FORHUSBS(family, husb, husbKey, database)  // For each husband in that family...
			appendToSequence(fathers, personToKey(husb), null, 0);  // Add him to the sequence.
		ENDHUSBS
	ENDFAMCS
	if (lengthSequence(fathers)) return fathers;
	deleteSequence(fathers);
	return null;
}

// personToMothers creates the Sequence of a person's mothers
Sequence *personToMothers (GNode* indi, Database *database) {
	if (!indi) return null;
	Sequence *mothers = createSequence(database);
	FORFAMCS(indi, fam, key, database)  // For each family the person is a child in...
		FORWIFES(fam, wife, wifeKey, database)  // For each wife in that family...
			appendToSequence(mothers, personToKey(wife), null, 0);  // Add her to the sequence.
		ENDWIFES
	ENDFAMCS
	if (lengthSequence(mothers)) return mothers;
	deleteSequence(mothers);
	return null;
}

// personToFamilies creates the Sequence of a person's families. If fams is true the families are
// those the person is a spouse in, else they are those the person is a child in.
Sequence* personToFamilies (GNode* person, bool fams, Database* database) {
	if (!person) return null;
	Sequence *families = createSequence(database);
	if (fams) {
		FORFAMSS(person, family, key, database)
			appendToSequence(families, key, null, 0);
		ENDFAMSS
	} else {
		FORFAMCS(person, family, key, database)
			appendToSequence(families, key, null, 0);
		ENDFAMCS
	}
	if (lengthSequence(families) > 0) return families;
	deleteSequence(families);
	return null;
}

// familyToChildren creates the Sequence of a family's children.
Sequence* familyToChildren(GNode* family, Database* database) {
	if (!family) return null;
	Sequence* children = createSequence(database);
	FORCHILDREN(family, chil, key, num, database) {
		appendToSequence(children, personToKey(chil), null, 0);
	} ENDCHILDREN
	if (lengthSequence(children) > 0) return children;
	deleteSequence(children);
	return null;
}

// familyToFathers create the Sequence of a family's fathers/husbands.
Sequence* familyToFathers(GNode* fam, Database* database) {
	if (!fam) return null;
	Sequence* seq = createSequence(database);
	FORHUSBS(fam, husb, key, database)
		appendToSequence(seq, personToKey(husb), null, 0);
	ENDHUSBS
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq);
	return null;
}

// familyToMothers creates the Sequence of a family's mothers/wives.
Sequence* familyToMothers(GNode *fam, Database* database) {
	if (!fam) return null;
	Sequence* seq = createSequence(database);
	FORWIFES(fam, wife, key, database)
		appendToSequence(seq, personToKey(wife), null, 0);
	ENDWIFES
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq);
	return null;
}

// siblingSequence creates the Sequence of the sibllings of the persons in the given Sequence.
// If close is true include the persons in the initial Sequence in the sibling Sequence.
Sequence* siblingSequence(Sequence* sequence, bool close) {
	GNode* fam;
	String key;
	Database* database = sequence->database;
	StringTable* tab = createStringTable(numBucketsInSequenceTables);
	Sequence* familySequence = createSequence(database);
	Sequence* siblingSequence = createSequence(database);
	FORSEQUENCE(sequence, element, num) {
		//  TODO: THIS ONLY USES THE FIRST FAMC FAMILY, WHICH IN 99.9% OF THE CASES IS OKAY.
		//  BUT TO BE CONSISTENT WITH OTHER SITUATIONS WHERE THERE ARE MULTIPLE FAMC NODES,
		//  IT MIGHT BE BETTER TO GO THROUGH THEM ALL.
		GNode *person = keyToPerson(element->key, database);
		if ((fam = personToFamilyAsChild(person, database)) == NULL) {
			appendToSequence(familySequence, familyToKey(fam), null, 0);
		}
		if (!close) addToStringTable(tab, element->key, null);
	}
	ENDSEQUENCE
	FORSEQUENCE(familySequence, el, num)
		fam = keyToFamily(el->key, database);
		FORCHILDREN(fam, chil, chilKey, num2, database)
			key = personToKey(chil);
			if (!isInHashTable(tab, key)) {
				appendToSequence(siblingSequence, key, null, 0);
				addToStringTable(tab, key, null);
			}
		ENDCHILDREN
	ENDSEQUENCE
	deleteHashTable(tab);
	deleteSequence(familySequence);
	return siblingSequence;
}

// ancestorSequence creates the Sequence of all ancestors of the persons in the given Sequence.
// The persons in the original sequence are not in the ancestor sequence unless they are an
// ancestor of someone in the original Sequence.
// TODO: Consider adding a "close" argument as done in siblingSequence.
Sequence *ancestorSequence(Sequence *startSequence) {
	ASSERT(startSequence);
	Database *database = startSequence->database;
	StringTable *ancestorKeys = createStringTable(numBucketsInSequenceTables);  // Keys of all persons encountered.
	List *ancestorQueue = createList(null, null, null, false);  // Queue of ancestor keys to process.
	Sequence *ancestorSequence = createSequence(database);  // The sequence holding the ancestors.

	//  Initialize the ancestor queue with the keys of persons in the start sequence.
	FORSEQUENCE(startSequence, el, num)
		enqueueList(ancestorQueue, (void*) el->key);
	ENDSEQUENCE

	// Iterate the ancestor queue; it grows when ancestors are found.
	while (!isEmptyList(ancestorQueue)) {
		String key = (String) dequeueList(ancestorQueue);  //  Key of next person in queue.
		String parentKey;

		//  Get the father and mother, if any, of the person from the queue.
		//  TODO: TREATS ONLY THE MOTHER AND FATHER OF THE PERSON'S 1ST FAMC FAMILY AS ANCESTORS.
		GNode *person = keyToPerson(key, database);
		GNode *father = personToFather(person, database);
		GNode *mother = personToMother(person, database);

		// If the father has not been seen, add him to the table and sequence.
		if (father && !isInHashTable(ancestorKeys, parentKey = father->key)) {
			appendToSequence(ancestorSequence, parentKey, null, 0);
			//  MNOTE: Lists don'e save their elements.
			enqueueList(ancestorQueue, strsave(parentKey));
			addToStringTable(ancestorKeys, parentKey, null);
		}
		// If the mother has not been seen before, add her to the table and sequence.
		if (mother && !isInHashTable(ancestorKeys, parentKey = mother->key)) {
			appendToSequence(ancestorSequence, parentKey, null, 0);
			//  MNOTE: Lists don't save their elements.
			enqueueList(ancestorQueue, strsave(parentKey));
			addToStringTable(ancestorKeys, parentKey, null);
		}
	}
	deleteHashTable(ancestorKeys);
	deleteList(ancestorQueue);
	return ancestorSequence;
}

//  descendentSequence -- Create the descendant sequence of a sequence. The persons in the original
//    sequence are not in the descendent sequence unless they are also a descendent of someone in
//    the original sequence.
//--------------------------------------------------------------------------------------------------
Sequence *descendentSequence(Sequence *startSequence)
//  startSequence -- Sequence of persons to get the decendents of.
{
	if (!startSequence) return null;
	Database *database = startSequence->database;
	String key, descendentKey;
	StringTable *descendentKeys = createStringTable(numBucketsInSequenceTables);  //  Keys of all descendents processed.
	StringTable *familyKeys = createStringTable(numBucketsInSequenceTables);  //  Keys of all families processed.
	List *descendentQueue = createList(null, null, null, false);  //  Queue of descendent keys.
	Sequence *descendentSequence = createSequence(database);  //  Sequence of all descendents to return.

	//  Initialize the descendent queue with the keys of the persons in the start sequence.
	FORSEQUENCE(startSequence, element, count)
		enqueueList(descendentQueue, /*(Word)*/ element->key);
	ENDSEQUENCE

	//  Dequeue the next person in the descendent queue.
	while (!isEmptyList(descendentQueue)) {
		key = (String) dequeueList(descendentQueue);
		GNode *person = keyToPerson(key, database);

		//  All children in the person's FAMS families are descendents.
		FORFAMSS(person, family, key, database) {
			if (isInHashTable(familyKeys, key)) goto a;
			addToStringTable(familyKeys, strsave(key), null);
			FORCHILDREN(family, child, chilKey, num, database)
				if (!isInHashTable(descendentKeys, descendentKey = personToKey(child))) {
					appendToSequence(descendentSequence, descendentKey, null, 0);
					//  MNOTE: strsave required -- lists don't save their elements.
					enqueueList(descendentQueue, strsave(descendentKey));
					addToStringTable(descendentKeys, descendentKey, null);
				}
			ENDCHILDREN
		a:;
		} ENDFAMSS
	}
	deleteHashTable(descendentKeys);
	deleteHashTable(familyKeys);
	deleteList(descendentQueue);
	return descendentSequence;
}

//  spouseSequence -- Create spouses sequence of a sequence
//--------------------------------------------------------------------------------------------------
Sequence *spouseSequence(Sequence *sequence)
//  seq -- Sequence of persons to get the spouses of.
{
	if (!sequence) return null;
	Database *database = sequence->database;
	StringTable *table = createStringTable(numBucketsInSequenceTables);
	Sequence *spouses = createSequence(database);
	FORSEQUENCE(sequence, el, num)   //  For each person in the original sequence
		GNode *person = keyToPerson(el->key, database);
		FORSPOUSES(person, spouse, fam, num1, database)   // For each spouse that that person has
			String key = personToKey(spouse);   // Get the key of the spouse
			if (!isInHashTable(table, key)) {   // If the spouse's key isn't in the table.
				appendToSequence(spouses, key, null, el->value);  // Add the spouse.
				addToStringTable(table, key, null);
			}
		ENDSPOUSES
	ENDSEQUENCE
	deleteHashTable(table);
	return spouses;
}

static void writeLimitedPerson(GNode *person);
static void writeLimitedFamily(GNode *family);

//  sequenceToGedcom -- Generate a Gedcom file from a sequence of persons. Only persons in
//    the sequence are written to the file. Families with links to at least two persons in the
//    sequence are also written to the file. Other persons referred to in the families are not
//    included in the file, and the links to the other persons are removed.
//--------------------------------------------------------------------------------------------------
static Sequence *personSequence;  // Person in the sequence to be generated as a Gedcom file.
static Sequence *familySequence;  // The families that interconnect persons in the person sequence.
void sequenceToGedcom(Sequence *sequence, FILE *fp)
//  sequence -- Sequence of persons to output in Gedcom format.
{
	if (!sequence) return;
	Database *database = sequence->database;
	if (!fp) fp = stdout;
	personSequence = sequence;  // Yuck. External access to these two sequences are required.
	familySequence = createSequence(database);
	StringTable *personTable = createStringTable(numBucketsInSequenceTables);  //  Table of person keys.
	StringTable *familyTable = createStringTable(numBucketsInSequenceTables);  //  Table of family keys.

	//  Add all person keys to the person key hash table.
	FORSEQUENCE(sequence, element, num)
		addToStringTable(personTable, element->key, null);
	ENDSEQUENCE

	//  Loop through each person in the sequence.
	FORSEQUENCE(sequence, element, num)
		GNode *person = keyToPerson(element->key, database);  // Get the person.
		SexType sex = SEXV(person);  //  And the person's sex.

		//  Check the person's parent families to see if any FAMC families should be output.
		FORFAMCS(person, family, key, database)
			if (isInHashTable(familyTable, family->key)) goto a;
			normalizeFamily(family);
			GNode *husband = HUSB(family);
			if (husband && isInHashTable(personTable, husband->value)) {
				appendToSequence(familySequence, familyToKey(family), null, 0);
				addToStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			GNode *wife = WIFE(family);
			if (wife && isInHashTable(personTable, wife->value)) {
				appendToSequence(familySequence, familyToKey(family), null, 0);
				addToStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			//  Check whether any of the children in this family are in the sequence.
			FORCHILDREN(family, child, chilKey, count, database)
				String childKey = personToKey(child);
				if (isInHashTable(personTable, childKey)) {
					appendToSequence(familySequence, familyToKey(family), null, 0);
					addToStringTable(familyTable, familyToKey(family), null);
					goto a;
				}
			ENDCHILDREN
		ENDFAMCS

		//  Check the person's as parent families to see if they should output.
	a:	FORFAMSS(person, family, key, database)
			if (isInHashTable(familyTable, familyToKey(family))) goto b;
			GNode *spouse = familyToSpouse(family, oppositeSex(sex), database);
			if (spouse && isInHashTable(personTable, personToKey(spouse))) {
				appendToSequence(familySequence, familyToKey(family), null, 0);
				addToStringTable(familyTable, familyToKey(family), null);
			}
	b:;	ENDFAMSS
	ENDSEQUENCE

	FORSEQUENCE(personSequence, element, count)
		writeLimitedPerson(keyToPerson(element->key, database));
	ENDSEQUENCE

	FORSEQUENCE(familySequence, element, count)
		writeLimitedFamily(keyToFamily(element->key, database));
	ENDSEQUENCE
	deleteSequence(familySequence);
	deleteHashTable(personTable);
	deleteHashTable(familyTable);
}

bool limitPersonNode(GNode* node, int level)
{
	//  FOR TESTING. WE SHOULD SEE NO BIRTH EVENTS IN THE OUTPUT.
	if (level == 1 && eqstr(node->tag, "BIRT")) return false;
	writeGNode(stdout, level, node, false);
	return true;
}

// writeLimitedPerson is used when writing a Sequence of persons to a Gedcom file. Will only
// include links to families (and other objects) that are also to be in the Gedcom file.
// TODO: NOT IMPLEMENTED.
static void writeLimitedPerson (GNode* person) {
	printf("writeLimitedPerson: %s\n", person->key);
	traverseNodes(person, 0, limitPersonNode);
}

// writeLimitedFamily is used when
static void writeLimitedFamily (GNode *family) {
	printf("writeLimitedFamily: %s\n", family->key);
}

// nameToSequence returns the Sequence of persons who match a Gedcom name. If the first letter of
// the given names is '*', the Sequence will contain all persons who match the surname.
Sequence* nameToSequence(String name, Database* database) {
	ASSERT(name && *name && database);
	if (!name || *name == 0 || !database) return null;
	int num;
	Sequence *seq = null;
	if (*name != '*') { // Name does not start with '*'.
		String *keys = personKeysFromName(name, database, &num /*true*/);
		if (num == 0) return null;
		seq = createSequence(database);
		for (int i = 0; i < num; i++)
			appendToSequence(seq, keys[i], null, 0);
		nameSortSequence(seq);
		return seq;
	}

	// Name starts with a '*'.
	char scratch[MAXLINELEN+1];
	sprintf(scratch, "a/%s/", getSurname(name));
	for (int c = 'a'; c <= 'z'; c++) {
		scratch[0] = c;
		String *keys = personKeysFromName(scratch, database, &num/*, true*/);
		if (num == 0) continue;
		if (!seq) seq = createSequence(database);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], null, 0);
		}
	}
	scratch[0] = '$';
	String *keys = personKeysFromName(scratch, database, &num/*, true*/);
	if (num) {
		if (!seq) seq = createSequence(database);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], null, 0);
		}
	}
	if (seq) {
		uniqueSequence(seq);
		nameSortSequence(seq);
	}
	return seq;
}

// showSequence is a debug function that shows the contents of a Sequence.
void showSequence(Sequence* sequence) {
	FORSEQUENCE(sequence, element, count)
		printf("%d: %s: %s\n", count, element->key, element->name ? element->name : "no name");
	ENDSEQUENCE

}

//  format_indiseq -- Format print lines of sequence.
//--------------------------------------------------------------------------------------------------
//static void format_indiseq (Sequence *seq, bool famp, bool marr)
////  seq -- Sequence to format.
////  famp -- This is a sequence of families.
////  marr -- Try to include marriage information??????
//{
//    GNode *fam, *spouse;
//    //int ifkey;
//    char scratch[20];
//    //String p;
//    if (famp) {
//        FORSEQUENCE(seq, el, num) {
//            fam = keyToFamily(skey(el));
//            if (sval(el)) {
//                sprintf(scratch, "I%ld", sval(el)->pvValue.uInt);
//                spouse = keyToPerson(scratch);
//            } else
//                spouse = null;
//            //sprn(el) = indi_to_list_string(spouse, fam, 68);
//        } ENDSEQUENCE
//    } else {
//        FORSEQUENCE(seq, el, num) {
//            //GNode *indi = keyToPerson(skey(el));
//            if (marr) {
//                sprintf(scratch, "F%ld", sval(el)->pvValue.uInt);
//                fam = keyToFamily(scratch);
//            } else
//                fam = null;
//            //sprn(el) = indi_to_list_string(indi, fam, 68);
//        } ENDSEQUENCE
//    }
//}
/*==============================================================
 * refn_to_indiseq -- Return indiseq whose user references match
 *============================================================*/
//Sequence refn_to_indiseq (ukey)
//String ukey;
//{
//    String *keys;
//    int num, i;
//    Sequence seq;
//
//    if (!ukey || *ukey == 0) return null;
//    get_refns(ukey, &num, &keys, 'I');
//    if (num == 0) return null;
//    seq = createSequence();
//    for (i = 0; i < num; i++) {
//        appendToSequence(seq, keys[i], null, null, false);
//    }
//    if (length_indiseq(seq) == 0) {
//        deleteSequence(seq, false);
//        return null;
//    }
//    namesort_indiseq(seq);
//    return seq;
//}

//  key_to_indiseq -- Return person sequence of the matching key
//--------------------------------------------------------------------------------------------------
//Sequence key_to_indiseq(String name)
////  name
//{
//    if (!name) return null;
//    String *keys;
//    Sequence seq = null;
//    if (!(id_by_key(name, &keys))) return null;
//    seq = createSequence();
//    appendToSequence(seq, keys[0], null, null, false);
//    return seq;
//}
/*===========================================================
 * str_to_indiseq -- Return person sequence matching a string
 * The rules of search precedence are implemented here:
 *  1. named indiset
 *  2. key, with or without the leading "I"
 *  3. REFN
 *  4. name
 *===========================================================*/
//Sequence str_to_indiseq (String name)
////String name;
//{
//    Sequence seq;
//    seq = find_named_seq(name);
//    if (!seq) seq = key_to_indiseq(name);
//    if (!seq) seq = refn_to_indiseq(name);
//    if (!seq) seq = nameToSequence(name);
//    return seq;
//}
