// DeadEnds
//
// sequence.c holds the functions that implement the Sequence data type that handles sets of
// persons and other record types. It underlies the indiseq data type of DeadEnds Script.
//
// Created by Thomas Wetmore on 1 March 2023.
// Last changed on 11 May 2025.

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
	return el->root->key;
}

// nameGetKey is the getKey function that returns the name of a SequenceEl.
static String nameGetKey(void* element) {
	SequenceEl* el = (SequenceEl*) element;
	return el->name;
}

// keyCompare is the compare function that compares SequenceEl's by key.
static int keyCompare(String a, String b) {
	return compareRecordKeys(a, b);
}

// nameCompare is the compare function that compares SequenceEl's by name.
static int nameCompare(String a, String b) {
	return compareNames(a, b);
}

// delete is the function that deletes a SequenceEl.
static void delete(void* element) {
	stdfree(element);
}

void baseFree(void *word) { free(word); }

#define key_to_name(key, database)  (NAME(keyToPerson(key, database))->value)

// sequenceElements returns the array of elments in a Sequence.
static SequenceEl** sequenceElements(Sequence *sequence) {
	return (SequenceEl**) (&(sequence->block))->elements;
}

// createSequenceEl creates a SequenceEl.
//SequenceEl* createSequenceEl(Database* database, String key, void* value) {
SequenceEl* createSequenceEl(RecordIndex* index, String key, void* value) {
	SequenceEl* element = (SequenceEl*) malloc(sizeof(SequenceEl));
	GNode* root = getRecord(key, index);
	ASSERT(root);
	element->root = root;
	if (recordType(root) == GRPerson) element->name = (NAME(root))->value;
	element->value = value;
	return element;
}

// createSequence creates a Sequence.
//Sequence* createSequence(Database* database) {
Sequence* createSequence(RecordIndex* index) {
	Sequence* sequence = (Sequence*) malloc(sizeof(Sequence));
	initBlock(&(sequence->block));
	//sequence->database  = database;
	sequence->index  = index;
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
	Block* block = &(sequence->block);
	deleteBlock(block, delete);
	free(sequence);
}

// emptySequence removes the elements from a Sequence.
void emptySequence(Sequence *sequence) {
	Block* block = &(sequence->block);
	emptyBlock(block, delete);
}

// appendToSequence creates and appends a SequenceEl to a Sequence.
void appendToSequence(Sequence* sequence, String key, void* value) {
	if (!sequence || !key) return;
	//SequenceEl* element = createSequenceEl(sequence->database, key, value);
	SequenceEl* element = createSequenceEl(sequence->index, key, value);
	appendToBlock(&(sequence->block), element);
}

// appendSequenceToSequence appends a Sequence to another Sequence. The Sequences must be distinct.
// destination changes; source does not.
void appendSequenceToSequence(Sequence* destination, Sequence* source) {
	FORSEQUENCE(source, element, count)
		appendToSequence(destination, element->root->key, element->value);
	ENDSEQUENCE
}

// renameElementInSequence updates an element in a Sequence with a new name.
void renameElementInSequence(Sequence* sequence, String key) {
	if (!sequence || !key) return;
	Block *block = &(sequence->block);
	SequenceEl** elements = (SequenceEl**) block->elements;
	for (int i = 0; i < block->length; i++) {
		GNode* root = elements[i]->root;
		if (eqstr(key, root->key)) {
			(elements[i])->name = NAME(root)->value;
		}
	}
}

