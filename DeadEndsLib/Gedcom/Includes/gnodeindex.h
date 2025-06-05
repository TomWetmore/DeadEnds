//
//  DeadEnds Library
//  gnodeindex.h is the header file for the GNodeIndex type which is a hash table of GNodeIndexEls.
//  A GNodeIndexEl is a pair consisting of a root GNode and an arbitrary data object.
//
//  Created by Thomas Wetmore on 6 October 2024.
//  Last changed on 4 June 2025.
//

#ifndef gnodeindex_h
#define gnodeindex_h

typedef struct GNode GNode;
typedef struct HashTable HashTable;

// GNodeIndexEl is the type of elements in a GNode Index.
typedef struct GNodeIndexEl {
	GNode* root; // Root of Gedcom record.
	void* data; // Arbitrary data.
} GNodeIndexEl;

// GNodeIndex is a specialization of HashTable.
typedef HashTable GNodeIndex;

// Interface to GNodeIndex and GNodeIndexEl.
GNodeIndex* createGNodeIndex(void(*delete)(void*));
GNodeIndexEl* createGNodeIndexEl(GNode* root, void* data);
void addToGNodeIndex(GNodeIndex*, GNode*, void* data);
void showGNodeIndex(GNodeIndex*, void(*show)(void*));
GNode* searchGNodeIndex(GNodeIndex*, String key);

#endif // gnodeindex_h
