// DeadEnds
//
// gnodelist.h
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 7 July 2024.

#ifndef gnodelist_h
#define gnodelist_h

#include "list.h"
#include "gnode.h"
#include "file.h"

// GNodeList is a List when used for Lists of GNodeListElements.
typedef List GNodeList;

// GNodeListEl is the type of GNodeList elements.
typedef struct GNodeListEl {
	GNode *node;
	int level;
	int line;
	struct GNodeListEl* elParent;
} GNodeListEl;

// User interface to GNodeList.
GNodeList* createGNodeList(void);
void deleteGNodeList(GNodeList*, bool del);
GNodeListEl* createGNodeListEl(GNode*, int lev, int line);
GNodeList* getGNodeListFromFile(File*, ErrorLog*);
GNodeList* getGNodeListFromString(String, ErrorLog*);
GNodeList* getGNodeTreesFromNodeList(GNodeList*, String file, ErrorLog*);
GNodeList* getGNodeTreesFromString(String, String, ErrorLog* errorLog);

GNodeList* getGNodeTreesFromFile(File*, ErrorLog*);
void showGNodeList(GNodeList* nodeList); // Debugging.

#endif // gnodelist_h