// isInSequence checks if a SequenceEl with given key is in a Sequence.
// TODO: Should take advantage of the sortability of the Sequence.
bool isInSequence(Sequence *seq, String key) {
	if (!seq || !key) return false;
	Block *block = &(seq->block);
	SequenceEl **elements = (SequenceEl**) block->elements;
	for (int i = 0; i < block->length; i++) {
		if (eqstr(key, (elements[i])->root->key)) return true;
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

// nameSortSequence sorts a sequence by the names of the persons. Assumes person Sequence.
void nameSortSequence(Sequence* sequence) {
	if (sequence->sortType == SequenceNameSorted) return;
	sortBlock(&(sequence->block), nameGetKey, nameCompare);
	sequence->sortType = SequenceNameSorted;
}

// keySortSequence sorts a Sequence by key.
void keySortSequence(Sequence* sequence) {
	if (sequence->sortType == SequenceKeySorted) return;
	sortBlock(&(sequence->block), keyGetKey, keyCompare);
	sequence->sortType = SequenceKeySorted;
}

// copySequence creates a copy of the given Sequence.
Sequence* copySequence(Sequence* sequence) {
	Sequence* copy = createSequence(sequence->index);
	FORSEQUENCE(sequence, element, count)
		appendToSequence(copy, element->root->key, element->value);
	ENDSEQUENCE
	return copy;
}

// uniqueSequence returns a Sequence with the unique elements from the given Sequence.
// MNOTE: creates a new Sequence, so caller may need to free the original.
Sequence* uniqueSequence(Sequence* sequence) {
	ASSERT(sequence);
	Block* block = &(sequence->block);
	Sequence* unique = createSequence(sequence->index);
	if (block->length == 0) return unique;
	int n = block->length;
	Block *uBlock = &(unique->block);
	if (sequence->sortType != SequenceKeySorted) keySortSequence(sequence);
	SequenceEl** els = (SequenceEl**) block->elements;
	SequenceEl* el = els[0];
	RecordIndex* index = sequence->index;
	appendToBlock(uBlock, createSequenceEl(index, el->root->key, el->value));
	int i, j;
	for (j = 0, i = 1; i < n; i++) {
		if (nestr(els[i]->root->key, els[j]->root->key)) {
			appendToBlock(uBlock, createSequenceEl(index, els[i]->root->key, els[i]->value));
			j = i;
		}
	}
	return unique;
}

// uniqueSequenceInPlace removes duplicate (have the same key) elements from a Sequence.
void uniqueSequenceInPlace(Sequence* sequence) {
	if (!sequence) return;
	Block* block = &(sequence->block);
	int n = block->length;
	if (n <= 1) return;
	if (sequence->sortType != SequenceKeySorted) keySortSequence(sequence);
	SequenceEl** els = (SequenceEl**) block->elements;
	int i, j;
	for (j = 0, i = 1; i < n; i++)
		if (nestr(els[i]->root->key, els[j]->root->key)) els[++j] = els[i];
	block->length = j + 1;
}

// unionSequence returns the union of two Sequences.
Sequence* unionSequence(Sequence* one, Sequence* two) {
	if (!one || !two || one->index != two->index) return null;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->index);
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	int i = 0, j = 0, rel;
	while (i < n && j < m) {
		if ((rel = keyCompare(u[i]->root->key, v[j]->root->key)) < 0) {
			appendToSequence(three, u[i]->root->key, u[i]->value);
			i++;
		} else if (rel > 0) {
			appendToSequence(three, v[j]->root->key, v[j]->value);
			j++;
		} else {
			appendToSequence(three, u[i]->root->key, u[i]->value);
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, u[i]->root->key, u[i]->value);
		i++;
	}
	while (j < m) {
		appendToSequence(three, v[j]->root->key, v[j]->value);
		j++;
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// intersectSequence returns the intersection of two Sequences.
Sequence* intersectSequence(Sequence* one, Sequence* two) {
	ASSERT(one && two);
	ASSERT(one->index == two->index);
	if (!one || !two || one->index != two->index) return null;
	int rel;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->index);
	int i = 0, j = 0;
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->root->key, v[j]->root->key)) < 0) {
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			appendToSequence(three, (u[i])->root->key, u[i]->value);
			i++; j++;
		}
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// differenceSequence returns the difference of two Sequences.
Sequence* differenceSequence(Sequence* one, Sequence* two) {
	ASSERT(one && two);
	ASSERT(one->index == two->index);
	if (!one || !two) return null;
	if (one->sortType != SequenceKeySorted) keySortSequence(one);
	if (two->sortType != SequenceKeySorted) keySortSequence(two);
	if (!one->unique) uniqueSequenceInPlace(one);
	if (!two->unique) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence* three = createSequence(one->index);
	int i = 0, j = 0;
	SequenceEl** u = sequenceElements(one);
	SequenceEl** v = sequenceElements(two);
	int rel;
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->root->key, v[j]->root->key)) < 0) {
			appendToSequence(three, u[i]->root->key, u[i]->value);
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, u[i]->root->key, u[i]->value);
		i++;
	}
	three->sortType = SequenceKeySorted;
	three->unique = true;
	return three;
}

