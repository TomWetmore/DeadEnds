// DeadEnds
//
// block.c holds the functions that implement the Block data type.
//
// Created by Thomas Wetmore on 9 March 2024
// Last changed on 5 May 2024.

#include "block.h"
#include "sort.h"

static bool blockDebugging = false;

// createBlock creates an empty Block.
Block* createBlock(void) {
	Block* block = (Block*) malloc(sizeof(Block));
	initBlock(block);
	return block;
}

// initBlock initializes an existing Block.
void initBlock(Block* block) {
	block->length = 0;
	block->maxLength = INITIAL_SIZE_LIST_DATA_BLOCK;
	block->elements = (void*) malloc(INITIAL_SIZE_LIST_DATA_BLOCK*sizeof(void*));
}

// deleteBlock deallocates the elements of a Block, but not the Block itself.
void deleteBlock(Block *block, void(*delete)(void*)) {
	if (delete) {
		for (int i = 0; i < block->length; i++) {
			delete((block->elements)[i]);
		}
	}
	free(block->elements);
}

// growBlock increases the size of a block when it reaches its current maximum.
static void growBlock(Block *block) {
	int newLength = block->maxLength = (3*block->maxLength)/2;
	void *newElements = malloc(newLength*sizeof(void*));
	memcpy(newElements, block->elements, (block->length)*sizeof(void*));
	free(block->elements);
	block->elements = newElements;
}

// emptyBlock removes all the elements in a Block.
void emptyBlock(Block *block, void(*delete)(void*)) {
	if (delete) {
		for (int i = 0; i < block->length; i++) {
			delete(block->elements[i]);
		}
	}
	block->length = 0;
}

// lengthBlock return the length of a Block.
int lengthBlock(Block *block) {
	return block->length;
}

// isEmptyBlock returns true if the Block is empty.
bool isEmptyBlock(Block* block) {
	ASSERT(block);
	return block->length == 0;
}

// getBlockElement returns an element from a Block; caller must not change it.
void* getBlockElement(Block* block, int index) {
	return (index < 0 || index >= block->length) ? null : (block->elements)[index];
}

// setBlockElement replaces an element in a Block. The replaced element is freed.
void setBlockElement(Block* block, void* element, void(*delete)(void*), int index) {
	if (index < 0 || index >= block->length) return;
	if (delete && (block->elements)[index])
		delete((block->elements)[index]);
	(block->elements)[index] = element;
}

// findInBlock returns the element with given key if it exists; null otherwise; uses linear search.
void* findInBlock(Block* block, String key, String(*getKey)(void*), int* index) {
	return linearSearch(block->elements, block->length, key, getKey, index);
}

// findInSortedBlock returns the element with given key if it exists; null otherwise; uses
// binary search.
void* findInSortedBlock(Block* block, String key, String(*getKey)(void*),
						int(*compare)(String, String), int* index) {
	if (block->length == 0) {
		if (index) *index = 0;
		return null;
	}
	return binarySearch(block->elements, block->length, key, getKey, compare, index);
}

// isInBlock returns true if an element with given key is in the Block; linear search is used.
bool isInBlock(Block* block, String key, String(*getKey)(void*), int* index) {
	return findInBlock(block, key, getKey, index) != null;
}

// isInSortedBlock returns true if an element with given key is in the Block; binary search is used.
bool isInSortedBlock(Block* block, String key, String(*getKey)(void*),
					 int(*compare)(String, String), int* index) {
	return findInSortedBlock(block, key, getKey, compare, index);
}

// copyBlock creates and returns a copy of a Block.
Block *copyBlock(Block *block, void*(*copyfunc)(void*)) {
	Block* copy = createBlock();
	void** els = block->elements;
	for (int i = 0; i < block->length; i++) {
		appendToBlock(copy, copyfunc(els[i]));
	}
	return copy;
}

// appendToBlock appends a new element to the end of a Block's elements.
void appendToBlock(Block *block, void *element) {
	insertInBlock(block, element, block->length);
}

// prependToBlock prepends a new element to the front of a Block's elements.
void prependToBlock(Block *block, void *element) {
	insertInBlock(block, element, 0);
}

// insertInBlock inserts an element into a Block at a given index.
void insertInBlock(Block *block, void *element, int index) {
	ASSERT(block && element && index >= 0 && index <= block->length);
	if (block->length >= block->maxLength) growBlock(block);
	void **elements = block->elements;
	int length = block->length;
	insertAtIndex(elements, length, element, index);
	(block->length)++;
}

// getFromBlock returns the indexed elemment from the Block; it is not removed from the Block.
void* getFromBlock(Block* block, int index) {
	if (index < 0 || index >= block->length) return null;
	return block->elements[index];
}

// getFirstBlockElement returns the first element in the Block; it is not removed from the Block.
void* getFirstBlockElement(Block* block) {
	return getFromBlock(block, 0);
}

