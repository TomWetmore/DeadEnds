//
//  DeadEnds
//
//  sequence.c -- The functions in this file implement the Sequence data type that handles
//    lists/sequences/sets of persons and sometimes families. This data type underlies the
//    indiseq data type of DeadEndsScript.
//
//  Created by Thomas Wetmore on 1 March 2023.
//  Last changed on 22 November 2023.
//

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

static bool debugging = false;

//  Compare functions used when sorting sequences of persons.
//--------------------------------------------------------------------------------------------------
static int nameCompare(SequenceEl, SequenceEl);  // Compare by Gedcom names.
static int keyCompare(SequenceEl, SequenceEl);  // Compare by key values.
static int valueCompare(SequenceEl, SequenceEl);  // Compare by value values.

static void sequenceSort(Word*, int, int(*compare)(Word, Word));

void baseFree(Word word) { stdfree(word); }

#define key_to_name(key, database)  (NAME(keyToPerson(key, database))->value)

//  createSequence -- Create a new sequence on the heap.
//--------------------------------------------------------------------------------------------------
Sequence *createSequence(Database *database)
{
	if (debugging) printf("createSequence");
	Sequence *seq = (Sequence*) stdalloc(sizeof(Sequence));
	seq->size = 0;
	seq->max = 20;
	IData(seq) = (SequenceEl*) stdalloc(20*sizeof(SequenceEl));
	seq->database  = database;
	seq->flags = 0;
	return seq;
}

//  lengthSequence -- Return the length of a sequence.
//--------------------------------------------------------------------------------------------------
int lengthSequence(Sequence *sequence)
{
	return sequence->size;
}

//  deleteSequence -- Remove a sequence from the heap.
//    TODO: May need a better system for freeing the elements.
//--------------------------------------------------------------------------------------------------
void deleteSequence(Sequence *sequence, bool fval)
//  sequence -- Sequence to delete.
//  fval -- Free the values.
{
	SequenceEl *d = IData(sequence);
	for (int i = 0, n = sequence->size; i < n; i++, d++) {
		stdfree((*d)->key);
		if ((*d)->name) stdfree((*d)->name);
		//if (fval && sval(*d)) stdfree(sval(*d));  // TODO: THIS IS AN OFFICIAL LOOSEEND.
	}
	stdfree(IData(sequence));
	stdfree(sequence);
}

//  copySequence -- Return a copy of a sequence.
//    NOTE: Not used anywhere yet.
//--------------------------------------------------------------------------------------------------
Sequence *copySequence(Sequence *seq)
{
	if (!seq) return null;
	Sequence *new = createSequence(seq->database);
	FORSEQUENCE(seq, el, num)
		appendToSequence(new, el->key, el->name, el->value);
	ENDSEQUENCE
	return new;
}

//  appendToSequence -- Create and append a new element to a sequence.
//--------------------------------------------------------------------------------------------------
void appendToSequence(Sequence *sequence, String key, String name, PValue *val)
//  sequence -- Sequence to add an element to.
//  key -- Key of the element; it cannot be null.
//  name -- Name of the element (if a person); it may be null.
//  val -- an extra value; may be null; otherwise is must point to a pvalue??
{
	if (!sequence || !key) return;
	int n = sequence->size;
	SequenceEl* old = IData(sequence);
	SequenceEl el = (SequenceEl) stdalloc(sizeof(*el));
	el->key = strsave(key);  // Sequence elements own their copies of the keys.
	el->name = null;
	if (*key == 'I') {
		if (name) el->name = strsave(name);  // They also own the name fields.
		else el->name = strsave(key_to_name(key, sequence->database));
	}
	el->value = val;  // They don't own the value fields.
	//spri(el) = 0;

	// Check if the list of elements needs to grow.
	if ((n = sequence->size) >= sequence->max)  {
		int m = 3*n;
		SequenceEl* new = (SequenceEl*) stdalloc(m*sizeof(SequenceEl));
		for (int i = 0; i < n; i++)
			new[i] = old[i];
		stdfree(old);
		IData(sequence) = old = new;
		sequence->max = m;
	}
	old[(sequence->size)++] = el;
	sequence->max = 0;
}

