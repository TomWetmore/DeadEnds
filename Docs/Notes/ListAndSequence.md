

## Functions on Blocks

```c
Block* createBlock(void)
void initBlock(Block*)
void deleteBlock(Block*, delete)
void growBlock(Block*)
void emptyBlock(Block*, delete);
int lengthBlock(Block*);
bool isEmptyBlock(Block*);
bool isKeyInBlock(Block*, String key, getKey);
bool isKeyInSortedBlock(Block*, String key, getKey);
bool isInBlock(Block*, void* element, compare);
bool isInSortedBlock*(Block*, void* element, compare);
Block* copyBlock(Block*, copyfunc);
void appendToBlock(Block*, void* element);
void prepandToBlock(Block*, void* element);
void insertInBlock(Block*, void* element, int *index);
void removeFirstBlockElement(Block*, delete);
void removeLastBlockElement(Block*, delete);
void removeFromBlock(Block*, int index, delete);
void removeFromSortedBlock(Block*, String key, getKey, *index);
void removeFromUnsortedBlock(Block*, String key, getKey, delete);
void sortBlock(Block*, compare);
void* searchBlock(Block*, void* element, getKey, int* index);
void* searchSortedBlock(Block*, String key, getKey);
void printBlock(Block*, getString);
bool isSortedBlock(Block*, getKey)
void iterateBlock(Block*, perform);


    
```



## Unifying List, Sequence and Bucket

List, Sequence and Bucket are data types used in DeadEnds. List is a general purpose list with several specializations. Sequence is a customized list for person (and sometimes family) records. Bucket is a list used as a HashTable bucket.

It would be good to unify them where possible to avoid code duplication.. Here is the List type:

```c
typedef struct List {
    bool sorted;     // True if to be sorted.
    bool isSorted;   // True when sorted.
	bool useSyslib;  // True if using syslib.qsort.
    int length;      // Current length.
    int maxLength;   // Max length before realloc.
    Word *data;      // List data.
    int (*compare)(const Word, const Word);  // Compare two elements.
    void (*delete)(Word);    // Delete element.
    String (*getKey)(Word);  // Get key of element.
} List;
```

Each List specialization defines its own element type. Sequence can be thought of as a fully speciaized list with a custom implementation.

The element type of Sequence:

```c
typedef struct SequenceEl {
	String key;     // Person or family key. GNode instead?
	String name;    // Name of person.
	PValue *value;  // Arbitrary program value.
}
*SequenceEl;
```

SequenceEl identifies a Gedcom record key. Using root GNodes might be a better implementation. The name field is an optimization for uses that process names. Sequences can be used for records other than persons, where the name field does not apply. Each element can have a PValue* field for uses where an additional value associated with each element comes in handy.

Here is the Sequence type. Flags is a bit field that tracks a Sequence's state. For example, a Sequence can be sorted on one of three ways, by key, by name, or by the alternative value. The flags field has bits that are set when the Sequence is sorted in one of those three ways. Also, duplicate entries can be removed from sequences, and flags indicate whether or not the Sequenced has been "uniqued."

```c
typedef struct Sequence Sequence;
struct Sequence {
	int size;          // Current length of sequence.
	int max;           // Max length before realloc.
	int flags;         // Attributes.
	SequenceEl *data;  // Elements of the sequence
	Database *database;  // Database sequence is from.
};
```

Finally the Bucket datatype is nearly identical to the List datatype with the three functions removed. Here is the Bucket type:

`````c
typedef struct Bucket {
	int length;      //  Current size of bucket.
	int maxLength;   //  Maximum size bucket before realloc.
	bool sorted;     //  True when bucket is sorted.
	Word *elements;  //  Elements in bucket.
} Bucket;
`````

A HashTable is primarly composed of Buckets. The HashTable type is:

```c
typedef struct HashTable {
	int (*compare)(Word, Word);  //  Compare two elements.
	void (*delete)(Word);        //  Delete an element.
	String(*getKey)(Word);       //  Get element key.
	Bucket *buckets[MAX_HASH];   //  The Buckets.
	int count;                   //  Number of elements in table.
} HashTable;
```

### Unification Ideas

1. All three grow their lists when max length is reached.

2. Interfaces to sorting.

3. Interfaces to searching.

4. Consistent naming of fields (e.g., elements instead of data.)

### Growing

   Here is the growList function for List:

```c
static void growList(List *list)
{
	int newLength = list->maxLength = (3*list->maxLength)/2;
	Word newData = stdalloc(newLength*sizeof(Word));
	memcpy(newData, list->data, (list->length)*sizeof(Word));
	stdfree(list->data);
	list->data = newData;
}
```

For Sequence the element array is grown inside the function appendSequenceElement:

```c
	if ((n = sequence->size) >= sequence->max)  {
		int m = 3*n;
		SequenceEl* new = (SequenceEl*) stdalloc(m*sizeof(SequenceEl));
		for (int i = 0; i < n; i++)
			new[i] = old[i];
		stdfree(old);
		IData(sequence) = old = new;
		sequence->max = m;
	}
```

And for Buckets:

```c
static void growBucket(Bucket *bucket)
{
	int newLength = bucket->maxLength = (3*bucket->maxLength)/2;
	Word newElements = stdalloc(newLength*sizeof(Word));
	memcpy(newElements, bucket->elements, (bucket->length)*sizeof(Word));
	stdfree(bucket->elements);
	bucket->elements = newElements;
}
```

### Sorting Interfaces

#### List

List sorting is in flux. I've been experimenting with using the system's qsort rather than the DeadEnds quick sort. The compare functions used by the two versions differ by a level of indirection. The system qsort needs the addresses of the elements to be compared. My function needs the values of elements, so there are two levels of indirection in the qsort approach and only one in my approach. I plan to return to my own. So the qsort approach isn't mentioned further.

```c
void quickSortList(List *list)
{
	ldata = list->data;
	lcmp = list->compare;
	quickSort(0, list->length - 1);
}
```



#### Sequence

#### Bucket

```c
void sortBucket(Bucket *bucket, int(*compare)(Word, Word), String(*getKey)(Word), bool force)
{
	if (bucket->sorted) return;
	if (!force && bucket->length < SORT_THRESHOLD) return;
	ldata = bucket->elements;
	lcmp = compare;
	quickSort(0, bucket->length - 1);
	bucket->sorted = true;
}
```

### Possible Redesigns

Block &mdash; current length, maximum length, delete-element, append, prepend, linear search, gett-at-index, remove at index, grow-block, delete-block

SortedBlock &mdash; everything in Block plus compare-function, sort, search, compare-elements, insert-sorted, remove-sorted.

Sort Function Hierarchy &mdash; 

1. Low level &mdash; the elements and compare function are static and the rest of the functions just deal with indexes.
2. Top sort function &mdash; called with the list, the list's length, and the element compare function.
3. Method on SortedBlock &mdash; has the block of elements, the number of elements and the compare function so constructs the call to the top sort function.
4. Sequence approach &mdash; the sequence can be sorted different ways. So it has separate methods to perform each sort type. It constructs the proper call to the top sort function by knowing how it will be sorted.



### Changes to the Mainline

```c
Block *createBlock(void) {
	Block *block = (Block*) malloc(sizeof(Block));
	initBlock(block);
	return block;
}
```







// initBlock initializes the memeory of a newly allocated block.

void initBlock(Block *block)

{

​	block->length = 0;

​	block->maxLength = INITIAL_SIZE_LIST_DATA_BLOCK;

​	block->elements = (void*) malloc(INITIAL_SIZE_LIST_DATA_BLOCK*sizeof(void*));

}



// growBlock increases the size of a block when it reaches its current maximum size.

//--------------------------------------------------------------------------------------------------

void growBlock(Block *block)

{

​	int newLength = block->maxLength = (3*block->maxLength)/2;

​	void *newElements = malloc(newLength*sizeof(void*));

​	memcpy(newElements, block->elements, (block->length)*sizeof(void*));

​	free(block->elements);

​	block->elements = newElements;

}



// appendToBlock appends a new element to the end of a Block's array of elements.

void appendToBlock(Block *block, void *element)

{

​	if (block->length >= block->maxLength) growBlock(block);

​	block->elements[(block->length)++] = element;

}



// removeFromBlock removes the element at a specific index from a Block's element array.

void removeFromBlock(Block *block, int index, void(*delete)(void*))

{

​	ASSERT(index >= 0 && index < block->length);

​	if (index < 0 || index >= block->length) return;

​	void **elements = block->elements;

​	if (delete) delete(elements[index]);

​	for (; index < block->length - 1; index++)

​		elements[index] = elements[index + 1];

​	(block->length)--;

}



void removeFromSortedBlock(Block *block, String key, int(*getKey)(void *a), void(*delete)(void*))

{

​	int index = -1;

​	if (!binarySearchElementsByKey(block->elements, block->length, key, getKey, &index)) return;

​	removeFromBlock(block, index, delete);

}



void removeFromUnsortedBlock(Block *block, String key, String(*getKey)(void *a), void(*delete)(void*))

{

​	if (!linearSearchElementsByKey(block->elements, block->length, key, getkey, &index)) return;

​	removeFromBlock(block, index, delete);



}



// sortBlock sorts the elements in a Block.

void sortBlock(Block* block, int(*compare)(void*, void*))

{

​	sortElements(block->elements, block->length, compare);

}



void *searchSortedBlock(Block *block, String key, String(*getKey)(void*))



// printBlock is a debugging function that shows the contents of a Block.

void printBlock(Block *block, String(*getString)(void*))

{

​	printf("Block: %d %d\n", (int) block->length, (int) block->maxLength);

​	for (int i = 0; i < block->length; i++) {

​		printf("%s ", getString(block->elements[i]));

​	}

​	printf("\n");

}



// isSorted returns true if the given Block is sorted.

bool isSorted(Block *block, String(*getKey)(void*))

// block is the Block to be checked for sortedness.

// getKey is the function that returns the sort key of an element.

{

​	if (block->length <= 1) return true;

​	void **elements = block->elements;

​	String key = getKey(elements[0]);

​	for (int i = 1; i < block->length; i++) {

​		String next = getKey(elements[i]);

​		if (strcmp(key, next) > 0) return false;

​	}

​	return true;

}
