// DeadEnds
//
// hashtable.h is the header file for the HashTable data type. A HashTable is an array of buckets.
// A bucket is a list of elements with the same hash value. Elements are defined by the user.
// The user a getKey function to return the key of an element, and a compare function that is
// use to keep the elements in a Bucket sorted.
//
// Created by Thomas Wetmore 29 November 2022.
// Last changed on 7 October 2024.

#ifndef hashtable_h
#define hashtable_h

#include "standard.h"
#include "block.h"

//#define INITIAL_BUCKET_LENGTH 30
#define INITIAL_BUCKET_LENGTH 4  // DBUG: Make the initial bucket length small for debugging.
//#define SORT_THRESHOLD 30
#define SORT_THRESHOLD 5  //  DBUG: Make the sort threshold small to debug the quick sort.

// Bucket is the type of HashTable buckets.
typedef struct Bucket {
	Block block;
} Bucket;

// HashTable is the type that implements a hash table. The getKey, compare and delete functions
// customize the elements used in specific HashTables. getKey gets the key of an element;
// compare compares two keys; and delete deletes an element.
typedef struct HashTable {
	int numBuckets; // Should be a prime number.
	String (*getKey)(void*);
	int (*compare)(String, String);
	void (*delete)(void*);
	Bucket** buckets;
} HashTable;

// User interface to HashTable.
HashTable* createHashTable(String(*g)(void*), int(*c)(String, String), void(*d)(void*), int numBuckets);
void deleteHashTable(HashTable*);
bool isInHashTable(HashTable*, String key);
void* searchHashTable(HashTable*, String key);

void addToHashTable(HashTable*, void*, bool);
bool addToHashTableIfNew(HashTable*, void*); // WARNING: NOT IMPLEMENTED YET.
void *firstInHashTable(HashTable*, int*, int*);
void* nextInHashTable(HashTable*, int*, int*);

int sizeHashTable(HashTable*);
void showHashTable(HashTable*, void(*show)(void*));
/*static*/ int getHash(String, int);
void removeFromHashTable(HashTable*, String key);
int iterateHashTableWithPredicate(HashTable*, bool(*)(void*));
void appendToBucket(Bucket*, void* element);
void removeElement(HashTable*, void* element);

// FORHASHTABLE and ENDHASHTABLE iterate the elements in a HashTable.
#define FORHASHTABLE(table, element) {\
		int __i = 0, __j = 0;\
		HashTable *__table = table;\
		void *element = null;\
		void *__element = firstInHashTable(__table, &__i, &__j);\
		for(; __element; __element = nextInHashTable(__table, &__i, &__j)) {\
			element = __element;
#define ENDHASHTABLE }}

#endif // hashtable_h