// parentSequence creates a Sequence with the parents of the persons in given Sequence.
Sequence* parentSequence(Sequence* sequence) {
	ASSERT(sequence && sequence->index);
	if (!sequence) return null;
	RecordIndex* index = sequence->index;
	StringTable* table = createStringTable(numBucketsInSequenceTables);
	Sequence* parents = createSequence(index);
	String key;
	FORSEQUENCE(sequence, el, count)
		GNode* indi = keyToPerson(el->root->key, index);
		GNode* fath = personToFather(indi, index);
		GNode* moth = personToMother(indi, index);
		if (fath && !isInHashTable(table, key = personToKey(fath))) {
			appendToSequence(parents, key, el->value);
			addToStringTable(table, key, null);
		}
		if (moth && !isInHashTable(table, key = personToKey(moth))) {
			appendToSequence(parents, key, el->value);
			addToStringTable(table, key, null);
		}
	ENDSEQUENCE
	deleteHashTable(table);
	return parents;
}

// childSequence creates a Sequence of the children of the persons in another Sequence.
Sequence* childSequence(Sequence* sequence) {
	if (!sequence) return null;
	StringTable* table = createStringTable(numBucketsInSequenceTables);
	RecordIndex* index = sequence->index;
	Sequence* children = createSequence(index);
	FORSEQUENCE(sequence, el, num)
		GNode* person = keyToPerson(el->root->key, index);
		FORFAMSS(person, fam, key, index)
			FORCHILDREN(fam, chil, childKey, num2, index)
				String key = personToKey(chil);
				if (!isInHashTable(table, key)) {
					appendToSequence(children, key, 0);
					addToStringTable(table, key, null);
				}
			ENDCHILDREN
		ENDFAMSS
	ENDSEQUENCE
	deleteHashTable(table);
	return children;
}

// personToChildren creates the Sequence of a person's children.
Sequence* personToChildren(GNode* person, RecordIndex* index) {
	if (!person) return null;
	Sequence* children = createSequence(index);
	FORFAMSS(person, family, key, index)
		FORCHILDREN(family, child, childKey, count, index)
			appendToSequence(children, personToKey(child), 0);
		ENDCHILDREN
	ENDFAMSS
	if (lengthSequence(children)) return children;
	deleteSequence(children);
	return null;
}

// personToSpouses creates the Sequence of a person's spouses; must be male or female; spouses
// must have the opposite sex; if there is more than one sex in a family the first is used.
Sequence* personToSpouses(GNode* person, RecordIndex* index) {
	SexType sex = SEXV(person);
	if (sex != sexMale && sex != sexFemale) return null;
	Sequence* spouses = createSequence(index);
	FORFAMSS(person, family, key, index)
		GNode* spouse = (sex == sexMale) ? familyToWife(family, index) : familyToHusband(family, index);
		if (spouse) appendToSequence(spouses, personToKey(spouse), 0);
	ENDFAMSS
	if (lengthSequence(spouses)) return spouses;
	deleteSequence(spouses);
	return null;
}

