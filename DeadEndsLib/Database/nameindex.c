//
//  DeadEnds Library
//
//  nameindex.c implements the NameIndex, an index that maps Gedcom name keys to the Sets of person
//  record keys that have the names.
//
//  Created by Thomas Wetmore on 26 November 2022.
//  Last changed on 4 June 2025.
//

#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "name.h"
#include "nameindex.h"
#include "set.h"
#include "sort.h"

static NameIndexEl* createNameIndexEl(String nameKey);
static bool nameIndexDebugging = false;
static int numNameIndexBuckets = 2048;

// getKey gets the name key of a NameIndex element.
static String getKey(void* element) {
	return ((NameIndexEl*) element)->nameKey;
}

// compare compares two name keys.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// delete frees a NameIndex element.
// MNOTE: the nameKey is freed.
// MNOTE: the recordKeys Set is freed (the recordKeys it contains are not).
// MNOTE: the element itself is freed.
static void delete(void* element) {
	NameIndexEl *el = (NameIndexEl*) element;
	stdfree(el->nameKey);
	deleteSet(el->recordKeys);
	stdfree(el);
}

// createNameIndex creates a NameIndex.
NameIndex *createNameIndex(void) {
	return createHashTable(getKey, compare, delete, numNameIndexBuckets);
}

// deleteNameIndex deletes a name index.
void deleteNameIndex(NameIndex *nameIndex) {
	deleteHashTable(nameIndex);
}

// getNameIndex returns the NameIndex of all persons in a RootList.
NameIndex* getNameIndex(RootList* persons) {
	int numNamesFound = 0; // Debugging.
	NameIndex* nameIndex = createNameIndex();
	FORLIST(persons, element) // Loop over persons.
		GNode* root = (GNode*) element;
		String recordKey = root->key; // Key of record, used as is in name index.
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				numNamesFound++; // For debugging.
				String nameKey = nameToNameKey(name->value); // MNOTE: points to static memory.
				insertInNameIndex(nameIndex, nameKey, recordKey);
			}
		}
	ENDLIST
	if (nameIndexDebugging) printf("the number of names encountered is %d.\n", numNamesFound);
	return nameIndex;
}

// insertInNameIndex adds a (name key, person key) relationship to a NameIndex.
// MNOTE: nameKey is in static memory; it is saved if createNameIndexEl is called.
// MNOTE: recordKey is the record key from the database; it is not saved.
void insertInNameIndex(NameIndex* index, String nameKey, String recordKey) {
	if (nameIndexDebugging)
		printf("insertInNameIndex: nameKey, personKey: %s, %s\n", nameKey, recordKey);
	NameIndexEl* element = (NameIndexEl*) searchHashTable(index, nameKey); // Name key seen before?
	if (!element) { // No.
		element = createNameIndexEl(nameKey); // MNOTE: createNameIndexEl saves nameKey.
		addToHashTable(index, element, true);
	}
	Set* recordKeys = element->recordKeys;
	if (!isInSet(recordKeys, recordKey)) {
		addToSet(recordKeys, recordKey); // MNOTE: recordKey from Database stored in index as is.
	}
}

// removeFromNameIndex
void removeFromNameIndex(NameIndex* index, String nameKey, String recordKey) {
	NameIndexEl* el = (NameIndexEl*) searchHashTable(index, nameKey);
	if (!el) {
		// Log something happened.
		return;
	}
	Set* recordKeys = el->recordKeys;
	if (!isInSet(recordKeys, recordKey)) {
		// Log something happened.
		return;
	}
	removeFromSet(recordKeys, recordKey);
}

// Remove all names of a person from a NameIndex.
void removeNamesOfPersonFromIndex(NameIndex* index, GNode* person) {
	String recordKey = person->key;
	GNode* name = NAME(person);
	while (name) {
		String nameKey = nameToNameKey(name->value);
		removeFromNameIndex(index, nameKey, recordKey);
		name = name->sibling;
		if (name && nestr(name->tag, "NAME")) name = null;
	}
}

// searchNameIndex searches NameIndex for a name and returns the record keys that have the name.
// MNOTE: The set that is returned is in the NameIndex. It cannot be changed.
Set* searchNameIndex(NameIndex* index, String name) {
	String nameKey = nameToNameKey(name);
	NameIndexEl* element = searchHashTable(index, nameKey);
	return element == null ? null : element->recordKeys;
}

// showNameIndex shows the contents of a name index.
static void showSetElement(void* setEl) {
	printf("  %s\n", (String) setEl);
}
static void showElement(void* element) {
	Set* recordKeys = ((NameIndexEl*)element)->recordKeys;
	iterateSet(recordKeys, showSetElement);
}
void showNameIndex(NameIndex* index) {
	showHashTable(index, showElement);
}

// getSetKey gets the key of a Set element.
static String getSetKey(void* element) {
	return (String) element;
}

// compareSetKeys compares two keys of Set elements.
static int compareSetKeys(String a, String b) {
	return compareRecordKeys(a, b);
}

// createNameIndexEl creates and returns a NameIndexEl.
// MNOTE: nameKey is in static memory so must be saved.
// MNOTE: the Set is created to hold the record keys.
// MNOTE: the Set's delete function is null because the record keys are not freed.
static NameIndexEl* createNameIndexEl(String nameKey) {
	NameIndexEl* el = (NameIndexEl*) stdalloc(sizeof(NameIndexEl));
	el->nameKey = strsave(nameKey);
	el->recordKeys = createSet(getSetKey, compareSetKeys, null);
	return el;
}

// showNameIndexStats show the statistic of a NameIndex; for testing and debugging.
void showNameIndexStats(NameIndex* index) {
	int numNameKeys, numRecordKeys;
	getNameIndexStats(index, &numNameKeys, &numRecordKeys);
	fprintf(stderr, "Summary of Name Index: %d numkeys from %d names.\n", numNameKeys, numRecordKeys);
}

// getNameIndexStats returns statistics about the NameIndex; for testing and debugging.
void getNameIndexStats(NameIndex* index, int* pnumNameKeys, int* pnumRecordKeys) {
	int numNameKeys = 0;
	int numRecordKeys = 0;
	FORHASHTABLE(index, element)
		numNameKeys++;
		NameIndexEl* el = (NameIndexEl*) element;
		numRecordKeys += lengthSet(el->recordKeys);
	ENDHASHTABLE
	*pnumNameKeys = numNameKeys;
	*pnumRecordKeys = numRecordKeys;
}
