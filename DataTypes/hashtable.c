//
//  DeadEnds
//
//  hashtable.c -- Implements a generic hash table. More specialized hash tables are created by
//    specializing this hash table.
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 21 November 2023.
//

#include "hashtable.h"
#include "sort.h"

Word searchBucket(Bucket*, String key, int(*compare)(Word, Word), String(*getKey)(Word), int* index);
static Word linearSearchBucket(Bucket*, String key, String(*)(Word), int *index);
static Word binarySearchBucket(Bucket*, String key, String(*)(Word), int *index);
static void sortBucket(Bucket*, int(*)(Word, Word), String(*)(Word), bool force);
static void growBucket(Bucket *bucket);
static void removeFromBucketByIndex(Bucket *bucket, int index, void (*delete)(Word));

static bool debugging = false;  //  Debugging flag.

//  These three items are a tunnel to sort.c.
//  TODO: Can we make the tunnel dissapear?
extern Word* ldata;
extern int(*lcmp)(Word, Word);
extern void quickSort(int left, int right);

//  createHashTable -- Create a hash table.
//--------------------------------------------------------------------------------------------------
HashTable *createHashTable(int(*compare)(Word, Word), void(*delete)(Word), String(*getKey)(Word))
//  compare -- Compare function used when sorting.
//  delete -- Delete function used when deleting elements.
//  getKey -- Key function used to get the key of an element.
{
	ASSERT(getKey);  //  getKey is the only required function; the other two can be null.
	// Allocate space for the hash table.
	HashTable *table = (HashTable*) stdalloc(sizeof(HashTable));

	// Initialize the the table with its functions and buckets.
	table->compare = compare;
	table->delete = delete;
	table->getKey = getKey;
	for (int i = 0; i < MAX_HASH; i++) table->buckets[i] = null;
	return table;
}

//  compareElements -- Generic compare function that uses strcmp on the keys of two elements.
//--------------------------------------------------------------------------------------------------
//  TODO: This function is not called anywhere.
int compareElements(Word a, Word b, String(*getKey)(Word))
{
	return strcmp(getKey(a), getKey(b));
}

//  deleteHashTable -- Delete a hash table. Call the table's delete function on the elements.
//--------------------------------------------------------------------------------------------------
void deleteHashTable(HashTable *table)
//  table -- Hash table to delete. When this function returns the table is gone.
{
	ASSERT(table);
	for (int i = 0; i < MAX_HASH; i++) {
		if (table->buckets[i] == null) continue;  //  Nothing to delete.
		Bucket *bucket = table->buckets[i];  // ith bucket is not empty.
		if (table->delete) {
			for (int j = 0; j < bucket->length; j++) {
				table->delete(bucket->elements[j]);
			}
		}
		stdfree(bucket);
	}
	stdfree(table);
}

//  createBucket -- Create an empty bucket for a hash table.
//--------------------------------------------------------------------------------------------------
Bucket *createBucket(void)
{
	// Create a bucket to hold a list of elements.
	Bucket *bucket = (Bucket*) stdalloc(sizeof(Bucket));
	bucket->length = 0;
	bucket->maxLength = INITIAL_BUCKET_LENGTH;
	bucket->sorted = true;
	bucket->elements = (Word*) stdalloc(INITIAL_BUCKET_LENGTH*sizeof(Word));
	return bucket;
}

//  getHash -- Hash function. This function was found on the internet.
//--------------------------------------------------------------------------------------------------
int getHash(String key)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *key++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	hash = hash & 0x0000efff;
	return (int) (hash % MAX_HASH);
}

//  searchHashTable -- Search a hash table for an element with a key. Return the element if it
//    exists or null otherwise.
//--------------------------------------------------------------------------------------------------
Word searchHashTable(HashTable *table, String key)
{
	ASSERT(table && key);
	// Get the Bucket that does or would contain the key's element.
	Bucket *bucket = table->buckets[getHash(key)];
	if (!bucket) return null;
	
	//  Search that bucket for an element with the given key.
	return searchBucket(bucket, key, table->compare, table->getKey, null);
}

