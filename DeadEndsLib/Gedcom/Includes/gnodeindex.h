// gnodeindex.h
// Library/Gedcom
//
// Created by Thomas Wetmore on 6 October 2024.
// Last changed on 10 December 2024.

#ifndef gnodeindex_h
#define gnodeindex_h

#include "standard.h"
#include "hashtable.h"
#include "gnode.h"

typedef struct GNodeIndexEl {
	GNode* root; // Root of Gedcom record.
	void* data; // Arbitrary data.
} GNodeIndexEl;

typedef HashTable GNodeIndex;

GNodeIndex* createGNodeIndex(void(*delete)(void*));
GNodeIndexEl* createGNodeIndexEl(GNode* root, void* data);
void addToGNodeIndex(GNodeIndex*, GNode*, void* data);
void showGNodeIndex(GNodeIndex*, void(*show)(void*));
GNode* searchGNodeIndex(GNodeIndex*, String key);


#endif // gnodeindex_h