//  rename_indiseq -- Update element name with standard name
//--------------------------------------------------------------------------------------------------
void rename_indiseq(Sequence *seq, String key)
//  seq -- Sequence with the element to change.
//  key -- Key of the element to change.
{
	int i, n;
	SequenceEl *data;
	if (!seq || !key || *key != 'I') return;  // We learn that key means a Gedcom key.
	n = seq->size;
	data = IData(seq);
	// Look through all the elements for the element with the given key.
	for (i = 0; i < n; i++) {
		if (eqstr(key, (data[i])->key)) {
			// If the key is in the sequence, change the name in the sequence to reflect the
			// name currently in that person's record.
			if ((data[i])->name) stdfree((data[i])->name);
			(data[i])->name = strsave(key_to_name(key, seq->database));
		}
	}
}

//  isInSequence -- See if an element with a given key is in a sequence. NOTE: Consider what
//    could be done if the sequence is sorted.
//--------------------------------------------------------------------------------------------------
bool isInSequence(Sequence *seq, String key)
//  seq -- Sequence to search for the key in.
//  key -- Key of the person (only persons?) to search for.
{
	if (!seq || !key) return false;
	SequenceEl* data = IData(seq);
	for (int i = 0; i < seq->size; i++) {
		if (eqstr(key, (data[i])->key)) return true;
	}
	return false;
}

//  removeFromSequence -- Remove an element from a sequence. If key is not null use it to find
//    the element. Othewise use the index. The element must be a person.
//--------------------------------------------------------------------------------------------------
bool removeFromSequence(Sequence *sequence, String key, String name, int index)
//  sequence -- Sequence to remove the element from.
//  key -- Key of the element; may be null.
//  name -- Name of element to be removed; may be null.
//  index -- Index of the element to be removed; may be computed?????
{
	if (!sequence) return false;
	int len = sequence->size;
	SequenceEl* data = IData(sequence);
	int i = 0;
	if (key) {
		if (*key != 'I') return false;  // Gotta be a person.
		for (i = 0; i < len; i++) {
			if (eqstr(key, (data[i])->key) && (!name || eqstr(name, (data[i])->name))) break;
		}
		if (i >= len) return false;
		index = i;
	}
	if (index < 0 || index >= len) return false;
	len--;
	SequenceEl el = data[index];
	for (i = index; i < len; i++)
		data[i] = data[i+1];
	(sequence->size)--;
	stdfree(el->key);
	if (el->name) stdfree(el->name);
	stdfree(el);
	return true;
}

//  element_indiseq -- Return an element from a sequence
//--------------------------------------------------------------------------------------------------
bool element_indiseq (Sequence *seq, int index, String* pkey, String* pname)
//  seq -- Sequence.
//  index -- Element index in sequence.
//  pkey -- (out) Key of the element.
//  name -- (out) Name of the element.
{
	*pkey = *pname = null;
	if (!seq || index < 0 || index > seq->size - 1) return false;
	*pkey =  (IData(seq)[index])->key;
	*pname = (IData(seq)[index])->name;
	return true;
}

//  nameCompare -- Compare two sequence elements by their name fields. The elements must hold
//    persons.
//--------------------------------------------------------------------------------------------------
static int nameCompare(SequenceEl el1, SequenceEl el2)
//  el1, el2 -- The two elements with the names to be compared.
{
	int rel = compareNames(el1->name, el2->name);
	if (rel) return rel;  // If names are not equal return their relationship.
	return compareRecordKeys(el1->key, el2->key);
}

//  keyCompare -- Compare two sequence elements by their record key fields.
//--------------------------------------------------------------------------------------------------
static int keyCompare(SequenceEl el1, SequenceEl el2)
//  el1, el2 -- The elements with integer keys to be compared.
{
	return compareRecordKeys(el1->key, el2->key);
}

//  valueCompare -- Compare two elements of a sequence by their values. TODO: This must be
//    converted to using a PValue like object for the value that has a type to consult.
//--------------------------------------------------------------------------------------------------
static int valueCompare (SequenceEl el1, SequenceEl el2)
//  el1, el2 -- The two elements with values to be compared.
{
	/* WARNING: this is not correct as sval() is a PVALUE structure */
	return (int) (long) el1->value - (int) (long) el2->value;
}