//  searchBucket -- Search for an element in a bucket. The element's key hashes to the bucket.
//    If the bucket has few elements a linear search is used. If the bucket has more than
//    SORT_THRESHOLD elements a binary search is used. This function is not static because
//    specialized hash tables may choose to use it.
//--------------------------------------------------------------------------------------------------
Word searchBucket(Bucket *bucket, String key, int (*compare)(Word, Word),
				  String (*getKey)(Word), int* index)
//  bucket -- Bucket of elements to search.
//  key -- Key to search for.
//  compare -- Key compare function.
//  getKey -- Function that gers the key from an element.
//  index --  If not null, is set to the index of the found element.
{
	ASSERT(bucket && key && compare && getKey);
	// Check whether to use linear search.
	if (bucket->length < SORT_THRESHOLD) {
		return linearSearchBucket(bucket, key, getKey, index);
	}
	// Otherwise sort the list and use binary search.
	if (!bucket->sorted) sortBucket(bucket, compare, getKey, true);
	return binarySearchBucket(bucket, key, getKey, index);
}

// linearSearchList -- Use linear search to look for for an element in a bucket.
//--------------------------------------------------------------------------------------------------
Word linearSearchBucket(Bucket *bucket, String key, String (*getKey)(Word), int *index)
//  bucket -- Bucket of elements to search.
//  key    -- Key to search for.
//  getKey -- Function that gets the key from an element.
//  index  -- If not null, is set to the index of the found element.
{
	ASSERT(bucket && key && getKey);
	if (index) *index = 0;
	Word *elements = bucket->elements;
	for (int i = 0; i < bucket->length; i++) {
		if (eqstr(key, getKey(elements[i]))) {
			if (index) *index = i;
			return elements[i];
		}
	}
	return null;  // The key was not found in the bucket.
}

//  binarySearchBucket -- Use binary search to look for an element in a bucket.
//--------------------------------------------------------------------------------------------------
Word binarySearchBucket(Bucket *bucket, String key, String(*getKey)(Word), int *index)
//  bucket -- Bucket of elements to search.
//  key    -- Key to search for.
//  getKey -- Function that gets the key from an element.
//  index  -- If not null, set to the index of the found element.
{
	int lo = 0;
	int hi = bucket->length - 1;
	while (lo <= hi) {
		int md = (lo + hi)/2;
		int rel = strcmp(key, getKey(bucket->elements[md]));
		if (rel < 0) hi = --md;
		else if (rel > 0) lo = ++md;
		else {
			if (index) *index = md;
			return bucket->elements[md];
		}
	}
	// If the element isn't in the list, set index to where it would be.
	if (index) *index = lo;
	return null;
}

//  sortBucket -- Sort a bucket. If force is true sort the bucket regardless of its length.
//    If force is false, sort the bucket only if its length is above the sort threshold.
//--------------------------------------------------------------------------------------------------
void sortBucket(Bucket *bucket, int(*compare)(Word, Word), String(*getKey)(Word), bool force)
//  bucket -- Bucket to sort.
//  force  -- If true always sort; else sort if at or above threshold.
{
	ASSERT(bucket && getKey);
	if (debugging) {
		printf("sortBucket: start: bucket of length %d\n", bucket->length);
		printf("  and the elements being sorted are:\n");
		for (int i = 0; i < bucket->length; i++) {
			printf("    %s\n", getKey(bucket->elements[i]));
		}
	}
	if (bucket->sorted) return;
	if (!force && bucket->length < SORT_THRESHOLD) return;
	if (debugging) printf("sortBucket: bucket is being sorted.\n");
	ldata = bucket->elements;
	lcmp = compare;
	quickSort(0, bucket->length - 1);
	bucket->sorted = true;
	if (debugging) {
		printf("sortBucket: end: bucket of length %d\n", bucket->length);
		printf("  and the elements are now:\n");
		for (int i = 0; i < bucket->length; i++) {
			printf("    %s\n", getKey(bucket->elements[i]));
		}
	}
}

