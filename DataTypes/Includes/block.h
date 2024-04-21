// DeadEnds
//
// block.h declares the structures and functions that implement the Block type.
//
// Created by Thomas Wetmore 8 March 2024.
// Last changed on 19 April 2024.

#ifndef block_h
#define block_h

#include "standard.h"

#define INITIAL_SIZE_LIST_DATA_BLOCK 30
//#define INITIAL_SIZE_LIST_DATA_BLOCK 4 // For debugging

// Block is a growable list of void* pointers.
typedef struct Block {
	int length;
	int maxLength;
	void** elements;
} Block;

Block *createBlock(void);
void initBlock(Block*); // TODO: Find a way to make this function private to Block.
void deleteBlock(Block*, void(*d)(void*));
void emptyBlock(Block*, void(*d)(void*));
int lengthBlock(Block*);
bool isEmptyBlock(Block*);

void* getBlockElement(Block*, int);
void setBlockElement(Block*, void*, void(*delete)(void*), int);

void* findInBlock(Block*, String, String(*g)(void*), int*);
void* findInSortedBlock(Block*, String, String(*g)(void*), int(*c)(String, String), int*);

bool isInBlock(Block*, String, String(*g)(void*), int*); // Linear search.
bool isInSortedBlock(Block*, String, String(*g)(void*), int(*c)(String, String), int*); // Binary search.

void prependToBlock(Block*, void*);
void appendToBlock(Block*, void*);
void insertInBlock(Block*, void*, int);
bool removeFromBlock(Block*, int, void(*d)(void*));
bool removeFromSortedBlock(Block*, String, String(*g)(void *a), int(*c)(String, String), void(*d)(void*));
bool removeFromUnsortedBlock(Block*, String, String(*g)(void *a), void(*d)(void*));
bool removeFirstBlockElement(Block*, void(*d)(void*));
bool removeLastBlockElement(Block*, void(*d)(void*));
void sortBlock(Block*, String(*g)(void*), int(*c)(String, String));
bool isSorted(Block*, String(*g)(void*), int(*c)(String, String));

void* getFromBlock(Block*, int);
void* getFirstBlockElement(Block*);
void* getLastBlockElement(Block*);

void uniqueBlock(Block*, String(*g)(void*), void(*d)(void*));
Block *copyBlock(Block*, void*(*copy)(void*));
void iterateBlock(Block*, void(*perform)(void*));

void* searchBlock(Block* block, String, String(*g)(void*), int*);
void* searchSortedBlock(Block* block, String key, String(*g)(void*), int(*c)(String, String), int*);

// Debugging only.
void showBlock(Block*, String(*describe)(void*));
void fprintfBlock(FILE*, Block*, String(*toString)(void*));

#endif // block_h