//  nameSortSequence -- Sort a sequence by the name fields of the elements.
//    TODO: WHAT IS THE NAME FIELDS HAVEN'T BEEN COMPUTED YET?
//--------------------------------------------------------------------------------------------------
void nameSortSequence(Sequence *seq)
//  seq -- The sequence to be name sorted.
{
	// The sequence may be sorted.
	if (seq->max & NAMESORT) return;

	// Perform the sort and set the flags.
	sequenceSort((Word*)IData(seq), seq->size, (int(*)(Word, Word))nameCompare);
	quickSort(0, seq->size - 1);
	seq->max &= ~KEYSORT;
	seq->max &= ~VALUESORT;
	seq->max |= NAMESORT;
}

//  keySortSequence -- Sort a sequence by key.
//--------------------------------------------------------------------------------------------------
void keySortSequence(Sequence *seq)
//  seq -- Sequence to be sorted.
{
	if (seq->max & KEYSORT) return;
	sequenceSort((Word*)IData(seq), seq->size, (int(*)(Word, Word))keyCompare);
	quickSort(0, seq->size - 1);
	seq->max &= ~NAMESORT;
	seq->max &= ~VALUESORT;
	seq->max |= KEYSORT;
}

//  valueSortSequence -- Sort a sequence by value.
//--------------------------------------------------------------------------------------------------
void valueSortSequence(Sequence *sequence)
//  seq -- Sequence to be sorted.
{
	if (sequence->max & VALUESORT) return;
	sequenceSort((Word*)IData(sequence), sequence->size, (int(*)(Word, Word))valueCompare);
	quickSort(0, sequence->size - 1);
	sequence->max &= ~NAMESORT;
	sequence->max &= ~KEYSORT;
	sequence->max |= VALUESORT;
}

static void sequenceSort(Word* data, int length, int(*compare)(Word, Word))
{
	extern Word* ldata;
	extern int (*lcmp)(Word, Word);
	ldata = data;
	lcmp = compare;
}

//  uniqueSequence -- Create and return a new sequence that contains only the unique elements from
//    the given sequence.
//--------------------------------------------------------------------------------------------------
Sequence *uniqueSequence(Sequence *sequence)
//  sequence -- The sequence to be uniqued.
{
	ASSERT(sequence);
	Sequence *unique = createSequence(sequence->database);
	int n = sequence->size;
	if (n == 0 || (sequence->flags & UNIQUED)) return unique;  // Return if no action needed.
	if (!(sequence->flags & KEYSORT)) keySortSequence(sequence);
	SequenceEl *d = IData(sequence);
	appendToSequence(unique, strsave(d[0]->key), null, null);
	int i, j;
	for (j = 0, i = 1; i < n; i++) {
		if (nestr(d[i]->key, d[j]->key)) {
			appendToSequence(unique, strsave(d[i]->key), null, null);
			j = i;
		}
	}
	unique->flags |= UNIQUED;
	return unique;
}

//  uniqueSequenceInPlace -- Remove duplicates (have the same key) elements from a sequence.
//    No new sequence is created.
//
//  MNOTE: This has a MEMORY LEAK -- the elements removed are not managed properly.
//--------------------------------------------------------------------------------------------------
void uniqueSequenceInPlace(Sequence *sequence)
//  sequence -- The sequence to be uniqued in place.
{
	int i, j;
	if (!sequence) return;
	int n = sequence->size;
	SequenceEl *d = IData(sequence);
	if (n == 0 || (sequence->flags & UNIQUED)) return;
	if (!(sequence->flags & KEYSORT)) keySortSequence(sequence);
	for (j = 0, i = 1; i < n; i++)
		if (nestr(d[i]->key, d[j]->key)) d[++j] = d[i];
	sequence->size = j + 1;
	sequence->flags |= UNIQUED;
}

