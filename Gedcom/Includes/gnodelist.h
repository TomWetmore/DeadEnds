// DeadEnds
//
// gnodelist.h
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 24 June 2024.

#ifndef gnodelist_h
#define gnodelist_h

#include "list.h"
#include "gnode.h"

// GNodeList is a List when used for Lists of GNodeListElements.
typedef List GNodeList;

// GNodeListElement is the type of GNodeList elements; either node or error will be null.
typedef struct GNodeListElement {
	GNode *node;
	int level;
	int lineNo;
	struct GNodeListElement* elParent;
} GNodeListElement;

// User interface to GNodeList.
GNodeList *createNodeList(void);
GNodeListElement *createNodeListElement(GNode*, int level, int lineNo);
GNodeList *getNodeListFromFile(FILE*, ErrorLog*);
GNodeList *getNodeTreesFromNodeList(GNodeList*, ErrorLog*);
void showNodeList(GNodeList *nodeList);  // Debugging.
int numberNodesInNodeList(GNodeList *list);
int numberErrorsInNodeList(GNodeList *list);

#endif // gnodelist_h
