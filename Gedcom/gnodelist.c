// DeadEnds
//
// gnodelist.c
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 27 May 2024.

#include "gnodelist.h"
#include "readnode.h"

// Shared with readnode.c
extern int xfileLine;
extern String xfileName;
extern int xlevel;
extern String xkey;
extern String xtag;
extern String xvalue;

// createNodeListElement creates an element for a GNodeList. Node or error must be null.
GNodeListElement* createNodeListElement(GNode* node, int level, int lineNo, Error* error) {
	ASSERT(node || error && (!node || !error));
	GNodeListElement *element = (GNodeListElement*) malloc(sizeof(GNodeListElement));
	element->node = node;
	element->level = level;
	element->lineNo = lineNo;
	element->error = error;
	return element;
}

// getKey is the getKey function for NodeLists.
static String getKey(void* element) {
	return ((GNodeListElement*) element)->node->tag;
}

// createNodeList creates a GNodeList; one type holds all GNodes from a file, the other holds the
// roots of trees.
GNodeList* createNodeList(void) {
	GNodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// getNodeListFromFile uses fileToLine and extractFields to create a GNode for each line in a
// Gedcom file. Lines with errors store Errors in the list rather than GNodes.
GNodeList *getNodeListFromFile(FILE* fp, int* numErrors) {
	GNodeList* nodeList = createNodeList();
	Error* error;

	*numErrors = 0;
	ReadReturn rc = fileToLine(fp, &error);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			appendToList(nodeList, createNodeListElement(
							createGNode(xkey, xtag, xvalue, null), xlevel, xfileLine, null));
		} else {
			appendToList(nodeList, createNodeListElement(null, xlevel, xfileLine, error));
			(*numErrors)++;
		}
		rc = fileToLine(fp, &error);
	}
	if (lengthList(nodeList) > 0) return nodeList;
	deleteList(nodeList);
	return null;
}

// showNodeList shows the contents of a GNodeList. Debugging.
void showNodeList(GNodeList* nodeList) {
	FORLIST(nodeList, element)
		GNodeListElement *nodeListElement = (GNodeListElement*) element;
		printf("%d ", nodeListElement->lineNo);
		if (nodeListElement->error) {
			printf("Error: "); showError(nodeListElement->error);
		} else if (nodeListElement->node) {
			printf("Node: "); showGNode(nodeListElement->level, nodeListElement->node);
		}
	ENDLIST
}

#define isZeroLevelNode(element) ((element)->node && (element)->level == 0)
#define isNonZeroLevelNode(element) ((element)->node && (element)->level != 0)
#define isNodeElement(element) ((element)->node)
#define isErrorElement(element) ((element)->error)
#define elementFields(element) element->node->key, element->node->tag, element->node->value, null

// getNodeTreesFromNodeList scans a GNodeList of GNodes and creates a GNodeList of GNode trees;
// uses a three state state machine to track levels and errors.
GNodeList *getNodeTreesFromNodeList(GNodeList *lowerList, ErrorLog *errorLog) {
	enum ExState { InitialState, MainState, ErrorState };
	enum ExState state = InitialState;
	int prevLevel = 0;
	int curLevel = 0;
	int line0Number = 0;  // Line number of current root node.
	GNode *curRoot = null;
	GNode *curNode = null;
	GNodeList *rootNodeList = createNodeList();

	for (int index = 0; index < lengthList(lowerList); index++) {
		GNodeListElement *element = getListElement(lowerList, index);
		if (isNodeElement(element)) {
			prevLevel = curLevel;
			curLevel = element->level;
		}
		switch (state) {
		case InitialState:
			if (isZeroLevelNode(element)) {
				curRoot = curNode = createGNode(elementFields(element));
				line0Number = element->lineNo;
				state = MainState;
				continue;
			}
			if (isNonZeroLevelNode(element)) {
				addErrorToLog(errorLog, createError(syntaxError, xfileName, element->lineNo, "Illegal line level."));
				state = ErrorState;
				continue;
			}
			if (isErrorElement(element)) {
				addErrorToLog(errorLog, element->error);
				state = ErrorState;
				continue;
			}
			break;
		case MainState:
			// On error enter ErrorState but return the possibly partial current node tree.
			if (isErrorElement(element)) {
				addErrorToLog(errorLog, element->error);
				appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
				state = ErrorState;
				continue;
			}
			// A 0-level node is the first node of the next record. Return current node tree.
			if (isZeroLevelNode(element)) {
				appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
				curRoot = curNode = createGNode(elementFields(element));
				prevLevel = curLevel = 0;
				line0Number = element->lineNo;
				continue;
			}
			// Found sibling node. Add it to the tree and make it current.
			if (curLevel == prevLevel) {
				GNode *newNode = createGNode(elementFields(element));
				newNode->parent = curNode->parent;
				curNode->sibling = newNode;
				curNode = newNode;
				continue;
			}
			// Found child node. Add it to the tree and make it current.
			if (curLevel == prevLevel + 1) {
				GNode *newNode = createGNode(elementFields(element));
				newNode->parent = curNode;
				curNode->child = newNode;
				curNode = newNode;
				continue;
			}
			// Found higher level node. Make it a sibling to higher node and make it current.
			if (curLevel < prevLevel) {
				while (curLevel < prevLevel) {
					curNode = curNode->parent;
					prevLevel--;
				}
				GNode *newNode = createGNode(elementFields(element));
				curNode->sibling = newNode;
				newNode->parent = curNode->parent;
				curNode = newNode;
				continue;
			}
			// Anything else is an error.
			addErrorToLog(errorLog, createError(syntaxError, xfileName, element->lineNo, "Illegal level number."));
			appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
			state = ErrorState;
			continue;
		case ErrorState:
			if (isErrorElement(element)) continue;
			if (isNonZeroLevelNode(element)) continue;
			line0Number = element->lineNo;
			curRoot = curNode = createGNode(elementFields(element));
			prevLevel = curLevel = 0;
			state = MainState;
			continue;
		default:
			ASSERT(false);
		}
	}
	//  If in MainState at the end there is a a tree to add to the list.
	if (state == MainState) {
		appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
	}
	return rootNodeList;
}

// numberNodesInNodeList returns the number of GNodes or GNode trees in a GNodeList.
int numberNodesInNodeList(GNodeList* list) {
	int numNodes = 0;
	FORLIST(list, element)
		GNodeListElement *e = (GNodeListElement*) element;
		if (e->node) numNodes++;
	ENDLIST
	return numNodes;
}

// numberErrorsInNodeList returns the number of Errors in a GNodeList.
int numberErrorsInNodeList(GNodeList* list) {
	int numErrors = 0;
	FORLIST(list, element)
		GNodeListElement *e = (GNodeListElement*) element;
		if (e->error) numErrors++;
	ENDLIST
	return numErrors;
}