//  unionSequence -- Create the union of two sequences.
//--------------------------------------------------------------------------------------------------
Sequence *unionSequence(Sequence *one, Sequence *two)
//  one, two -- The two sequences to be unioned.
{
	ASSERT(one && two);
	ASSERT(one->database == two->database);
	if (!one || !two) return null;

	// Make sure the sequences are sorted by key and uniqued.
	if (!(one->flags & KEYSORT)) keySortSequence(one);
	if (!(one->flags & UNIQUED)) uniqueSequenceInPlace(one);
	if (!(two->flags & KEYSORT)) keySortSequence(two);
	if (!(two->flags & UNIQUED)) uniqueSequenceInPlace(two);

	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence *three = createSequence(one->database);
	SequenceEl* u = IData(one);
	SequenceEl* v = IData(two);
	int i = 0, j = 0, rel;
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->key, v[j]->key)) < 0) {
			appendToSequence(three, (u[i])->key, null, u[i]->value);
			i++;
		} else if (rel > 0) {
			appendToSequence(three, (v[j])->key, null, v[j]->value);
			j++;
		} else {
			appendToSequence(three, (u[i])->key, null, u[i]->value);
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, (u[i])->key, null, u[i]->value);
		i++;
	}
	while (j < m) {
		appendToSequence(three, (v[j])->key, null, v[j]->value);
		j++;
	}
	three->flags = KEYSORT|UNIQUED;
	return three;
}

//  intersectSequence -- Create the intersection of two sequences.
//--------------------------------------------------------------------------------------------------
Sequence *intersectSequence(Sequence *one, Sequence *two)
//  one, two -- Two sequences to intersect.
{
	ASSERT(one && two);
	ASSERT(one->database == two->database);
	if (!one || !two) return null;  // Nothing to do.
	int rel;

	// Make sure the two sequences are sorted by key and have unique elements.
	if (!(one->flags & KEYSORT)) keySortSequence(one);
	if (!(one->flags & UNIQUED)) uniqueSequenceInPlace(one);
	if (!(two->flags & KEYSORT)) keySortSequence(two);
	if (!(two->flags & UNIQUED)) uniqueSequenceInPlace(two);

	// Prepare to create the intersection.
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence *three = createSequence(one->database);
	int i = 0, j = 0;
	SequenceEl* u = IData(one);
	SequenceEl* v = IData(two);

	// Iteration that does the intersection. Using the integer keys not the string keys.
	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->key, v[j]->key)) < 0) {
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			//key = strsave((u[i])->seKey);
			appendToSequence(three, (u[i])->key, null, u[i]->value);
			i++; j++;
		}
	}
	three->flags = KEYSORT|UNIQUED;
	return three;
}

//  differenceSequence -- Create the difference of two sequences.
//--------------------------------------------------------------------------------------------------
Sequence *differenceSequence (Sequence *one, Sequence *two)
//  one, two --
{
	ASSERT(one && two);
	ASSERT(one->database == two->database);
	if (!one || !two) return null;

	// Make sure the two sequences are sorted by key and have unique elements.
	if (!(one->flags & KEYSORT)) keySortSequence(one);
	if (!(one->flags & UNIQUED)) uniqueSequenceInPlace(one);
	if (!(two->flags & KEYSORT)) keySortSequence(two);
	if (!(two->flags & UNIQUED)) uniqueSequenceInPlace(two);
	int n = lengthSequence(one);
	int m = lengthSequence(two);
	Sequence *three = createSequence(one->database);
	int i = 0, j = 0;
	SequenceEl *u = IData(one);
	SequenceEl *v = IData(two);
	int rel;

	while (i < n && j < m) {
		if ((rel = compareRecordKeys(u[i]->key, v[j]->key)) < 0) {
			appendToSequence(three, (u[i])->key, null, u[i]->value);
			i++;
		} else if (rel > 0) {
			j++;
		} else {
			i++; j++;
		}
	}
	while (i < n) {
		appendToSequence(three, (u[i])->key, null, u[i]->value);
		i++;
	}
	three->flags = KEYSORT|UNIQUED;
	return three;
}

