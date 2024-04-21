//
// DeadEnds
//
// readnode.h is the header file for functions and variables that read GNodes from Gedcom files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 30 March 2024.

#ifndef readnode_h
#define readnode_h

#include "standard.h"
#include "gnode.h"
#include "errors.h"

// Return codes from fileToLine and extractFields.
typedef enum ReadReturn {
	ReadEOF = 0, // At end of file.
	ReadOkay,    // Last line okay.
	ReadError    // Last line has an error.
} ReadReturn;

// NodeList is a synonym for a List when used for Lists of NodeListElements.
typedef List NodeList;

// NodeListElement is the type of NodeList elements; either node or error will be null, not both.
typedef struct NodeListElement {
	GNode *node;
	int level;
	Error *error;
	int lineNo;
} NodeListElement;

// User interface to NodeList.
NodeList *createNodeList(void);
NodeListElement *createNodeListElement(GNode*, int level, int lineNo, Error*);
NodeList *getNodeListFromFile(FILE*, int*);
NodeList *getNodeTreesFromNodeList(NodeList*, ErrorLog*);
void showNodeList(NodeList *nodeList);  // Debugging.
int numberNodesInNodeList(NodeList *list);
int numberErrorsInNodeList(NodeList *list);

#endif