//  isInHashTable -- Check whether an element with a given key is in the hash table.
//    The key is extracted from the element argument.
//--------------------------------------------------------------------------------------------------
bool isInHashTable(HashTable *table, String key)
{
	ASSERT(table && key);
	return searchHashTable(table, key) != null;
}

//  insertInHashTable -- Insert a new element into a HashTable. There is no string key argument
//    because the key is encoded within the element.
//--------------------------------------------------------------------------------------------------
void insertInHashTable(HashTable *table, Word element)
//  table -- Hash table to all the element to.
//  element -- Element to add to the hash table.
{
	ASSERT(table && element);
	String key = table->getKey(element);
	int hash = getHash(key);
	Bucket *bucket = table->buckets[hash];
	if (!bucket) {
		bucket = createBucket();
		table->buckets[hash] = bucket;
	}

	//  Append the element to the end of the bucket.
	//  NOTE: Not checking for duplicates.
	appendToBucket(bucket, element);
	table->count += 1;

	// If the length of the bucket is less than the sort threshold return.
	if (bucket->length < SORT_THRESHOLD) return;

	// The bucket has reached the sort threshold. Sort it.
	sortBucket(bucket, table->compare, table->getKey, false);
}

//  removeFromHashTable -- Remove an element with a specific key from a hash table.
//--------------------------------------------------------------------------------------------------
void removeFromHashTable(HashTable *table, String key)
{
	// Hash the key to find the bucket with the element.
	int hash = getHash(key);
	Bucket *bucket = table->buckets[hash];

	//  Find the element to be removed and remove it.
	//  NOTE: IF THE SIZE OF THE BUCKET IS GREATER THAN SORT_THRESHOLD AND IS SORTED WE
	//    SHOULD BE USING A BINARY SEARCH HERE.
	for (int i = 0; i < bucket->length; i++) {
		if (eqstr(key, table->getKey(bucket->elements[i]))) {
			removeFromBucketByIndex(bucket, i, table->delete);
			break;
		}
	}
}

//  Remove an element at a specific index from a bucket.
//--------------------------------------------------------------------------------------------------
static void removeFromBucketByIndex(Bucket *bucket, int index, void (*delete)(Word))
{
	ASSERT(bucket && index >= 0 && index < bucket->length);
	if (delete) (*delete)(bucket->elements[index]);
	for (int i = index; i < bucket->length - 1; i++) {
		bucket->elements[i] = bucket->elements[i + 1];
	}
	bucket->length = bucket->length - 1;
}

//  appendToBucket -- Add a new element to the end of a bucket.
//--------------------------------------------------------------------------------------------------
void appendToBucket(Bucket *bucket, Word element)
{
	ASSERT(bucket && element);
	bucket->sorted = false;
	if (bucket->length >= bucket->maxLength) growBucket(bucket);
	bucket->elements[(bucket->length)++] = element;
}

//  removeElement -- remove an element from a hash table. This does not use binary search
//    in cases where it could be used.
//    TODO: GET BINARY SEARCH WORKING IF LENGTH IS OVER THRESHHOLD.
//--------------------------------------------------------------------------------------------------
void removeElement(HashTable* table, Word element)
{
	String key = table->getKey(element);
	Bucket *bucket = table->buckets[getHash(key)];
	Word *elements = bucket->elements;
	int i = 0;
	for (; i < bucket->length; i++) {
		String check = table->getKey(elements[i]);
		if (eqstr(key, check)) break;
	}
	if (table->delete) table->delete(elements[i]);
	for (; i < bucket->length - 1; i++)
		elements[i] = elements[i+1];
	bucket->length--;
}