// personToFathers creates the Sequence of a person's fathers.
Sequence* personToFathers(GNode* person, RecordIndex* index) {
	if (!person) return null;
	Sequence* fathers = createSequence(index);
	FORFAMCS(person, family, key, index)  // For each family the person is a child in...
		FORHUSBS(family, husb, husbKey, index)  // For each husband in that family...
			appendToSequence(fathers, personToKey(husb), 0);  // Add to sequence.
		ENDHUSBS
	ENDFAMCS
	if (lengthSequence(fathers)) return fathers;
	deleteSequence(fathers);
	return null;
}

// personToMothers creates the Sequence of a person's mothers
Sequence* personToMothers (GNode* indi, RecordIndex* index) {
	if (!indi) return null;
	Sequence* mothers = createSequence(index);
	FORFAMCS(indi, fam, key, index)  // For each family the person is a child in...
		FORWIFES(fam, wife, wifeKey, index)  // For each wife in that family...
			appendToSequence(mothers, personToKey(wife), 0);  // Add to sequence.
		ENDWIFES
	ENDFAMCS
	if (lengthSequence(mothers)) return mothers;
	deleteSequence(mothers);
	return null;
}

// personToFamilies creates the Sequence of a person's families. If fams is true the families are
// those the person is a spouse in, else they are those the person is a child in.
Sequence* personToFamilies (GNode* person, bool fams, RecordIndex* index) {
	if (!person) return null;
	Sequence* families = createSequence(index);
	if (fams) {
		FORFAMSS(person, family, key, index)
			appendToSequence(families, key, 0);
		ENDFAMSS
	} else {
		FORFAMCS(person, family, key, index)
			appendToSequence(families, key, 0);
		ENDFAMCS
	}
	if (lengthSequence(families) > 0) return families;
	deleteSequence(families);
	return null;
}

// familyToChildren creates the Sequence of a family's children.
Sequence* familyToChildren(GNode* family, RecordIndex* index) {
	if (!family) return null;
	Sequence* children = createSequence(index);
	FORCHILDREN(family, chil, key, num, index) {
		appendToSequence(children, personToKey(chil), 0);
	} ENDCHILDREN
	if (lengthSequence(children) > 0) return children;
	deleteSequence(children);
	return null;
}

// familyToFathers create the Sequence of a family's fathers/husbands.
Sequence* familyToFathers(GNode* fam, RecordIndex* index) {
	if (!fam) return null;
	Sequence* seq = createSequence(index);
	FORHUSBS(fam, husb, key, index)
		appendToSequence(seq, personToKey(husb), 0);
	ENDHUSBS
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq);
	return null;
}

// familyToMothers creates the Sequence of a family's mothers/wives.
Sequence* familyToMothers(GNode* fam, RecordIndex* index) {
	if (!fam) return null;
	Sequence* seq = createSequence(index);
	FORWIFES(fam, wife, key, index)
		appendToSequence(seq, personToKey(wife), 0);
	ENDWIFES
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq);
	return null;
}

// siblingSequence creates the Sequence of the siblings of the persons in the input Sequence.
// If close is true persons in the input Sequence are added to the sibling Sequence.
Sequence* siblingSequence(Sequence* sequence, bool close) {
	RecordIndex* index = sequence->index;
	StringTable* tab = createStringTable(numBucketsInSequenceTables);
	Sequence* familySequence = createSequence(index);
	Sequence* siblingSequence = createSequence(index);
	FORSEQUENCE(sequence, element, num) {
		//  TODO: THIS ONLY USES THE FIRST FAMC FAMILY, WHICH IN 99.9% OF THE CASES IS OKAY.
		//  BUT TO BE CONSISTENT WITH OTHER SITUATIONS WHERE THERE ARE MULTIPLE FAMC NODES,
		//  IT MIGHT BE BETTER TO GO THROUGH THEM ALL.
		GNode* person = keyToPerson(element->root->key, index);
		GNode* famc = personToFamilyAsChild(person, index);
		if (famc)
			appendToSequence(familySequence, familyToKey(famc), 0);
		if (!close)
			addToStringTable(tab, element->root->key, null);
	}
	ENDSEQUENCE
	FORSEQUENCE(familySequence, el, num)
		GNode* fam = keyToFamily(el->root->key, index);
		FORCHILDREN(fam, chil, chilKey, num2, index)
			String key = personToKey(chil);
			if (!isInHashTable(tab, key)) {
				appendToSequence(siblingSequence, key, 0);
				addToStringTable(tab, key, null);
			}
		ENDCHILDREN
	ENDSEQUENCE
	deleteHashTable(tab);
	deleteSequence(familySequence);
	return siblingSequence;
}

