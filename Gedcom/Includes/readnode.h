//
//  DeadEnds
//
//  readnode.h -- Header file for routines and variables that read Gedcom files.
//
//  Created by Thomas Wetmore on 17 December 2022.
//  Last changed on 12 December 2023.
//

#ifndef readnode_h
#define readnode_h

#include "standard.h"
#include "gnode.h"
#include "errors.h"

//  Return codes from fileToLine and extractFields.
//-------------------------------------------------------------------------------------------------
typedef enum ReadReturn {
	ReadEOF = 0, // File is at end of file.
	ReadOkay,    // Last line read was syntactically correct.
	ReadError    // Last line read had an error.
} ReadReturn;

//  NodeList -- List of NodeListElements. A List.
//-------------------------------------------------------------------------------------------------
typedef List NodeList;

//  NodeListElement -- Element of a NodeList. Either node or error will be null, but not both.
//-------------------------------------------------------------------------------------------------
typedef struct NodeListElement {
	GNode *node;
	int level;
	Error *error;
	int lineNo;
} NodeListElement;

NodeList *createNodeList(void);
NodeListElement *createNodeListElement(GNode*, int level, int lineNo, Error*);
NodeList *getNodeListFromFile(FILE*);
NodeList *getNodeTreesFromNodeList(NodeList*, ErrorLog*);
void showNodeList(NodeList *nodeList);  // Debugging.
int numberNodesInNodeList(NodeList *list);
int numberErrorsInNodeList(NodeList *list);

#endif
