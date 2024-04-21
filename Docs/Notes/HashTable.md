# Hash Tables

## What Should the User Interface to a Hash Table Be?

Let's get started by defining a HashTable and mentioning current and possible interfaces. This discussion is motivated by observations that the current interface implementation may have performance issues based on the fact that the _getHash_ function is getting called more often than seems it should.

The purpose of the DeadEnds _HashTable_ is conventional &mdash; an efficient string to value map.

#### Creating

The main feature of the DeadEnds _HashTable_ type is its ability to hold elements of different types. Its elements are pointers to structures, and all elements in a specific HashTable must point to instances of the same structure type. In addition elements must have String keys, usually implemented as a field in the structure, though the key can also be computable from other fields in the structure. Therefore elements in HashTables are (pointers to) structures that include a real or computable key and a value also encoded in the fields of the structure. The HashTable doesn't need to know the details of the structure &mdash; when created the caller must supply three functions that allow the HashTable to handle the elements. First is the _String getKey(void\* element)_ function that returns the key of an element. Second is the _int compare(String, String)_ function that compare two element keys. Finally is the _void delete(void* element)_ function that frees and element when it is removed from the HashTable. Why is a compare function needed? Because a Buckets might be sorted. What does that mean?

HashTable store their elements in Buckets. Currently all HashTables have 1024 Buckets, though this is an easily changed parameter. A minor change to HashTable would allow each specific HashTable to have its own numbers of Buckets.

A Bucket is a list of elements, a list of pointers to the structures used by a specific HashTable. A Bucket is implemented using the DeadEnds _Block_ data type. _HashTable_ has a feature where the list of elements in a bucket is left unsorted (sortedness is defined by the _getKey_ and _compare_ functions) until a sort threshold is reached and then sorted thereafter. The threshold is currently set to 30 for all HashTables, though another simple change would allow each specific HashTable to have its own sort threshold.

#### Creating

