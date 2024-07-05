// DeadEnds
//
// gnodelist.c implements the GNodeList data type.
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 2 July 2024.

#include "gnodelist.h"
#include "readnode.h"
#include "file.h"

// Shared with readnode.c
// DEPRECATED.
//extern int xfileLine;
extern String xfileName;
//extern int xlevel;
extern String xkey;
extern String xtag;
extern String xvalue;

// createNodeListElement creates an element for a GNodeList.
GNodeListEl* createNodeListEl(GNode* node, int level, int lineNo) {
	GNodeListEl *element = (GNodeListEl*) malloc(sizeof(GNodeListEl));
	element->node = node;
	element->level = level;
	element->line = lineNo;
	element->elParent = null; // Used when building trees.
	return element;
}

// getKey is the getKey function for NodeLists. Meaningful for elements with level 0 GNodes.
static String getKey(void* element) {
	return ((GNodeListEl*) element)->node->tag;
}

// delete is the delete function for GNodeLists.
static void delete(void* element) {
	freeGNodes(((GNodeListEl*) element)->node);
}

// createNodeList creates a GNodeList. There are currently two types, one that holds all GNodes
// from and file, and one that holds the roots of GNode trees.
GNodeList* createNodeList(void) {
	GNodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// deleteNodeList deletes a GNodeList; if delNodes is true the GNodes are also deleted.
void deleteNodeList(GNodeList* list, bool delNodes) {
	if (delNodes) list->delete = delete;
	else list->delete = null;
	deleteList(list);
}

// getNodeListFromFile uses fileToLine and extractFields to create a GNode for each line in a
// Gedcom file. Lines with errors add Errors to the ErrorLog.
// DEPRECATED.
//GNodeList* oldGetNodeListFromFile(File* file, ErrorLog* errorLog) {
//	GNodeList* nodeList = createNodeList();
//	Error* error;
//
//	ReadReturn rc = oldFileToLine(file, &error);
//	while (rc != ReadAtEnd) {
//		if (rc == ReadOkay) {
//			appendToList(nodeList, createNodeListEl(
//							createGNode(xkey, xtag, xvalue, null), xlevel, 0/*xfileLine*/));
//		} else {
//			addErrorToLog(errorLog, error);
//		}
//		rc = oldFileToLine(file, &error);
//	}
//	if (lengthList(nodeList) > 0) return nodeList;
//	deleteList(nodeList);
//	return null;
//}

// getNodeListFromFile uses fileToLine and extractFields to create a GNodeList of all the GNodes
// from a Gedcom file. Any errors are added to the ErrorLog.
GNodeList* getNodeListFromFile(File* file, ErrorLog* errorLog) {
	GNodeList* nodeList = createNodeList();
	int level;
	int line = 0;
	String key, tag, value;
	String errstr;
	FILE* fp = file->fp;

	ReadReturn rc = fileToLine(fp, &line, &level, &key, &tag, &value, &errstr);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			GNode* gnode = createGNode(key, tag, value, null);
			GNodeListEl* el = createNodeListEl(gnode, level, line);
			appendToList(nodeList, el);
		} else {
			Error* error = createError(gedcomError, file->name, line, errstr);
			addErrorToLog(errorLog, error);
		}
		rc = fileToLine(fp, &line, &level, &key, &tag, &value, &errstr);
	}
	if (lengthList(nodeList) > 0) return nodeList;
	deleteList(nodeList);
	return null;
}

// showNodeList shows the contents of a GNodeList. Debugging.
void showNodeList(GNodeList* nodeList) {
	FORLIST(nodeList, element)
		GNodeListEl *e = (GNodeListEl*) element;
		printf("%d ", e->line);
		printf("Node: "); showGNode(e->level, e->node);
	ENDLIST
}

// getNodeTreesFromNodeList iterates a GNodeList of GNodes to make a GNodeList of GNode trees;
// it uses a state machine to track levels and errors.
GNodeList* getNodeTreesFromNodeList(GNodeList *lowerList, String fileName, ErrorLog *errorLog) {
	enum State { InitialState, MainState, ErrorState } state = InitialState;
	GNodeListEl* element = null;
	GNodeListEl* rootElement = null;
	GNodeListEl* previous = null;
	GNodeList* rootNodeList = createNodeList();
	Block* block = &(lowerList->block);
	void** elements = block->elements;

	for (int i = 0; i < block->length; i++) {
		previous = element;
		element = elements[i];
		switch (state) {
		case InitialState: // At first element.
			if (element->level == 0) {
				rootElement = element;
				state = MainState;
				break;
			}
			addErrorToLog(errorLog,
				createError(syntaxError, fileName, element->line, 	"Illegal line level."));
			state = ErrorState;
			continue;;
		case MainState: // Normal state.
			if (element->level == 0) { // Found root of next record.
				appendToList(rootNodeList, rootElement);
				rootElement = element;
				continue;
			}
			if (element->level == previous->level) { // Found sibling.
				element->elParent = previous->elParent;
				element->node->parent = previous->node->parent;
				previous->node->sibling = element->node;
				continue;
			}
			if (element->level == previous->level + 1) { // Found child.
				element->elParent = previous;
				element->node->parent = previous->node;
				previous->node->child = element->node;
				continue;
			}
			if (element->level < previous->level) { // Found lower level.
				int count = 0;
				while (element->level < previous->level) {
					if (count++ > 100) {
						printf("Infinite Loop in Backing up tree?\n");
						exit(4);
					}
					previous = previous->elParent;
				}
				element->elParent = previous->elParent;
				previous->node->sibling = element->node;
				element->node->parent = previous->node->parent;
				continue;
			}
			// Anything else is an error.
			addErrorToLog(errorLog, createError(syntaxError, xfileName, element->line, "Illegal level number."));
			appendToList(rootNodeList, rootElement);
			state = ErrorState;
			continue;
		case ErrorState:
			if (element->level != 0) continue;
			state = MainState;
			continue;
		default:
			ASSERT(false);
		}
	}
	if (state == MainState) { // Add last tree to list.
		appendToList(rootNodeList, rootElement);
	}
	return rootNodeList;
}
