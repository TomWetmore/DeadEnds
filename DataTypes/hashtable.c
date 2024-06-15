// DeadEnds
//
// hashtable.c implements a general hash table. Specialized hash tables are created through
// customization.
//
// Created by Thomas Wetmore on 29 November 2022.
// Last changed on 25 April 2024.

#include "standard.h"
#include "hashtable.h"
#include "sort.h"

bool debugging = true;
bool debuggingHash = false;
bool sortChecking = false;
extern FILE* debugFile;

static void* searchBucket(Bucket*, String key, String(*g)(void*), int(*c)(String, String), int* index);
//static void* linearSearchBucket(Bucket*, String key, String(*g)(void*), int* index);
//static void* binarySearchBucket(Bucket*, String key, String(*g)(void*), int(*c)(String, String), int* index);
//static void sortBucket(Bucket*, String(*g)(void*), int(*c)(String, String));
//static void removeFromBucketByIndex(Bucket* bucket, int index, void (*delete)(void*));

// createHashTable creates and returns a HashTable. getKey is a function that returns the key of
// an element, and delete is an optional function that frees an element.
HashTable* createHashTable(String(*getKey)(void*), int(*compare)(String, String),
						   void(*delete)(void*), int numBuckets) { PH;
	HashTable *table = (HashTable*) malloc(sizeof(HashTable));
	table->compare = compare;
	table->delete = delete;
	table->getKey = getKey;
	table->numBuckets = numBuckets;
	table->buckets = (Bucket**) malloc(numBuckets*sizeof(Bucket));
	for (int i = 0; i < table->numBuckets; i++) table->buckets[i] = null;
	return table;
}

// deleteHashTable deletes a HashTable. If there is a delete function it is called on the elements.
void deleteHashTable(HashTable *table) { //PH;
	for (int i = 0; i < table->numBuckets; i++) {
		if (table->buckets[i] == null) continue;
		deleteBucket(table->buckets[i], table->delete);
	}
	free(table);
}

// createBucket creates and returns an empty Bucket.
Bucket *createBucket(void) { //PH;
	Bucket *bucket = (Bucket*) malloc(sizeof(Bucket));
	initBlock(&(bucket->block));
	return bucket;
}

// lengthBucket returns the length of a Bucket.
int lengthBucket(Bucket* bucket) {
	return lengthBlock(&(bucket->block));
}

// deleteBucket deletes a Bucket. If there is a delete function the elements are deleted.
void deleteBucket(Bucket* bucket, void(*delete)(void*)) { //PH;
	if (delete) {
		Block* block = &(bucket->block);
		for (int j = 0; j < block->length; j++) {
			delete(block->elements[j]);
		}
	}
	free(bucket);
}