HashTables are created by the _createHashTable(String(*getKey)(void\*), int(*\*compare(String, String), void(\*delete)(void\*))_ function. The caller must provide the _getKey, compare, and delete_ functions.

#### Deleting

HashTables are deleted using the _deleteHashTable()_ function. This uses the element delete function _delete_ to remove all elements, and then deletes all buckets and then the HashTable structure itself.

#### Searching

Searching is a key function. At the basic level the purpose of searching is to find out whetehr an element with a given key is in the table. At this level some options exist. For example, the user might want to know only of and element with given key is in the table, so an interface like _bool isInHashTable(HashTable\*, String key)_ might be sufficient. However, if this operation is to be followed by another operation, say inserting an element if the key isn't there, or getting the element if it is there, or changing the element to a new value if it is there, then actions already taken by searching will have to be redone by an insertion operation:

_bool isInHashTable(HashTable\* table, String key)_ &mdash; looks for an element with given key and returns whether found. A user level function.

Other functions need to know if an element is there as a first step; we could keep info about where the element is (or would be). The current implementation calls _searchHashTable(HashTable\*, String)_.

I added the function _static void* detailSearch(HashTable\* table, String key, int\* phash, int\* pindex)_. It searches for an element. If found it is returned. If not found null is returned. Regardless phash points to the key's hash value, and pindex points to the index of the element in the Bucket, if there, or where it would be in a sorted Bucket.

_void* searchHashTable(HashTable\* table, String key)_ or _getElementFromHashTable(HashTable\* table, String key)_ &mdash; searches for element with key and returns it if there and _null_ if not.

void* searchHashTableWithElement(HashTable\* table, void\* element) &mdash;Like previous, but passes an element rather than a key. Needed?

_void addToHashTable(HashTable\* table, void\* element)_ &mdash; what if element already there? Replace? Don't add?

void updateHashTable &mdash; If element not in table add it; if element is in the table replace it.

void addToHashTableIfNew &mdash; If element not in table add it; if element in in symbol table do nothing.

For more fine tuned access (see the SymbolTable) when searching returns an Element, the caller, knowing the structure of the element, can do anything to the value of the element.

#### Inserting





## Functions on Hash Tables

HashTable provides a hash table type. It uses void* elements to hold data. Using HashTable requires a structure definition for holding elements. Elements must have String keys to be identifiable so there is usually a String field in the structure that holds the key. When a HashTable is created three fuctions can be assigned to it:

```c
void (*delete)(void* element);
int (*compare)(void* elementA, void* elementB);
String (*getKey)(void* element);
```

The delete function frees the memorty of an element; compare returns he order relationship between two elements, and getKey returns the string key of an element. In the interfaces below, the functions are indicated by their names, not their signatures.

There is type of key that I don't want ordered by simple alphabetic rules. It is the record key used to identify Gedcom records, which has the form of @...@, that is, two @-signs surrounding characters that can contain letters and numbers and a few others. For these I want numeric keys to order numerically, not alphabetically. Without special cases like this the compare function can be based on calling the getKey on two elements and comparing them with strcmp. For the record key case this doesn't work. 

Here is the HashTable structure:

```c
typedef struct HashTable {
	int (*compare)(void*, void*); // Compare two elements.
	void (*delete)(void*);        // Delete an element.
	String(*getKey)(void*);       // Get key of element.
	void* (*createEl)(String);    // Create an element with key. // MAY NOT STAND THE TEST OF TIME.
	Bucket *buckets[MAX_HASH];
} HashTable;
```

The structure holds the three functions and an array of Buckets, which are described below.

Here are the HashTable "methods".

```c
HashTable* createHashTable(compare, delete, getKey)
void deleteHashTable(HashTable*)
int getHash(String)
void* searchHashTable(HashTable*, String key, compare, int* index)
void* linearSearchBucket(Bucket*, String key, compare, int* index)
void* binarySearchBucket(Bucket*, String key, compare, int* index)
bool isInHashTable(Bucket*, String key)
void insertInHashTable(HashTable, void* element)
void removeFromHashTable(HashTable*, String key)
void removeElement(HashTable*, void* element)
int sizeHashTable(HashTable*)
void* firstInHashTable(HashTable*, int*. int*)
void* nextInHashTable(HashTable*, int*, int*)
void iterateHashTableWithPredicate(HashTable*, predicate)
void showHashTable(HashTable*, show)
```

Here is the Bucket structure:

```c
typedef struct Bucket {
	Block block;
	bool sorted;
} Bucket;
```

A Bucket is a list of Elements, all with the same hash key. When a Bucket passes a size threshold it is kept sorted for faster searching. When sorted the boolean sorted flag is true.

The elements themselves are kept in a Block, a data type used by HashTable and other data types to hold elements. It is also described below.

The Bucket "methods" and functions are:

```c
Bucket* createBucket(void)
void* searchBucket(HashTable*, String key, compare, int* index)
void sortBucket(Bucket*, compare)
void appendToBucket(Bucket*, void* element)
```

### Block

Let's stick the Block type in here as well. The Block structure is:

```c
typedef struct Block {
	int length;
	int maxLength;
	void** elements;
} Block;
```

The Block's methods and functions are:

```c
Block *createBlock(void);
void initBlock(Block*);
void growBlock(Block*);
void deleteBlock(Block*, void(*d)(void*));
int lengthBlock(Block*);
void emptyBlock(Block*, void(*d)(void*));
bool isEmptyBlock(Block*);

bool isKeyInBlock(Block*, String, String(*getKey)(void*));
bool isKeyInSortedBlock(Block*, String, String(*getKey)(void*));
bool isInBlock(Block*, void*, String(*getKey)(void*));
bool isInSortedBlock(Block*, void*, int(*compare)(void*, void*));

void prependToBlock(Block*, void*);
void appendToBlock(Block*, void*);
void insertInBlock(Block*, void*, int);
bool removeFromBlock(Block*, int, void(*delete)(void*));
bool removeFromSortedBlock(Block*, String, String(*getKey)(void *a), void(*delete)(void*));
bool removeFromUnsortedBlock(Block*, String, String(*getKey)(void *a), void(*delete)(void*));
bool removeFirstBlockElement(Block*, void(*delete)(void*));
bool removeLastBlockElement(Block*, void(*delete)(void*));
void sortBlock(Block*, int(*compare)(void*, void*));
bool isSorted(Block*, String(*getKey)(void*));

void uniqueBlock(Block*, int(*compare)(void *a, void *b), String(*getKey)(void*));
Block *copyBlock(Block*, void*(*copy)(void*));
void iterateBlock(Block*, void(*perform)(void*));

void* searchBlock(Block* block, void* element, String(*getKey)(void*), int*);

// Debugging only.
void printBlock(Block*, String(*getString)(void*));
```

