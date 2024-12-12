// DeadEnds
//
// gnodelist.h
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 11 December 2024.

#ifndef gnodelist_h
#define gnodelist_h

#include "list.h"
#include "gnode.h"
#include "file.h"
#include "integertable.h"
#include "errors.h"

// GNodeList is a List of GNodeListEls.
typedef List GNodeList;

// GNodeListEl is the type of GNodeList elements.
typedef struct GNodeListEl {
	GNode *node;
	void* data;
} GNodeListEl;

// User interface to GNodeList.
GNodeListEl* createGNodeListEl(GNode*, void*);
GNodeList* createGNodeList(void);
void deleteGNodeList(GNodeList*, void(*delete)(void*));
GNodeList* getGNodeListFromFile(File*, IntegerTable*, ErrorLog*);
GNodeList* getGNodeListFromString(String, ErrorLog*);
GNodeList* getGNodeTreesFromString(String, String, ErrorLog* errorLog);
void writeGNodeTreesToFile(GNodeList*, File*);
void appendToGNodeList(GNodeList*, GNode*, void*);
void showGNodeList(GNodeList* nodeList); // Debugging.

#endif // gnodelist_h