// getLastBlockElement return the last element in the Block; it is not removed from the Block.
void* getLastBlockElement(Block* block) {
	return getFromBlock(block, block->length - 1);
}

// removeFirstBlockEement removes the first element from the given Block.
bool removeFirstBlockElement(Block *block, void(*delete)(void*)) {
	return removeFromBlock(block, 0, delete);
}

// removeLastBlockElement removes the last elem,ent from the given Block.
bool removeLastBlockElement(Block *block, void(*delete)(void*)) {
	return removeFromBlock(block, block->length - 1, delete);
}

// removeFromBlock removes the element at a specific index from a Block's elements.
bool removeFromBlock(Block *block, int index, void(*delete)(void*)) {
	if (blockDebugging) printf("remove from %d\n", index);
	if (!block || index < 0 || index >= block->length) return false;
	void **elements = block->elements;
	if (delete) delete(elements[index]);
	for (; index < block->length - 1; index++)
		elements[index] = elements[index + 1];
	(block->length)--;
	return true;
}

// removeFromSortedBlock removes an element with specific key from a sorted Block's elements.
bool removeFromSortedBlock(Block *block, String key, String(*getKey)(void *a),
						   int(*compare)(String, String), void(*delete)(void*)) {
	if (blockDebugging) printf("remove %s from sorted block\n", key);
	int index = -1;
	if (!binarySearch(block->elements, block->length, key, getKey, compare, &index)) return false;
	removeFromBlock(block, index, delete);
	return true;
}

// removeFromUnsortedBlock removes an element with specific key from an unsorted Block's elements.
bool removeFromUnsortedBlock(Block* block, String key, String(*getKey)(void*),
							 void(*delete)(void*)) {
	if (blockDebugging) printf("remove %s from unsorted block\n", key);
	int index = -1;
	if (!linearSearch(block->elements, block->length, key, getKey, &index)) return false;
	removeFromBlock(block, index, delete);
	return true;
}

//  sortBlock sorts the elements in a Block.
void sortBlock(Block* block, String(*getKey)(void*), int(*compare)(String, String)) {
	if (blockDebugging) printf("sortBlock of length %d\n", block->length);
	sortElements(block->elements, block->length, getKey, compare);
}

// searchBlock searches an unsorted Block for an element. Index is set to its location.
void* searchBlock(Block* block, String key, String(*getKey)(void*), int* index) {
	if (index) *index = -1;
	return linearSearch(block->elements, block->length, key, getKey, index);
}

// searchSortedBlock searches a sorted Block for an element. Index is set to its location.
void* searchSortedBlock(Block *block, String key, String(*getKey)(void*),
						int(*compare)(String, String), int* index) {
	if (index) *index = -1;
	return binarySearch(block->elements, block->length, key, getKey, compare, index);
}

// showBlock is a debugging function that shows the contents of a Block.
void showBlock(Block *block, String(*getString)(void*)) {
	printf("Block: %d %d\n", (int) block->length, (int) block->maxLength);
	for (int i = 0; i < block->length; i++) {
		printf("%s\n", getString(block->elements[i]));
	}
	printf("\n");
}

// fprintfBlock is a debugging function that prints the contents of a Block to an open file.
void fprintfBlock(FILE* file, Block* block, String(*toString)(void*)) {
	if (!file) return;
	fprintf(file, "Block: %d %d\n", (int)block->length, (int)block->maxLength);
	for (int i = 0; i < block->length; i++) {
		fprintf(file, "%s\n", toString(block->elements[i]));
	}
	fprintf(file, "\n");
}

// isSorted returns true if the Block is sorted.
bool isSorted(Block *block, String(*getKey)(void*), int(*compare)(String, String)) {
	if (block->length <= 1) return true;
	void **elements = block->elements;
	String key = getKey(elements[0]);
	for (int i = 1; i < block->length; i++) {
		String next = getKey(elements[i]);
		if (strcmp(key, next) > 0) return false;
		key = next;
	}
	return true;
}

// iterateBlock iterates over the elements of a Block performing an action.
void iterateBlock(Block *block, void(*perform)(void*)) {
	ASSERT(block && perform);
	if (!perform) return;
	for (int i = 0; i < block->length; i++) {
		(*perform)(block->elements[i]);
	}
}

// uniqueBlock removes elements with duplicate keys from a Block. The Block must be ordered so
// that all elements with the same key are grouped together.
void uniqueBlock(Block* block, String(*getKey)(void*), void(*delete)(void*)) {
	ASSERT(block && getKey);
	int length = block->length;
	if (!block || !getKey || length <= 0) return;
	void** elements = block->elements;
	int j = 0;
	String key = getKey(elements[0]);
	for (int i = 1; i < length; i++) {
		String nkey = getKey(elements[i]);
		if (nestr(key, nkey)) {
			j++;
			elements[j] = elements[i];
			key = nkey;
		} else if (delete) {
			delete(elements[i]);
		}
	}
	block->length = j + 1;
}
