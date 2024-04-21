//
// DeadEnds Project
//
// nameindex.c implements the NameIndex data type. Gedcom names are mapped to name keys which are
// keys in a NameIndex. The index maps the name keys to the Set of person keys who have names that
// match the name key.
//
// Created by Thomas Wetmore on 26 November 2022.
// Last changed on 27 March 2024.
//

#include "nameindex.h"
#include "name.h"
#include "sort.h"
#include "set.h"
#include "gedcom.h"

// Local functions
static NameElement* createNameElement(String nameKey, String recordKey);
static String getIndexKey(void*);
static int compareIndexKeys(String, String);
static void deleteIndexEl(void*);
static String getSetKey(void*);
static int compareSetKeys(String, String);
static void deleteSetEl(void*);

static int numNameIndexBuckets = 2048;

// createNameIndex creates and returns a NameIndex.
NameIndex *createNameIndex(void) {
	return createHashTable(getIndexKey, compareIndexKeys, deleteIndexEl, numNameIndexBuckets);
}

// deleteNameIndex deletes a name index.
void deleteNameIndex(NameIndex *nameIndex) {
	deleteHashTable(nameIndex);
}

// insertInNameIndex adds a (name key, person key) relationship to a NameIndex.
void insertInNameIndex(NameIndex* index, String nameKey, String recordKey)
{
	//printf("insertInNameIndex: nameKey, personKey: %s, %s\n", nameKey, personKey); // DEBUG
	NameElement* element = (NameElement*) searchHashTable(index, nameKey);
	if (!element) {
		element = createNameElement(nameKey, recordKey);
		addToHashTable(index, element, true);
	}
	Set* recordKeys = element->recordKeys;
	if (!isInSet(recordKeys, recordKey)) {
		addToSet(recordKeys, recordKey);
	}
}

// searchNameIndex searches a NameIndex for a name and returns the Set of record keys that
// have the name.
Set *searchNameIndex(NameIndex *index, String name) {
	String nameKey = nameToNameKey(name);
	NameElement* element = searchHashTable(index, nameKey);
	return element == null ? null : element->recordKeys;
}

// showNameIndex shows the contents of a name index. Two static functions below required.
static void showSetElement(void* setEl) {
	printf("  %s\n", (String) setEl);
}
static void showElement(void* element) {
	Set* recordKeys = ((NameElement*)element)->recordKeys;
	iterateSet(recordKeys, showSetElement);
}
void showNameIndex(NameIndex* index) {
	showHashTable(index, showElement);
}

// createNameElement creates and returns a NameElement.
static NameElement* createNameElement(String nameKey, String recordKey) {
	NameElement* element = (NameElement*) stdalloc(sizeof(NameElement));
	element->nameKey = strsave(nameKey);  // MNOTE: nameKey is in data space.
	element->recordKeys = createSet(getSetKey, compareSetKeys, deleteSetEl);
	return element;
}

// getIndexKey gets the name key of a NameIndex element.
static String getIndexKey(void* element) {
	return ((NameElement*) element)->nameKey;
}

// compareIndexKeys compares two name keys.
static int compareIndexKeys(String a, String b) {
	return strcmp(a, b);
}

// deleteIndexEl frees a NameIndex element.
static void deleteIndexEl(void* element) {
	NameElement *nameEl = (NameElement*) element;
	stdfree(nameEl->nameKey);
	deleteSet(nameEl->recordKeys);
	stdfree(nameEl);
}

// getSetKey gets the key of a Set element.
static String getSetKey(void* element) {
	return (String) element;
}

// compareSetKeys compares two keys of Set elements.
static int compareSetKeys(String a, String b) {
	return compareRecordKeys(a, b);
}

static void deleteSetEl(void* element) {
	// NOTE: Currently a no-op. Should it remain so?
}
