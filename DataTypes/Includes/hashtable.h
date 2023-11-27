//
//  DeadEnds
//
//  hashtable.h -- Implements a HashTable. A HashTable is an array of MAX_HASH buckets. A bucket
//    is a list of elements with the same hash value. Elements are defined by the user. The user
//    must provide a compare function used to keep the elements in the buckets sorted.
//
//  Created by Thomas Wetmore 29 November 2022.
//  Last changed on 20 October 2023.
//

#ifndef hashtable_h
#define hashtable_h

#include "standard.h"

#define MAX_HASH 1024
//#define MAX_HASH 16  // DBUG: Make MAXHASH small for debugging.
#define INITIAL_BUCKET_LENGTH 30
//#define INITIAL_BUCKET_LENGTH 4  // DBUG: Make the initial bucket length small for debugging.
#define SORT_THRESHOLD 30
//#define SORT_THRESHOLD 4  //  DBUG: Make the sort threshold small to debug the quick sort.

//  Bucket -- Hash tables consist of MAXHASH buckets. Each bucket holds an array of elements.
//    The elements are treated as void* pointers. When a bucket's size reaches the sort
//    threshold, the elements are sorted using the hash table's compare function.
//--------------------------------------------------------------------------------------------------
typedef struct Bucket {
	int length;      //  Current size of this bucket.
	int maxLength;   //  Maximum size this bucket can be before reallocation.
	bool sorted;     //  True when this bucket is sorted.
	Word *elements;  //  The elements in this bucket.
} Bucket;

//  HashTable -- Hash table.
//--------------------------------------------------------------------------------------------------
typedef struct HashTable {
	int (*compare)(Word, Word);  //  Compare two elements in a bucket.
	void (*delete)(Word);        //  Function to delete an element.
	String(*getKey)(Word);       //  Function to get the key from an element.
	Bucket *buckets[MAX_HASH];   //  Buckets of the hash table.
	int count;                   //  The number of elements in the table.
} HashTable;

// User interface to hash table.
//--------------------------------------------------------------------------------------------------
HashTable *createHashTable(int (*compare)(Word, Word), void(*delete)(Word), String(*getKey)(Word));
void deleteHashTable(HashTable*);  // Delete a HashTable; use the element delete function if exists.
bool isInHashTable(HashTable*, String key);  //  Return whether an element with key is in.
Word searchHashTable(HashTable*, String key);  // Return the element that matches the key.

void insertInHashTable(HashTable*, Word element);  // Add a new element to the table.
Word firstInHashTable(HashTable*, int*, int*);  // Return first element in a new iteration.
Word nextInHashTable(HashTable*, int*, int*);  // Return next table element in iteration.
int sizeHashTable(HashTable*);  // Return the number of elements in a table.
void showHashTable(HashTable*, void (*show)(Word));  // Show the contents of a table; for debugging.
/*static*/ int getHash(String);  // Return the hashed value of a String.
void removeFromHashTable(HashTable*, String key);
int iterateHashTableWithPredicate(HashTable*, bool (*function)(Word element));

//  SHOULDN'T THE BUCKET FUNCTIONS BE STATIC, SO NOT DECLARED IN HERE AT ALL??
Bucket *createBucket(void);  // Create a bucket.
void deleteBucket(Bucket*, void(*)(Word));  // Delete a bucket.
Word searchBucket(Bucket*, String key, int(*compare)(Word, Word), String (*getKey)(Word), int* index);  // Search a bucket.

void appendToBucket(Bucket*, Word element);  // Append an element to a bucket.
void removeElement(HashTable*, Word element);  // Remove an element from the hash table.

//  The rest of this file defines the interfaces to hash tables with specific element types.
#define IntegerTable HashTable      //  Hash table whose element values are integers.
//#define FunctionTable HashTable     //  Hash table whose element values are function definitions.
#define WordTable HashTable         //  Hash table whose element values are arbitrary void*'s.

//  WordElement -- Elements for wholly generic hash tables. Casts needed throughout.
//--------------------------------------------------------------------------------------------------
typedef struct WordElement {
	String key;
	Word value;
} WordElement;

//  Macros for iterating over all elements in a hash table. Meanings of the brackets: outside
//    pair of brackets enclose the full macro expansion; the middle pair of brackets enclose the
//    internal while loop; and the inner pair of brackets enclose what the user provides as the
//    loop body.
//-------------------------------------------------------------------------------------------------
#define FORHASHTABLE(table, element) {\
			int __i = 0, __j = 0;\
			HashTable *__table = table;\
			Word element = firstInHashTable(__table, &__i, &__j);\
			while (element) {{\
			
#define ENDHASHTABLE }\
			element = nextInHashTable(__table, &__i, &__j);\
		}}\

#endif // hashtable_h