//  parentSequence -- Create a sequence of the parents of the persons in a sequence.
//--------------------------------------------------------------------------------------------------
Sequence *parentSequence(Sequence *sequence)
//  sequence -- Sequence of persons to find the parents of.
{
	ASSERT(sequence && sequence->database);
	if (!sequence) return null;
	Database *database = sequence->database;
	StringTable *table = createStringTable();  //  Keep track of parents added to the sequence.
	Sequence *parents = createSequence(sequence->database);
	String key;

	// For each person in the original sequence.
	FORSEQUENCE(sequence, el, num)
		GNode *indi = keyToPerson(el->key, database); // Get the person from its key.
		GNode *fath = personToFather(indi, database);   // Get the person's father, if there.
		GNode *moth = personToMother(indi, database);   // Get the person's mother, if there.

		// If the father hasn't been added, add to sequence and table.
		if (fath && !isInHashTable(table, key = personToKey(fath))) {
			appendToSequence(parents, key, null, el->value);
			insertInStringTable(table, key, null);
		}

		// If the mother hasn't been added, add to the sequence and table.
		if (moth && !isInHashTable(table, key = personToKey(moth))) {
			appendToSequence(parents, key, null, el->value);
			insertInStringTable(table, key, null);
		}
	ENDSEQUENCE
	deleteHashTable(table);
	return parents;
}

//  childSequence -- Create a sequence with the children of the persons in another sequencee.
//--------------------------------------------------------------------------------------------------
Sequence *childSequence(Sequence *sequence)
//  sequence -- Sequence of persons to find all the children of.
{
	if (!sequence) return null;
	StringTable *table = createStringTable();
	Database *database = sequence->database;
	Sequence *children = createSequence(database);

	// For each person in the sequence.
	FORSEQUENCE(sequence, el, num)
		GNode *person = keyToPerson(el->key, database);

		//  For each family the person is a spouse in.
		FORFAMSS(person, fam, database)
			//  For the children in that family.
			FORCHILDREN(fam, chil, num2, database)
				//  Add child to sequence if not there.
				String key = personToKey(chil);
				if (!isInHashTable(table, key)) {
					appendToSequence(children, key, null, null);
					insertInStringTable(table, key, null);
				}
			ENDCHILDREN
		ENDFAMSS
	ENDSEQUENCE
	deleteHashTable(table);
	return children;
}

//  personToChildren -- Create the sequence of a person's children.
//--------------------------------------------------------------------------------------------------
Sequence *personToChildren(GNode* person, Database *database)
//  indi -- Person whose children are sought.
{
	if (!person) return null;
	Sequence *children = createSequence(database);

	FORFAMSS(person, family, database)
		FORCHILDREN(family, child, count, database)
			appendToSequence(children, personToKey(child), null, null);
		ENDCHILDREN
	ENDFAMSS
	if (lengthSequence(children)) return children;
	deleteSequence(children, false);
	return null;
}

//  personToSpouses -- Create the sequence of a person's spouses. Looks at each family the person is
//    in as a spouse. The person must be male or female. The spouse must have the opposite sex. If
//    there is more than one spouse of the opposite sex in a family, the first is selected.
//--------------------------------------------------------------------------------------------------
Sequence *personToSpouses(GNode *person, Database *database)
//  indi -- Person whose spouses are sought.
{
	SexType sex = SEXV(person);
	if (sex != sexMale && sex != sexFemale) return null;
	Sequence *spouses = createSequence(database);
	FORFAMSS(person, family, database)
		GNode *spouse = (sex == sexMale) ? familyToWife(family, database) : familyToHusband(family, database);
		if (spouse) appendToSequence(spouses, personToKey(spouse), null, null);
	ENDFAMSS
	if (lengthSequence(spouses)) return spouses;
	deleteSequence(spouses, false);  //  MNOTE: Should the second argument be false?
	return null;

}

//  personToFathers -- Create the sequence of a person's fathers.
//--------------------------------------------------------------------------------------------------
Sequence *personToFathers(GNode *person, Database *database)
//  person -- Person to get the fathers of.
{
	if (!person) return null;
	Sequence *fathers = createSequence(database);

	FORFAMCS(person, family, database)  // For each family the person is a child in...
		FORHUSBS(family, husb, database)  // For each husband in that family...
			appendToSequence(fathers, personToKey(husb), null, null);  // Add him to the sequence.
		ENDHUSBS
	ENDFAMCS

	if (lengthSequence(fathers)) return fathers;
	deleteSequence(fathers, false);
	return null;
}

