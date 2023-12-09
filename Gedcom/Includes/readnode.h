//
//  DeadEnds
//
//  readnode.h -- Header file for routines and variables that read Gedcom files.
//
//  Created by Thomas Wetmore on 17 December 2022.
//  Last changed on 9 December 2023.
//

#ifndef readnode_h
#define readnode_h

#include "standard.h"
#include "gnode.h"
#include "errors.h"

//  NodeList -- List of NodeListElements
//-------------------------------------------------------------------------------------------------
typedef List NodeList;

//  NodeListElement -- Element in a NodeList.
//-------------------------------------------------------------------------------------------------
typedef struct NodeListElement {
	GNode *node;
	int level;
	Error *error;
	int lineNo;
} NodeListElement;

NodeList *createNodeList(void);
NodeListElement *createNodeListElement(GNode*, int level, int lineNo, Error*);
NodeList *getNodeList(FILE*, ErrorLog*);
void showNodeList(NodeList *nodeList);


GNode* firstNodeTreeFromFile(FILE*, String file, int *line, ErrorLog*);
GNode* nextNodeTreeFromFile(FILE*, int *line, ErrorLog*);

#endif
