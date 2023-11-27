# HashTable

Implements a HashTable, an array of MAX_HASH Buckets. Each Bucket is a list of elements with the same hash value. Elements are defined by the user. The user provides a compare function used to keep the elements in the Buckets sorted.
```
typedef struct HashTable {
	int (*compare)(Word, Word);
	void (*delete)(Word);
	String(*getKey)(Word);
	Bucket *buckets[MAX_HASH];
	int count;
} HashTable;
```
|Field|Description|
|:---|:---|
|compare|Function that compares two elements in a bucket.|
|delete|Function to delete an element.|
|getKey|Function to get an element's key.|
|buckets|Array of MAX_HASH buckets.|
|count|Number of elements in the table.|

HashTables consist of Buckets. Each Bucket holds an array of elements. Each Bucket holds an array of elements. The elements are treated as Words (void\* pointers). When a Bucket's size reaches the sort threshold the elements are sorted using the table's compare function.

```
typedef struct Bucket {
	int length;
	int maxLength;
	bool sorted;
	Word *elements;
} Bucket;
```
|Field|Description|
|:---|:---|
|length|Current length of this Bucket (number of elements in the Bucket).|
|maxLength|Maximum length this Bucket can be before reallocation.|
|sorted|True when this Bucket is sorted.|
|elements|Array of elements in this Bucket.|

The user interface to HashTable is:

|Component|Description|
|:---|:---|
|HashTable *createHashTable (int(\*comp)(Word, Word), void(\*del)(Word), String(\*getKey)(Word));|Create a hash table. Functions to compare, delete, and retrieve keys from elements are passed in.|
|void deleteHashTable (HashTable*)|Delete a HashTable; use the element delete function if it exists.|
|bool isInHashTable (HashTable\*, String key)|Return whether an element with the given key is in the table.|
|Word searchHashTable (HashTable\*, String key)|Return the element that matches the given key if it is in the table; otherwise return null.|
|void insertInHashTable (HashTable*, Word element)|Insert (add) an element into the table. They getKey function is called to extract the key from the element.|
|Word firstInHashTable (HashTable\*, int\*, int\*)|Return the first element in the table. Provide two integers to track the iteration state.|
|Word nextInHashTable (HashTable\*, int\*, int\*)|Return the next element in the table in the iteration defined by the two state variables.|
|int sizeHashTable (HashTable\*)|Return the number of elements in the table.|
|void showHashTable (HashTable\*, void (\*show)(Word))|Show the contents of the hash table &mdash; for debugging.||
|/\*static\*/ int getHash (String)|Return the hashed value of a String. *This function should be  static, but there is a dependency to clean up.*|
|void removeFromHashTable (HashTable\*, String key)|Remove the element with the given key from the hash table. Call the delete function on the element if present.|
|Bucket \*createBucket(void) |Create a bucket. *Should this be static?*|
|void deleteBucket (Bucket\*, void(\*)(Word))|Delete a bucket.|
|Word searchBucket (Bucket\*, String key, int(\*compare)(Word, Word), String (\*getKey)(Word), int \*index)|Search a bucket.|
|void appendToBucket (Bucket\*, Word element)|Append an element to a bucket.|
|void removeElement (HashTable\*, Word element)|Remove an element from the hash table. *How does this relate to the removeFromHashTable function?*|
|int iterateHashTableWithPredicate (HashTable\*, bool(*)(Word element))|Iterate through a hash table performing a function. *Needs a better description.*|