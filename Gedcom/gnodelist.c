// DeadEnds
//
// gnodelist.c implements the GNodeList data type.
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 8 July 2024.

#include "gnodelist.h"
#include "readnode.h"
#include "file.h"

// createNodeListElement creates a GNodeList element. gnode is the GNode, level is its level, and
// line is its line number from the Gedcom file. level is needed because GNodes don't hold their
// levels.
GNodeListEl* createGNodeListEl(GNode* gnode, int level, int line) {
	GNodeListEl *element = (GNodeListEl*) malloc(sizeof(GNodeListEl));
	element->node = gnode;
	element->level = level;
	element->line = line;
	element->elParent = null;
	return element;
}

// getKey is the getKey function for NodeLists. Only useful for elements with level 0 GNodes.
static String getKey(void* element) {
	return ((GNodeListEl*) element)->node->key;
}

// delete is the delete function for GNodeLists.
static void delete(void* element) {
	freeGNodes(((GNodeListEl*) element)->node);
}

// createGNodeList creates a GNodeList. delete function not set; it is controlled by the boolean
// parameter to the deleteGNodeList function.
GNodeList* createGNodeList(void) {
	GNodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// deleteGNodeList deletes a GNodeList; if delNodes is true the GNodes are also deleted.
void deleteGNodeList(GNodeList* list, bool delNodes) {
	if (delNodes) list->delete = delete;
	else list->delete = null;
	deleteList(list);
}

// getGNodeTreesFromFile gets the GNodeList for the Gedcom records in a Gedcom file. If successful
// the list is returned; otherwise null is returned and the ErrorLog holds the errors.
GNodeList* getGNodeTreesFromFile(File* file, ErrorLog* errorLog) {
	int numErrors = lengthList(errorLog);
	GNodeList* nodeList = getGNodeListFromFile(file, errorLog);
	if (numErrors != lengthList(errorLog)) {
		if (nodeList) deleteGNodeList(nodeList, true);
		return null;
	}
	GNodeList* rootList = getGNodeTreesFromNodeList(nodeList, file->name, errorLog);
	if (numErrors != lengthList(errorLog)) {
		deleteGNodeList(nodeList, true);
		if (rootList) deleteGNodeList(rootList, true);
		return null;
	}
	return rootList;
}

// getGNodeListFromFile uses fileToLine and extractFields to create a GNodeList of all the GNodes
// from a Gedcom file. Any errors are added to the ErrorLog.
GNodeList* getGNodeListFromFile(File* file, ErrorLog* errorLog) {
	GNodeList* nodeList = createGNodeList();
	int level;
	int line = 0;
	String key, tag, value;
	String errstr;
	FILE* fp = file->fp;

	ReadReturn rc = fileToLine(fp, &line, &level, &key, &tag, &value, &errstr);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			GNode* gnode = createGNode(key, tag, value, null);
			GNodeListEl* el = createGNodeListEl(gnode, level, line);
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

// getGnodeTreesFromString reads a String holding Gedcom records and returns them as a GNodeList.
GNodeList* getGNodeTreesFromString(String string, String name, ErrorLog* errorLog) {
	int numErrors = lengthList(errorLog);
	GNodeList* nodeList = getGNodeListFromString(string, errorLog);
	if (numErrors != lengthList(errorLog)) {
		if (nodeList) deleteGNodeList(nodeList, true);
		return null;
	}
	GNodeList* rootList = getGNodeTreesFromNodeList(nodeList, name, errorLog);
	if (numErrors != lengthList(errorLog)) {
		deleteGNodeList(nodeList, true);
		if (rootList) deleteGNodeList(rootList, true);
		return null;
	}
	return rootList;
}

// getGNodeListFromString reads a String with Gedcom records and creates and converts them into
// a GNodeList.
GNodeList* getGNodeListFromString(String string, ErrorLog* errorLog) {
	GNodeList* nodeList = createGNodeList();
	int level;
	int line = 0;
	String key, tag, value;
	String errstr;
	String b = string;
	ReadReturn rc = stringToLine(&b, &line, &level, &key, &tag, &value, &errstr);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			GNode* gnode = createGNode(key, tag, value, null);
			GNodeListEl* el = createGNodeListEl(gnode, level, line);
			appendToList(nodeList, el);
		} else {
			Error* error = createError(gedcomError, "string", line, errstr);
			addErrorToLog(errorLog, error);
		}
		rc = stringToLine(&b, &line, &level, &key, &tag, &value, &errstr);
	}
	if (lengthList(nodeList) > 0) return nodeList;
	deleteList(nodeList);
	return null;
}

// showGNodeList shows the contents of a GNodeList. Debugging.
void showGNodeList(GNodeList* nodeList) {
	FORLIST(nodeList, element)
		GNodeListEl *el = (GNodeListEl*) element;
		printf("%d ", el->line);
		printf("Node: "); showGNode(el->level, el->node);
	ENDLIST
}

// getNodeTreesFromNodeList iterates a GNodeList of GNodes to make a GNodeList of GNode trees;
// it uses a state machine to track levels and errors.
GNodeList* getGNodeTreesFromNodeList(GNodeList *lowerList, String name, ErrorLog *errorLog) {
	enum State { InitialState, MainState, ErrorState } state = InitialState;
	GNodeListEl* element = null;
	GNodeListEl* rootElement = null;
	GNodeListEl* previous = null;
	GNodeList* rootNodeList = createGNodeList();
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
				createError(syntaxError, name, element->line, "Illegal line level."));
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
			addErrorToLog(errorLog, createError(syntaxError, name, element->line, "Illegal level number."));
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
