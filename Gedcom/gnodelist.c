// DeadEnds
//
// gnodelist.c
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 26 June 2024.

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
GNodeListElement* createNodeListElement(GNode* node, int level, int lineNo) {
	GNodeListElement *element = (GNodeListElement*) malloc(sizeof(GNodeListElement));
	element->node = node;
	element->level = level;
	element->lineNo = lineNo;
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
GNodeList* getNodeListFromFile(FILE* fp, ErrorLog* errorLog) {
	GNodeList* nodeList = createNodeList();
	Error* error;

	ReadReturn rc = fileToLine(fp, &error);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			appendToList(nodeList, createNodeListElement(
							createGNode(xkey, xtag, xvalue, null), xlevel, xfileLine));
		} else {
			addErrorToLog(errorLog, error);
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
		printf("Node: "); showGNode(nodeListElement->level, nodeListElement->node);
	ENDLIST
}

// getNodeTreesFromNodeList interates a GNodeList of GNodes to create a GNodeList of GNode trees;
// it uses a three state state machine to track levels and errors.
GNodeList *getNodeTreesFromNodeList(GNodeList *lowerList, ErrorLog *errorLog) {
	enum State { InitialState, MainState, ErrorState } state = InitialState;
	GNodeListElement* element = null;
	GNodeListElement* rootElement = null;
	GNodeListElement* previous = null;
	GNodeList *rootNodeList = createNodeList();
	Block *block = &(lowerList->block);
	void **elements = block->elements;

	for (int i = 0; i < block->length; i++) {
		previous = element;
		element = elements[i];
		//showGNode(element->level, element->node);
		switch (state) {
		case InitialState: // At first element.
			if (element->level == 0) {
				rootElement = element;
				state = MainState;
				break;
			}
			addErrorToLog(errorLog,
				createError(syntaxError, xfileName, element->lineNo, 	"Illegal line level."));
			state = ErrorState;
			break;
		case MainState: // Normal state.
			if (element->level == 0) { // Found 0 level node of next record.
				appendToList(rootNodeList, rootElement);
				rootElement = element;
				break;
			}
			if (element->level == previous->level) { // Found sibling.
				element->elParent = previous->elParent;
				element->node->parent = previous->node->parent;
				previous->node->sibling = element->node;
				break;
			}
			if (element->level == previous->level + 1) { // Found child.
				element->elParent = previous;
				element->node->parent = previous->node;
				previous->node->child = element->node;
				break;
			}
			if (element->level < previous->level) { // Found lower (higher!) level.
				int count = 0;
				while (element->level < previous->level) {
					if (count++ > 100) {
						printf("Infinite Loop in Backing up tree?\n");
						exit(4);
					}
					previous = previous->elParent;
				}
				previous->node->sibling = element->node;
				element->node->parent = previous->node->parent;
				element = previous;
				break;
			}
			// Anything else is an error.
			addErrorToLog(errorLog, createError(syntaxError, xfileName, element->lineNo, "Illegal level number."));
			appendToList(rootNodeList, rootElement);
			state = ErrorState;
			break;
		case ErrorState:
			if (element->level != 0) continue;
			state = MainState;
			break;
		default:
			ASSERT(false);
		}
	}
	if (state == MainState) { // Add last tree to list.
		appendToList(rootNodeList, rootElement);
	}
	return rootNodeList;
}