//  growBucket -- Private function that grows the elements block of a Bucket when required.
//--------------------------------------------------------------------------------------------------
static void growBucket(Bucket *bucket)
{
	int newLength = bucket->maxLength = (3*bucket->maxLength)/2;
	Word newElements = stdalloc(newLength*sizeof(Word));
	memcpy(newElements, bucket->elements, (bucket->length)*sizeof(Word));
	stdfree(bucket->elements);
	bucket->elements = newElements;
}

//  sizeHashTable -- Return the size (number of elements) in a hash table.
//--------------------------------------------------------------------------------------------------
int sizeHashTable(HashTable *table)
//  table -- Hash table to return the size of.
{
	ASSERT(table);
	int length = 0;
	for (int i = 0; i < MAX_HASH; i++)
		if (table->buckets[i]) length += table->buckets[i]->length;
	return length;
}

//  firstInHashTable -- Returns the first element in a hash table. Works with nextInHashTable
//    to iterate the table, returning each element in turn. The (in, out) variables keep track
//    of the iteration state. The user must provide two stack variables to hold the state.
//--------------------------------------------------------------------------------------------------
Word firstInHashTable(HashTable *table, int *bucketIndex, int *elementIndex)
//  table -- Hash table to iterate over.
//  bucketIndex -- (in, out) Index of the current bucket.
//  elementIndex -- (in, out) Index of the current element in current bucket.
{
	for (int i = 0; i < MAX_HASH; i++) {
		Bucket *bucket = table->buckets[i];  // Bucket is a list of elements.
		if (bucket == null) continue;  //  Bucket has nothing in it.
		//  Found the first bucket with contents.
		*bucketIndex = i;
		*elementIndex = 0;
		return bucket->elements[0];  // Returns first element in first bucket with contents.
	}
	return null;  //  Returns null because the table is empty.
}

//  nextInHashTable -- Returns the next element in the hash table, using the (in,out) state
//    variables to keep track of the state of the iteration.
//-------------------------------------------------------------------------------------------------
Word nextInHashTable(HashTable *table, int *bucketIndex, int *elementIndex)
//  table -- Hash table being iterated.
//  bucketIndex -- (in,out) Index of the current bucket.
//  elementIndex -- (in,out) Index of the current element in current bucket.
{
	ASSERT(table);
	// See if there is another element in the current bucket.
	Bucket *bucket = table->buckets[*bucketIndex];
	if (*elementIndex < bucket->length - 1) {
		*elementIndex += 1;
		return bucket->elements[*elementIndex];
	}
	// Reached the end of the current Bucket. Find the next Bucket with elements.
	for (int i = *bucketIndex + 1; i < MAX_HASH; i++) {
		bucket = table->buckets[i];
		if (bucket == null) continue;  // Bucket has nothing in it.
		// Found another Bucket with elements.
		*bucketIndex = i;
		*elementIndex = 0;
		return bucket->elements[0];  // Returns first element in next bucket with elements.
	}
	return null;  // Reached the end of the table; no more elements to return.
}

//  iterateHashTable -- Iterate a hash table and perform a function on each element.  The
//    elements are visited in hash key order. Returns the number of elements that match the
//    predicates.
//--------------------------------------------------------------------------------------------------
int iterateHashTableWithPredicate(HashTable *table, bool (*predicate)(Word))
{
	int bucketIndex, elementIndex;
	int count = 0;
	Word element = firstInHashTable(table, &bucketIndex, &elementIndex);
	while (element) {
		if ((*predicate)(element)) count++;
		element = nextInHashTable(table, &bucketIndex, &elementIndex);
	}
	return count;
}

//  showHashTable -- Show the contents of a hash table. Intended for debugging.
//--------------------------------------------------------------------------------------------------
void showHashTable (HashTable *table, void (*show)(Word))
{
	int count = 0;
	FORHASHTABLE(table, element)
		(*show)(element);
		count++;
		printf("\n");
	ENDHASHTABLE
	printf("showHashTable showed %d elements\n", count);
}

//  This file also implements some more specific hash tables;

String wordGetKey(Word element) { return ((WordElement*) element)->key; }
