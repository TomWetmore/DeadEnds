// DeadEnds
//
// gnodelist.h
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 1 July 2024.

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
GNodeList *createNodeList(void);
void deleteNodeList(GNodeList*, bool delEl);
GNodeListEl *createNodeListEl(GNode*, int level, int lineNo);
GNodeList *getNodeListFromFile(File*, ErrorLog*);
GNodeList *getNodeTreesFromNodeList(GNodeList*, String file, ErrorLog*);
void showNodeList(GNodeList *nodeList);  // Debugging.
int numberNodesInNodeList(GNodeList *list);
int numberErrorsInNodeList(GNodeList *list);

#endif // gnodelist_h