// getHash returns the hash code of a Strings; found on the internet.
int getHash(String key, int maxHash) { //PH;
	unsigned long hash = 5381;
	int c;
	String p = key;
	while ((c = *p++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	hash = hash & 0x0000efff;
	hash = hash % maxHash;
	return (int) hash;
}

// detailSearch is a static function at the bottom of HashTable's search stack.
// NOTE: It may be better to return the Bucket rather than the hash.
static void* detailSearch(HashTable* table, String key, int* phash, int* pindex) { //PH;
	int hash = getHash(key, table->numBuckets);
	if (phash) *phash = hash;
	Bucket* bucket = table->buckets[hash];
	if (!bucket) { // Bucket doesn't exist.
		if (pindex) *pindex = 0;
		return null;
	}
	//printf("detailSearch calling searchBucket: hash %d; length %d, key: %s\n", hash, lengthBucket(bucket), key); // DEBUG
	return searchBucket(bucket, key, table->getKey, table->compare, pindex); // Bucket exists.
}

// searchHashTable searches a HashTable for the element with given key. It returns the element
// if found or null otherwise.
void* searchHashTable(HashTable* table, String key) { //PH;
	return detailSearch(table, key, null, null);
}

// searchHashTableWithElement searches a HashTable for the element with the same key as the given
// element.
void* searchHashTableWithElement(HashTable* table, void* element) {
	return detailSearch(table, table->getKey(element), null, null);
}

// searchBucket searches a Bucket for an element by key. Depending on Bucket size either linear or
// binary search is used.
void* searchBucket(Bucket* bucket, String key, String(*getKey)(void*),
				   int(*compare)(String, String), int* index) { //PH;
	return searchBlock(&(bucket->block), key, getKey, index);
}

// linearSearchBucket uses linear search to look for an element in a Bucket.
//void* linearSearchBucket(Bucket* bucket, String key, String(*getKey)(void*), int* index) { PH;
	//Block* block = &(bucket->block);
	//return linearSearch(block->elements, block->length, key, getKey, index);
//}

// isInHashTable returns whether an element with the given key is in the HashTable.
bool isInHashTable(HashTable* table, String key) {
	return detailSearch(table, key, null, null) != null;
}

// addToHashTable adds a new element to a HashTable.
void addToHashTable(HashTable* table, void* element, bool replace) { //PH;
	//printf("addToHashTable called element with key %s\n", table->getKey(element)); // DEBUG
	String key = table->getKey(element);
	int hash, index;
	Bucket* bucket = null;
	bool found = detailSearch(table, key, &hash, &index);
	if (found && replace) { // Replace existing element.
		bucket = table->buckets[hash];
		setBlockElement(&(bucket->block), element, table->delete, index);
		return;
	}
	if (found) return; // Element exists, but don't replace.
	bucket = table->buckets[hash]; // Add it; be sure Bucket exists.
	if (!bucket) {
		bucket = createBucket();
		table->buckets[hash] = bucket;
	}
	appendToBlock(&(bucket->block), element);
}

bool addToHashTableIfNew(HashTable* table, void* element) { //PH;
	// See if it is there and if not where it should go if the bucket is sorted.
	// If there return false, meaning didn't add it.
	// Put the element into the right place.
	return true;
}

// removeFromHashTable removes the element with given key from a HashTable.
void removeFromHashTable(HashTable* table, String key) { //PH;
	int hash = getHash(key, table->numBuckets);
	Bucket *bucket = table->buckets[hash];
	if (!bucket) return /*false*/;
	Block *block = &(bucket->block);
	int index = 0;
	void *element = linearSearch(block->elements, block->length, key, table->getKey, &index);
	if (element) {
		ASSERT(index != -1);
		removeFromBlock(block, index, table->delete);
	}
}

// appendToBucket adds a new element to the end of a bucket.
void appendToBucket(Bucket* bucket, void* element) { //PH;
	appendToBlock(&(bucket->block), element);
}

// removeElement removes an element from a hash table. It does not use binary search in cases
// when it should.
// TODO: GET BINARY SEARCH WORKING IF LENGTH IS OVER THRESHHOLD.
void removeElement(HashTable* table, void *element) { //PH;
	String key = table->getKey(element);
	Bucket *bucket = table->buckets[getHash(key, table->numBuckets)];
	Block *block = &(bucket->block);
	void **elements = block->elements;
	int length = block->length;
	int i = 0;
	for (; i < length; i++) {
		String check = table->getKey(elements[i]);
		if (eqstr(key, check)) break;
	}
	if (i >= length) return; // without doing anything.
	if (table->delete) table->delete(elements[i]);
	for (; i < length - 1; i++)
		elements[i] = elements[i+1];
	block->length--;
}

//  sizeHashTable returns the size (number of elements) in a hash table.
int sizeHashTable(HashTable* table) { //PH;
	int length = 0;
	for (int i = 0; i < table->numBuckets; i++) {
		if (table->buckets[i]) {
			Block *block = &((table->buckets[i])->block);
			length += block->length;
		}
	}
	return length;
}

// firstInHashTable returns the first element in a hash table; it works with nextInHashTable to
// iterate the table, returning each element in turn. The (in, out) variables keep track of the
// iteration state. The user provides two stack variables to hold the state.
void* firstInHashTable(HashTable* table, int* bucketIndex, int* elementIndex) { //PH;
	for (int i = 0; i < table->numBuckets; i++) {
		Bucket* bucket = table->buckets[i];
		if (bucket == null) continue;
		*bucketIndex = i;
		*elementIndex = 0;
		Block *block = &(bucket->block);
		return block->elements[0];
	}
	return null;
}

// nextInHashTable returns the next element in the hash table, using the (in,out) state
// variables to keep track of the state of the iteration.
void* nextInHashTable(HashTable* table, int* bucketIndex, int* elementIndex) { //PH;
	Bucket* bucket = table->buckets[*bucketIndex];
	Block* block = &(bucket->block);
	if (*elementIndex < block->length - 1) {
		*elementIndex += 1;
		return block->elements[*elementIndex];
	}
	// Reached end of current Bucket; find next.
	for (int i = *bucketIndex + 1; i < table->numBuckets; i++) {
		bucket = table->buckets[i];
		if (bucket == null) continue;  // 'Empty' bucket.
		*bucketIndex = i;
		*elementIndex = 0;
		block = &(bucket->block);
		return block->elements[0];
	}
	return null; // No more elements.
}

// iterateHashTable iterates a hash table and perform a function on each element; elements are
// visited in hash key order; returns the number of elements that match the predicate.
int iterateHashTableWithPredicate(HashTable* table, bool (*predicate)(void*)) { //PH;
	int bucketIndex, elementIndex;
	int count = 0;
	void* element = firstInHashTable(table, &bucketIndex, &elementIndex);
	while (element) {
		if ((*predicate)(element)) count++;
		element = nextInHashTable(table, &bucketIndex, &elementIndex);
	}
	return count;
}

// showHashTable is a debugging function that shows the contents of a hash table.
void showHashTable(HashTable* table, void (*show)(void*)) { //PH;
	int count = 0;
	FORHASHTABLE(table, element)
		printf("%d %d ", __i, __j);
		(*show)(element);
		count++;
		//printf("\n");
	ENDHASHTABLE
	printf("showHashTable showed %d elements\n", count);
}