//  personToMothers -- Create sequence of person's mothers
//--------------------------------------------------------------------------------------------------
Sequence *personToMothers (GNode* indi, Database *database)
//  indi -- Person to get the mothers of.
{
	if (!indi) return null;
	Sequence *mothers = createSequence(database);
	FORFAMCS(indi, fam, database)  // For each family the person is a child in...
		FORWIFES(fam, wife, database)  // For each wife in that family...
			appendToSequence(mothers, personToKey(wife), null, null);  // Add her to the sequence.
		ENDWIFES
	ENDFAMCS
	if (lengthSequence(mothers)) return mothers;
	deleteSequence(mothers, false);
	return null;
}

//  personToFamilies -- Create the sequence of a person's families. If the boolean is true the
//    families are the families the person is a spouse in, else they are the families the person
//    is a child in.
//--------------------------------------------------------------------------------------------------
Sequence *personToFamilies (GNode* person, bool fams, Database *database)
//  person -- Person to get the families of.
//  fams -- Families as spouse (true) or families as child (false).
{
	if (!person) return null;
	String key;
	Sequence *families = createSequence(database);
	if (fams) {
		FORFAMSS(person, family, database)
			key = familyToKey(family);
			appendToSequence(families, key, null, null);
		ENDFAMSS
	} else {
		FORFAMCS(person, family, database)
			key = familyToKey(family);
			appendToSequence(families, key, null, null);
		ENDFAMCS
	}
	if (lengthSequence(families) > 0) return families;
	deleteSequence(families, false);
	return null;
}

//  familyToChildren -- Create the sequence of a family's children.
//--------------------------------------------------------------------------------------------------
Sequence *familyToChildren(GNode* family, Database *database)
//  family -- Family to get the child sequence of.
{
	if (!family) return null;
	Sequence *children = createSequence(database);
	FORCHILDREN(family, chil, num, database) {
		appendToSequence(children, personToKey(chil), null, null);
	} ENDCHILDREN
	if (lengthSequence(children) > 0) return children;
	deleteSequence(children, false);
	return null;
}

// familyToFathers -- Create the sequence of a family's fathers/husbands.
//--------------------------------------------------------------------------------------------------
Sequence *familyToFathers(GNode* fam, Database *database)
//  fam -- Root node of a family.
{
	if (!fam) return null;
	Sequence *seq = createSequence(database);
	FORHUSBS(fam, husb, database)
		appendToSequence(seq, personToKey(husb), null, null);
	ENDHUSBS
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq, false);
	return null;
}

//  familyToMothers -- Create the sequence of a family's mothers/wives.
//--------------------------------------------------------------------------------------------------
Sequence *familyToMothers(GNode *fam, Database *database)
//  fam -- Root node of a family.
{
	if (!fam) return null;
	Sequence *seq = createSequence(database);
	FORWIFES(fam, husb, database) {
		//key = personToKey(husb);
		appendToSequence(seq, personToKey(husb), null, null);
	}
	ENDWIFES
	if (lengthSequence(seq)) return seq;
	deleteSequence(seq, false);
	return null;
}

//  siblingSequence -- Create sibling sequence of a sequence
//--------------------------------------------------------------------------------------------------
Sequence *siblingSequence(Sequence *sequence, bool close)
//  sequence -- Sequence to get the siblings of.
//  close -- If true include persons in the initial sequence in the sibling sequence.
{
	GNode *fam;
	String key;
	Database *database = sequence->database;
	StringTable *tab = createStringTable();
	Sequence *familySequence = createSequence(database);
	Sequence *siblingSequence = createSequence(database);
	FORSEQUENCE(sequence, element, num) {
		//  TODO: THIS ONLY USES THE FIRST FAMC FAMILY, WHICH IN 99.9% OF THE CASES IS OKAY.
		//  BUT TO BE CONSISTENT WITH OTHER SITUATIONS WHERE THERE ARE MULTIPLE FAMC NODES,
		//  IT MIGHT BE BETTER TO GO THROUGH THEM ALL.
		GNode *person = keyToPerson(element->key, database);
		if ((fam = personToFamilyAsChild(person, database)) == NULL) {
			appendToSequence(familySequence, familyToKey(fam), null, null);
		}
		if (!close) insertInStringTable(tab, element->key, null);
	}
	ENDSEQUENCE
	FORSEQUENCE(familySequence, el, num)
		fam = keyToFamily(el->key, database);
		FORCHILDREN(fam, chil, num2, database)
			key = personToKey(chil);
			if (!isInHashTable(tab, key)) {
				appendToSequence(siblingSequence, key, null, null);
				insertInStringTable(tab, key, null);
			}
		ENDCHILDREN
	ENDSEQUENCE
	deleteHashTable(tab);
	deleteSequence(familySequence, false);
	return siblingSequence;
}

