//
//  DeadEnds Project
//
//  refnindex.c -- Handle user reference indexing.
//
//  Gedcom records can have 1 REFN nodes whose values give records unique identifiers. A record
//  can have only one REFN value. Other records cannot reference other records using those records'
//  REFN values. LifeLines allowed records to have any number of REFN nodes, and it also allowed
//  links to be made to records with REFN values. Each REFN value in a database must be unique.
//
//  Created by Thomas Wetmore on 16 December 2023.
//  Last changed on 1 January 2024.
//

#include "refnindex.h"
#include "gedcom.h"

//  searchRefnIndex -- Search a RefnIndex for a reference (REFN) value.
//--------------------------------------------------------------------------------------------------

String searchRefnIndex(RefnIndex *index, String refn)
{
	RefnIndexEl *el = (RefnIndexEl*) searchHashTable(index, refn);
	return el ? el->key : null;
}

//  createRefnIndexEl -- Create a new reference index entry.
//-------------------------------------------------------------------------------------------------
RefnIndexEl *createRefnIndexEl(String refn, String key)
{
	RefnIndexEl *el = (RefnIndexEl*) stdalloc(sizeof(RefnIndexEl));
	el->refn = refn;
	el->key = key;
	return el;
}

//  showRefnIndex -- Show a RefnIndex, for debugging.
//--------------------------------------------------------------------------------------------------
void showRefnIndex(RefnIndex *index)
{
	printf("showRefnIndex: Write me\n");
}

//  cmpRefnIndexEls -- Compare function for RefnIndexEls.
//--------------------------------------------------------------------------------------------------
static int cmpRefnIndexEls (Word a, Word b)
{
	return strcmp(((RefnIndexEl*) a)->key, ((RefnIndexEl*) b)->key);
}

//  getRefnIndexElKey -- Get key function for RefnIndexEls.
//--------------------------------------------------------------------------------------------------
static String getRefnIndexElKey (Word a)
{
	return ((RefnIndexEl*) a)->key;
}

//  createRefnIndex -- Create a RefnIndex.
//--------------------------------------------------------------------------------------------------
RefnIndex *createRefnIndex(void)
{
	return (RefnIndex*) createHashTable(cmpRefnIndexEls, null, getRefnIndexElKey);
}

//  deleteRefnIndex -- Delete a RefnIndex.
//--------------------------------------------------------------------------------------------------
void deleteRefnIndex (RefnIndex *index)
{
	deleteHashTable(index);
}

//  insertInRefnIndex -- Insert a new reference entry in the RefnIndex. Returns true if the the
//    reference value was not already in the index. Returns false of the reference value is
//    already in the index.
//-------------------------------------------------------------------------------------------------
bool insertInRefnIndex (RefnIndex *index, String refn, String key)
{
	//  Get the bucket index and create a bucket if it does not exist.
	int hash = getHash(refn);
	Bucket *bucket = index->buckets[hash];
	if (!bucket) {
		bucket = createBucket();
		index->buckets[hash] = bucket;
	}
	//  See if the reference (REFN) value is already in the index.
	if (searchRefnIndex(index, refn)) return false;
	insertInHashTable(index, createRefnIndexEl(refn, key));
	return true;
}
