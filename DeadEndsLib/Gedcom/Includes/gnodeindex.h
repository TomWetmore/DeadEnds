//
//  DeadEnds Library
//  gnodeindex.h
//
//  Created by Thomas Wetmore on 6 October 2024.
//  Last changed on 4 June 2025.
//xb

#ifndef gnodeindex_h
#define gnodeindex_h

typedef struct GNode GNode;
typedef struct HashTable HashTable;


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
