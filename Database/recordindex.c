//
//  DeadEnds
//
//  recordindex.c -- Data type that implements the indexes to person, family, etc, Gedcom records
//    in their internal node tree form. A record index is a thin layer over the general hash table
//    type. The type RecordIndex is a synonym of HashTable.
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 14 February 2024.
//

#include "recordindex.h"
#include "list.h"
#include "sort.h"
#include "gedcom.h"

//  RecordIndexEls -- Element compare function needed by the record index hash table.
//--------------------------------------------------------------------------------------------------
static int compareRecordIndexEls(Word leftEl, Word rightEl)
{
	String a = ((RecordIndexEl*) leftEl)->root->key;
	String b = ((RecordIndexEl*) rightEl)->root->key;
	return strcmp(a, b);
}

//  deleteRecordIndexEl -- Delete function needed by the record index hash table.
//--------------------------------------------------------------------------------------------------
static void deleteRecordIndexEl(Word word)
{
	RecordIndexEl* element = (RecordIndexEl*) word;
	stdfree(element);
}

//  recordIndexElKey -- Return the key of record index element. This is the key of the root node.
//--------------------------------------------------------------------------------------------------
static String recordIndexElKey(Word word) {
	return ((RecordIndexEl*) word)->root->key;
}

//  createRecordIndex -- Create a record index. A record index is a hash table with its functions
//    set to handle record index elements.
//--------------------------------------------------------------------------------------------------
RecordIndex *createRecordIndex(void)
{
	return createHashTable(compareRecordIndexEls, deleteRecordIndexEl, recordIndexElKey);
}

//  deleteRecordIndex -- Delete a record index. Delegate to the hash table.
//--------------------------------------------------------------------------------------------------
void deleteRecordIndex(RecordIndex *index)
{
	deleteHashTable(index);
}

//  insertInRecordIndex -- Add a (key, root) element to a record index. This function saves a
//    copy of the key in the table. When elements are deleted the key strings should also be.
//    TODO: Shouldn't this function create an element and then delegate to the hash table.
//--------------------------------------------------------------------------------------------------
static int recordInsertCount = 0;  //  Used for debugging.
void insertInRecordIndex(RecordIndex *index, String key, GNode* root, int lineNumber)
//  index -- Record index to add the (key, root) entry to.
//  key -- Key (minus @-signs) of a Gedcom node record.
//  root -- Root of the Gedom record.
//  lineNumber -- Line number where the record was found in the Gedcom file.
{
	recordInsertCount++;  //  Debugging.
	ASSERT(index && key && root);
	//  Hash the record key to get the bucket. If the bucket doesn't exist, create it.
	int hash = getHash(key);
	Bucket *bucket = index->buckets[hash];
	if (!bucket) {
		bucket = createBucket();
		index->buckets[hash] = bucket;
	}

	// See if there is an element for the key. If not create one and insert it.
	RecordIndexEl* element = searchBucket(bucket, key, index->compare,  index->getKey, null);
	// TODO. Note the index pointer is null above. If we used it, we could insert faster below.
	if (!element) {
		element = (RecordIndexEl*) stdalloc(sizeof(RecordIndexEl));
		//element->key = strsave(key);
		element->root = root; // MNOTE: Not copied, records persist.
		element->lineNumber = lineNumber;
		appendToBucket(bucket, element);
	} //else {
		//printf("The element exists\n");  //  Debugging.
	//}
	// TODO: Should there be a warning if the element exists?
	// TODO: Should it be an error if the element exists?
}



//  getRecordInsertCount -- Return the record insert count. For debugging.
//--------------------------------------------------------------------------------------------------
int getRecordInsertCount(void)
{
	return recordInsertCount;
}

//  searchRecordIndex -- Search a record index for a key, and return the associated node tree.
//--------------------------------------------------------------------------------------------------
GNode* searchRecordIndex(RecordIndex *index, String key)
// index -- Record index to search for the key in.
// key -- Key of record to search for.
{
	ASSERT(index && key);
	RecordIndexEl* element = searchHashTable(index, key);
	return element == null ? null : element->root;
}

// showRecordIndex -- Show the contents of a RecordIndex. For debugging.
//--------------------------------------------------------------------------------------------------
void showRecordIndex(RecordIndex *index)
{
	//  Loop through the buckets.
	for (int i = 0; i < MAX_HASH; i++) {

		//  Ignore empty buckets.
		if (!index->buckets[i]) continue;

		Bucket *bucket = index->buckets[i];
		Word* elements = bucket->elements;
		printf("Bucket %d\n", i);

		// Loop through the elements in the ith Bucket.
		for (int j = 0; j < bucket->length; j++) {
			RecordIndexEl* element = elements[j];
			printf("    Key %s\n", element->root->key);
		}
	}
}
