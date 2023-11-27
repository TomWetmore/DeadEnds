//
//  DeadEnds Project
//
//  nameindex.c -- Implements a name index that is built with a hash table. Gedcom names are
//    mapped to name keys. Name keys are the keys in the name index. The index maps the name
//    keys to the list of keys of the persons who have names that map to the name key.
//
//  Created by Thomas Wetmore on 26 November 2022.
//  Last changed on 22 November 2023.
//

#include "nameindex.h"
#include "name.h"
#include "sort.h"
#include "set.h"
#include "gedcom.h"

//  compareNameKeys -- Compare two name keys. This uses the standard strcmp.
//--------------------------------------------------------------------------------------------------
static int compareNameKeys(Word leftEl, Word rightEl)
{
	String a = ((NameElement*) leftEl)->nameKey;
	String b = ((NameElement*) rightEl)->nameKey;
	return strcmp(a, b);
}

//  getNameKey -- Get the name key of an element.
//--------------------------------------------------------------------------------------------------
static String getNameKey(Word element)
{
	return ((NameElement*) element)->nameKey;
}

//  deleteNameElement
//--------------------------------------------------------------------------------------------------
static void deleteNameElement(Word element)
{
	NameElement *nameEl = (NameElement*) element;
	stdfree(nameEl->nameKey);
	deleteSet(nameEl->recordKeys);
	stdfree(nameEl);
}

//  createNameIndex -- Create a name index from a hash table.
//--------------------------------------------------------------------------------------------------
NameIndex *createNameIndex(void)
{
	return createHashTable(compareNameKeys, deleteNameElement, getNameKey);
}

//  deleteNameIndex -- Delete a name index.
//--------------------------------------------------------------------------------------------------
void deleteNameIndex(NameIndex *nameIndex)
{
	deleteHashTable(nameIndex);
}

//  compareRecordKeysInSets -- Compare two record keys in a set of record keys in a name index.
//    Uses compareRecordKeys.
//-------------------------------------------------------------------------------------------------
static int compareRecordKeysInSets(Word a, Word b)
{
	return compareRecordKeys((String) a, (String) b);
}

static String getRecordKey(Word element) { return (String) element; }

//  insertNameIndex -- Add a (name key, person key) pair to a name index.
//    MNOTE: Describe the memory situration of the two parameters.
//--------------------------------------------------------------------------------------------------
void insertInNameIndex(NameIndex *index, String nameKey, String personKey)
//  index -- Name index to update.
//  nameKey -- Name key to insert.
//  personKey -- Person key to insert.
{
	//  DEBUG:
	//printf("insertInNameIndex: nameKey, personKey: %s, %s\n", nameKey, personKey);
	//  Hash the name key to get a bucket index; create a bucket if it does not exist.
	int hash = getHash(nameKey);
	//  DEBUG
	//printf("insertInNameIndex: hash: %d\n", hash);
	Bucket *bucket = index->buckets[hash];
	if (!bucket) {
		// DEBUG
		//printf("insertInNameIndex: bucket for nameKey doesn't exist.\n");
		bucket = createBucket();
		index->buckets[hash] = bucket;
	}

	//  See if there is an element for the name key; create if not.
	NameElement *element = searchBucket(bucket, nameKey, index->compare, index->getKey, null);
	if (!element) {
		//  DEBUG
		//printf("insertInNameIndex: element for nameKey %s doesn't exist.\n", nameKey);
		element = (NameElement*) stdalloc(sizeof(NameElement));
		element->nameKey = strsave(nameKey);  // MNOTE: nameKey is in data space.
		element->recordKeys = createSet(compareRecordKeysInSets, null, getRecordKey);
		appendToBucket(bucket, element);
	}
	//  Add the person key to element's set of person keys.
	if (!isInSet(element->recordKeys, personKey))
		//  DEBUG
		//printf("insertInNameIndex: calling addToSet with personKey: %s\n", personKey);
		addToSet(element->recordKeys, strsave(personKey));  //  MNOTE: personKey is in data space.
}

//  searchNameIndex -- Search a name index for a name.
//--------------------------------------------------------------------------------------------------
Set *searchNameIndex(NameIndex *index, String name)
//  index -- Name index, a specialized hash table.
//  name -- Name being search for.
{
	ASSERT(index && name);
	String nameKey = nameToNameKey(name);
	NameElement* element = searchHashTable(index, nameKey);
	return element == null ? null : element->recordKeys;
}

//  showNameIndex -- Show the contents of a name index; for debugging.
//--------------------------------------------------------------------------------------------------
void showNameIndex(NameIndex *index)
//  index -- Name index, a specialized hash table.
{
	for (int i = 0; i < MAX_HASH; i++) {

		if (!index->buckets[i]) continue;
		Bucket *bucket = index->buckets[i];
		Word *elements = bucket->elements;
		printf("Bucket %d:\n", i);

		for (int j = 0; j < bucket->length; j++) {

			NameElement* element = elements[j];
			printf("  Name key %s:\n", element->nameKey);

			Set *recordKeys = element->recordKeys;
			for (int k = 0; k < lengthSet(recordKeys); k++) {
				printf("    %s\n", (String) recordKeys->list->data[k]);
			}
		}
	}
}
