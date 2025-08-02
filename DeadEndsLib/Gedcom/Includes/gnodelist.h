//
// DeadEnds Library
//
//  gnodelist.h
//
//  Created by Thomas Wetmore on 27 May 2024.
//  Last changed on 2 August 2025.
//

#ifndef gnodelist_h
#define gnodelist_h

#include "standard.h"

typedef struct File File;
typedef struct GNode GNode;
typedef struct HashTable HashTable;
typedef struct List List;
typedef List ErrorLog;
typedef HashTable IntegerTable;

// GNodeList is a List of GNodeListEls.
typedef List GNodeList;

// GNodeListEl is the type of GNodeList elements.
typedef struct GNodeListEl {
	GNode *node;
    int level;
	//void* data;
} GNodeListEl;

// User interface to GNodeList.
GNodeListEl* createGNodeListEl(GNode*, int);
GNodeList* createGNodeList(void);
void deleteGNodeList(GNodeList*);
GNodeList* getGNodeListFromFile(File*, IntegerTable*, ErrorLog*);
GNodeList* getGNodeListFromString(String, ErrorLog*);
GNodeList* getGNodeTreesFromString(String, String, ErrorLog* errorLog);
void writeGNodeTreesToFile(GNodeList*, File*);
void appendToGNodeList(GNodeList*, GNode*, int);
void showGNodeList(GNodeList* nodeList); // Debugging.

#endif // gnodelist_h