// ancestorSequence creates the Sequence of all ancestors of the persons in the input Sequence.
// Persons in the input sequence are not in the ancestor sequence unless they are also an
// ancestor of someone in the input Sequence.
// TODO: Consider adding a "close" argument as done in siblingSequence.
Sequence* ancestorSequence(Sequence* startSequence, bool close) {
	ASSERT(startSequence);
	RecordIndex* index = startSequence->index;
	StringTable* ancestorKeys = createStringTable(numBucketsInSequenceTables);
	List* ancestorQueue = createList(null, null, null, false); // Keys to process.
	Sequence* ancestorSequence = createSequence(index);
	if (close) uniqueSequenceInPlace(startSequence);

	FORSEQUENCE(startSequence, el, num) // Init ancestor queue.
		String key = el->root->key;
		enqueueList(ancestorQueue, (void*) key);
		if (close) {
			appendToSequence(ancestorSequence, key, 0);
			addToStringTable(ancestorKeys, key, null);
		}
	ENDSEQUENCE

	while (!isEmptyList(ancestorQueue)) { // Process queue.
		String key = (String) dequeueList(ancestorQueue);
		String parentKey;
		GNode* person = keyToPerson(key, index);
		GNode* father = personToFather(person, index);
		GNode* mother = personToMother(person, index);
		if (father && !isInHashTable(ancestorKeys, parentKey = father->key)) {
			appendToSequence(ancestorSequence, parentKey, 0);
			enqueueList(ancestorQueue, strsave(parentKey));
			addToStringTable(ancestorKeys, parentKey, null);
		}
		if (mother && !isInHashTable(ancestorKeys, parentKey = mother->key)) {
			appendToSequence(ancestorSequence, parentKey, 0);
			enqueueList(ancestorQueue, strsave(parentKey));
			addToStringTable(ancestorKeys, parentKey, null);
		}
	}
	deleteHashTable(ancestorKeys);
	deleteList(ancestorQueue);
	return ancestorSequence;
}