//  ancestorSequence -- Create the ancestor sequence of a sequence. The persons in the original
//    sequence are not in the ancestor sequence unless they are also an ancestor of someone in
//    the original sequence.
//--------------------------------------------------------------------------------------------------
Sequence *ancestorSequence(Sequence *startSequence)
{
	ASSERT(startSequence);
	Database *database = startSequence->database;
	StringTable *ancestorKeys = createStringTable();  // Keys of all persons encountered.
	List *ancestorQueue = createList(null, baseFree, null);  // Queue of ancestor keys to process.
	Sequence *ancestorSequence = createSequence(database);  // The sequence holding the ancestors.

	//  Initialize the ancestor queue with the keys of persons in the start sequence.
	FORSEQUENCE(startSequence, el, num)
		enqueueList(ancestorQueue, (Word) el->key);
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
			appendToSequence(ancestorSequence, parentKey, null, null);
			//  MNOTE: Lists don'e save their elements.
			enqueueList(ancestorQueue, strsave(parentKey));
			insertInStringTable(ancestorKeys, parentKey, null);
		}
		// If the mother has not been seen before, add her to the table and sequence.
		if (mother && !isInHashTable(ancestorKeys, parentKey = mother->key)) {
			appendToSequence(ancestorSequence, parentKey, null, null);
			//  MNOTE: Lists don't save their elements.
			enqueueList(ancestorQueue, strsave(parentKey));
			insertInStringTable(ancestorKeys, parentKey, null);
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
	String key, descendentKey, familyKey;
	StringTable *descendentKeys = createStringTable();  //  Keys of all descendents processed.
	StringTable *familyKeys = createStringTable();  //  Keys of all families processed.
	List *descendentQueue = createList(null, baseFree, null);  //  Queue of descendent keys.
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
		FORFAMSS(person, family, database) {
			if (isInHashTable(familyKeys, familyKey = familyToKey(family))) goto a;
			insertInStringTable(familyKeys, strsave(familyKey), null);
			FORCHILDREN(family, child, num, database)
				if (!isInHashTable(descendentKeys, descendentKey = personToKey(child))) {
					appendToSequence(descendentSequence, descendentKey, null, null);
					//  MNOTE: strsave required -- lists don't save their elements.
					enqueueList(descendentQueue, strsave(descendentKey));
					insertInStringTable(descendentKeys, descendentKey, null);
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
	StringTable *table = createStringTable();
	Sequence *spouses = createSequence(database);
	FORSEQUENCE(sequence, el, num)   //  For each person in the original sequence
		GNode *person = keyToPerson(el->key, database);
		FORSPOUSES(person, spouse, fam, num1, database)   // For each spouse that that person has
			String key = personToKey(spouse);   // Get the key of the spouse
			if (!isInHashTable(table, key)) {   // If the spouse's key isn't in the table.
				appendToSequence(spouses, key, null, el->value);  // Add the spouse.
				insertInStringTable(table, key, null);
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
	StringTable *personTable = createStringTable();  //  Table of person keys.
	StringTable *familyTable = createStringTable();  //  Table of family keys.

	//  Add all person keys to the person key hash table.
	FORSEQUENCE(sequence, element, num)
		insertInStringTable(personTable, element->key, null);
	ENDSEQUENCE

	//  Loop through each person in the sequence.
	FORSEQUENCE(sequence, element, num)
		GNode *person = keyToPerson(element->key, database);  // Get the person.
		SexType sex = SEXV(person);  //  And the person's sex.
		
		//  Check the person's parent families to see if any FAMC families should be output.
		FORFAMCS(person, family, database)
			if (isInHashTable(familyTable, family->key)) goto a;
			normalizeFamily(family);
			GNode *husband = HUSB(family);
			if (husband && isInHashTable(personTable, husband->value)) {
				appendToSequence(familySequence, familyToKey(family), null, null);
				insertInStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			GNode *wife = WIFE(family);
			if (wife && isInHashTable(personTable, wife->value)) {
				appendToSequence(familySequence, familyToKey(family), null, null);
				insertInStringTable(familyTable, familyToKey(family), null);
				goto a;
			}
			//  Check whether any of the children in this family are in the sequence.
			FORCHILDREN(family, child, count, database)
				String childKey = personToKey(child);
				if (isInHashTable(personTable, childKey)) {
					appendToSequence(familySequence, familyToKey(family), null, null);
					insertInStringTable(familyTable, familyToKey(family), null);
					goto a;
				}
			ENDCHILDREN
		ENDFAMCS

		//  Check the person's as parent families to see if they should output.
	a:	FORFAMSS(person, family, database)
			if (isInHashTable(familyTable, familyToKey(family))) goto b;
			GNode *spouse = familyToSpouse(family, oppositeSex(sex), database);
			if (spouse && isInHashTable(personTable, personToKey(spouse))) {
				appendToSequence(familySequence, familyToKey(family), null, null);
				insertInStringTable(familyTable, familyToKey(family), null);
			}
	b:;	ENDFAMSS
	ENDSEQUENCE

	FORSEQUENCE(personSequence, element, count)
		writeLimitedPerson(keyToPerson(element->key, database));
	ENDSEQUENCE

	FORSEQUENCE(familySequence, element, count)
		writeLimitedFamily(keyToFamily(element->key, database));
	ENDSEQUENCE
	deleteSequence(familySequence, false);
	deleteHashTable(personTable);
	deleteHashTable(familyTable);
}

bool limitPersonNode(GNode *node, int level)
{
	//  FOR TESTING. WE SHOULD SEE NO BIRTH EVENTS IN THE OUTPUT.
	if (level == 1 && eqstr(node->tag, "BIRT")) return false;
	writeGNode(stdout, level, node, false);
	return true;
}

//  writeLimitedPerson -- Used when writing a sequence of persons to a Gedcom file. Can only
//    include links to families (and persons and others objects maybe eventually) that are
//    also to be in the Gedcom file.
//-------------------------------------------------------------------------------------------------
static void writeLimitedPerson (GNode *person)
//  person -- Person to write to a limited and closed Gedcom file.
{
	printf("writeLimitedPerson: %s\n", person->key);
	traverseNodes(person, 0, limitPersonNode);
}


static void writeLimitedFamily (GNode *family)
{
	printf("writeLimitedFamily: %s\n", family->key);
}

//  nameToSequence -- Return the sequence of persons who match a name. The name must be formatted
//    as a Gedcom name. However, if the first letter of the given names is a '*', the given
//    name is treated as a wild card, and the sequencce will contain all persons that match the
//    surname.
//--------------------------------------------------------------------------------------------------
Sequence *nameToSequence(String name, Database *database)
//  name -- Name.
//  index -- Name index with the name information.
{
	if (!name || *name == 0) return null;

	int num;
	Sequence *seq = null;

	// Simple case -- the name does not start with a '*'.
	if (*name != '*') {
		String *keys = personKeysFromName(name, database, &num /*true*/);
		if (num == 0) return null;
		seq = createSequence(database);
		for (int i = 0; i < num; i++)
			appendToSequence(seq, keys[i], null, null);
		nameSortSequence(seq);
		return seq;
	}

	// Wild card case -- the name starts with a '*', which matches all firstnames.
	char scratch[MAXLINELEN+1];
	sprintf(scratch, "a/%s/", getSurname(name));
	for (int c = 'a'; c <= 'z'; c++) {
		scratch[0] = c;
		String *keys = personKeysFromName(scratch, database, &num/*, true*/);
		if (num == 0) continue;
		if (!seq) seq = createSequence(database);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], null, null);
		}
	}
	scratch[0] = '$';
	String *keys = personKeysFromName(scratch, database, &num/*, true*/);
	if (num) {
		if (!seq) seq = createSequence(database);
		for (int i = 0; i < num; i++) {
			appendToSequence(seq, keys[i], null, null);
		}
	}
	if (seq) {
		uniqueSequence(seq);
		nameSortSequence(seq);
	}
	return seq;
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