// descendentSequence creates the descendant Sequence of a Sequence. Those in the input Sequence
// are not in the descendents unless they are a descendent of someone in the input Sequence.
// TODO: Consider adding a "close" flag to the interface.
Sequence* descendentSequence(Sequence* startSequence, bool close) {
	if (!startSequence) return null;
	RecordIndex* index = startSequence->index;
	String key, descendentKey;
	StringTable* descendentKeys = createStringTable(numBucketsInSequenceTables); // Persons processed.
	StringTable* familyKeys = createStringTable(numBucketsInSequenceTables);  // Families processed.
	List* descendentQueue = createList(null, null, null, false); // Descendent keys.
	Sequence *descendentSequence = createSequence(index);
	FORSEQUENCE(startSequence, element, count) // Init descendentQueue
	String key = element->root->key;
		enqueueList(descendentQueue, key);
		if (close) {
			appendToSequence(descendentSequence, key, 0);
			addToStringTable(descendentKeys, key, null);
		}
	ENDSEQUENCE
	while (!isEmptyList(descendentQueue)) { // Loop descendentQueue.
		key = (String) dequeueList(descendentQueue);
		GNode* person = keyToPerson(key, index);
		FORFAMSS(person, family, key, index) {
			if (isInHashTable(familyKeys, key)) goto a;
			addToStringTable(familyKeys, strsave(key), null);
			FORCHILDREN(family, child, chilKey, num, index)
				if (!isInHashTable(descendentKeys, descendentKey = personToKey(child))) {
					appendToSequence(descendentSequence, descendentKey, 0);
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

// spouseSequence creates spouses Sequence of a Sequence.
Sequence* spouseSequence(Sequence *sequence) {
	if (!sequence) return null;
	RecordIndex* index = sequence->index;
	StringTable* table = createStringTable(numBucketsInSequenceTables);
	Sequence* spouses = createSequence(index);
	FORSEQUENCE(sequence, el, num)
		GNode* person = keyToPerson(el->root->key, index);
		FORSPOUSES(person, spouse, fam, num1, index)
			String key = personToKey(spouse);
			if (!isInHashTable(table, key)) {
				appendToSequence(spouses, key, el->value);
				addToStringTable(table, key, null);
			}
		ENDSPOUSES
	ENDSEQUENCE
	deleteHashTable(table);
	return spouses;
}

static void writeLimitedPerson(GNode *person);
static void writeLimitedFamily(GNode *family);

// sequenceToGedcom generate a Gedcom file from a sequence of persons. Only persons in the
// the sequence are written. Families with links to at least two persons in the sequence are also
// written to the file. Other persons referred to in the families are not included, and the links
// to the other persons are removed.
static Sequence *personSequence; // Person in the sequence to be generated in a Gedcom file.
static Sequence *familySequence; // Families that interconnect persons in the person sequence.
void sequenceToGedcom(Sequence *sequence, FILE *fp) {
	if (!sequence) return;
	RecordIndex* index = sequence->index;
	if (!fp) fp = stdout;
	personSequence = sequence;  // Yuck. External access to these two sequences are required.
	familySequence = createSequence(index);
	StringTable *personTable = createStringTable(numBucketsInSequenceTables); // Table of person keys.
	StringTable *familyTable = createStringTable(numBucketsInSequenceTables); // Table of family keys.

	// Add all person keys to the person key hash table.
	FORSEQUENCE(sequence, element, num)
		addToStringTable(personTable, element->root->key, null);
	ENDSEQUENCE
	FORSEQUENCE(sequence, element, num) // For each person in the sequence ...
		GNode *person = keyToPerson(element->root->key, index);  // Get the person ...
		SexType sex = SEXV(person);  //  ... and the person's sex.
		//  Check the person's parent families to see if any FAMC families should be output.
		FORFAMCS(person, family, key, index)
			if (isInHashTable(familyTable, family->key)) goto a;
			normalizeFamily(family);
			GNode *husband = HUSB(family);
			if (husband && isInHashTable(personTable, husband->value)) {
				appendToSequence(familySequence, familyToKey(family), 0);
				addToStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			GNode *wife = WIFE(family);
			if (wife && isInHashTable(personTable, wife->value)) {
				appendToSequence(familySequence, familyToKey(family), 0);
				addToStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			// Check if any of the children in this family are in the sequence.
			FORCHILDREN(family, child, chilKey, count, index)
				String childKey = personToKey(child);
				if (isInHashTable(personTable, childKey)) {
					appendToSequence(familySequence, familyToKey(family), 0);
					addToStringTable(familyTable, familyToKey(family), null);
					goto a;
				}
			ENDCHILDREN
		ENDFAMCS

		//  Check the person's as parent families to see if they should output.
	a:	FORFAMSS(person, family, key, index)
			if (isInHashTable(familyTable, familyToKey(family))) goto b;
			GNode *spouse = familyToSpouse(family, oppositeSex(sex), index);
			if (spouse && isInHashTable(personTable, personToKey(spouse))) {
				appendToSequence(familySequence, familyToKey(family), 0);
				addToStringTable(familyTable, familyToKey(family), null);
			}
	b:;	ENDFAMSS
	ENDSEQUENCE

	FORSEQUENCE(personSequence, element, count)
		writeLimitedPerson(keyToPerson(element->root->key, index));
	ENDSEQUENCE

	FORSEQUENCE(familySequence, element, count)
		writeLimitedFamily(keyToFamily(element->root->key, index));
	ENDSEQUENCE
	deleteSequence(familySequence);
	deleteHashTable(personTable);
	deleteHashTable(familyTable);
}

// Still needed?
bool limitPersonNode(GNode* node, int level) {
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
// MNOTE: returns a new sequence; caller responsible for its memory.
Sequence* nameToSequence(String name, RecordIndex* rindex, NameIndex* nindex) {
	ASSERT(name && *name && rindex && nindex);
	if (!name || *name == 0 || !rindex || !nindex) return null;
	int num;
	Sequence *seq = null;
	if (*name != '*') { // Name does not start with '*'.
		String *keys = personKeysFromName(name, rindex, nindex, &num); // MNOTE: keys is static.
		if (num == 0) return null;
		seq = createSequence(rindex);
		for (int i = 0; i < num; i++)
			appendToSequence(seq, keys[i], 0);
		nameSortSequence(seq);
		return seq;
	}
	// Name starts with a '*'.
	char scratch[MAXLINELEN+1];
	sprintf(scratch, "a/%s/", getSurname(name));
	for (int c = 'a'; c <= 'z'; c++) {
		scratch[0] = c;
		String *keys = personKeysFromName(scratch, rindex, nindex, &num/*, true*/);
		if (num == 0) continue;
		if (!seq) seq = createSequence(rindex);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], 0);
		}
	}
	scratch[0] = '$';
	String *keys = personKeysFromName(scratch, rindex, nindex, &num/*, true*/);
	if (num) {
		if (!seq) seq = createSequence(rindex);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], 0);
		}
	}
	if (seq) {
		Sequence* useq = uniqueSequence(seq);
		deleteSequence(seq);
		seq = useq;
		nameSortSequence(seq);
	}
	return seq;
}

// showSequence is a debug function that shows the contents of a Sequence.
void showSequence(Sequence* sequence, String title) {
	if (title) printf("%s:\n", title);
	FORSEQUENCE(sequence, element, count)
		printf("%d: %s: %s\n", count, element->root->key, element->name ? element->name : "no name");
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

// keyToSequence returns a single element Sequence with the record with the given key; or null.
Sequence* keyToSequence(String key, RecordIndex* index) {
	GNode* record = getRecord(keyToKey(key), index);
	if (!record) return null;
	Sequence* sequence = createSequence(index);
	appendToSequence(sequence, record->key, null);
	return sequence;
}

// refnToSequence returns a single element Sequence with the record with given refn value; or null.
Sequence* refnToSequence(String value, RecordIndex* index, RefnIndex* findex) {
	String recordKey = searchRefnIndex(findex, value);
    if (!recordKey) return null;
	Sequence* sequence = createSequence(index);
	appendToSequence(sequence, recordKey, null);
	return sequence;
}

// stringToSequence returns a person sequence whose members "match" a string. Order: a) named
// Sequence (don't exist); b) key with or without leading 'I' (if used); c) REFN value; d) name.
Sequence* stringToSequence(String name, Database* database) {
	Sequence* sequence = null;
//    sequence = find_named_seq(name);
	if (!sequence) sequence = keyToSequence(name, database->recordIndex);
	if (!sequence) sequence = refnToSequence(name, database->recordIndex, database->refnIndex);
	if (!sequence) sequence = nameToSequence(name, database->recordIndex, database->nameIndex);
	return sequence;
}
