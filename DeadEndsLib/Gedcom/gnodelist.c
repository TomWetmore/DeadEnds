//
//  DeadEnds Library
//
//  gnodelist.c implements the GNodeList data type.
//
//  Created by Thomas Wetmore on 27 May 2024.
//  Last changed on 3 June 2025.
//

#include "file.h"
#include "gnodelist.h"
#include "integertable.h"
#include "readnode.h"
#include "rootlist.h"
#include "writenode.h"

static bool debugging = true;

// createNodeListEl creates a GNodeList element. gnode is the GNode, and data is arbitrary.
GNodeListEl* createGNodeListEl(GNode* gnode, void* data) {
	GNodeListEl *element = (GNodeListEl*) malloc(sizeof(GNodeListEl));
	element->node = gnode;
	element->data = data;
	return element;
}

// getKey is the getKey function for NodeLists. Only useful for elements with level 0 GNodes.
static String getKey(void* element) {
	return ((GNodeListEl*) element)->node->key;
}

// delete is the delete function for GNodeLists. It frees the GNodes.
static void delete(void* element) {
	freeGNodes(((GNodeListEl*) element)->node);
}

// createGNodeList creates a GNodeList. delete function not set; it is controlled by the boolean
// parameter to the deleteGNodeList function.
GNodeList* createGNodeList(void) {
	GNodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// addToGNodeList adds a new element to a GNodeList.
void appendToGNodeList(GNodeList* list, GNode* gnode, void* data) {
	GNodeListEl* el = createGNodeListEl(gnode, data);
	appendToList(list, el);
}

// deleteGNodeList deletes a GNodeList using caller provided delete function.
void deleteGNodeList(GNodeList* list, void (*delete)(void*)) {
	list->delete = delete;
	deleteList(list);
}

// getGNodeListFromFile uses fileToLine to get the GNodeList of all GNodes in a Gedcom file. If
// the keymap is not null it is used to map record keys to the lines where defined. Syntax errors
// are added to the ErrorLog. The file is fully processed regardless of errors. If errors are
// found the list is deleted and null is returned. The data field in the GNodeListEl holds the
// Gedcom level of the GNode. This is necessary because getRootListFromGNodeList needs that
// informmation for its state machine.
GNodeList* getGNodeListFromFile(File* file, IntegerTable* keymap, ErrorLog* elog) {
	ASSERT(file && file->fp && elog);
	FILE* fp = file->fp;
	GNodeList* nodeList = createGNodeList();
	int level;
	int line = 0;
	String key, tag, value;
	String errstr;

	// Read lines and create nodes.
	ReadReturn rc = fileToLine(fp, &line, &level, &key, &tag, &value, &errstr);
	while (rc != ReadAtEnd) {
		if (rc == ReadOkay) {
			GNode* gnode = createGNode(key, tag, value, null);
			GNodeListEl* el = createGNodeListEl(gnode, (void*)(long) level);
			if (key && keymap) insertInIntegerTable(keymap, gnode->key, line);
			appendToList(nodeList, el);
		} else {
			Error* error = createError(gedcomError, file->name, line, errstr);
			addErrorToLog(elog, error);
		}
		rc = fileToLine(fp, &line, &level, &key, &tag, &value, &errstr);
	}
	if (lengthList(nodeList) > 0) return nodeList;
	deleteList(nodeList);
	return null;
}

// getGnodeTreesFromString reads a String holding Gedcom records and returns them as a GNodeList.
// TODO: The string functions have not caught up!!
GNodeList* getGNodeTreesFromString(String string, String name, ErrorLog* errorLog) {
	int numErrors = lengthList(errorLog);
	GNodeList* nodeList = getGNodeListFromString(string, errorLog);
	if (numErrors != lengthList(errorLog)) {
		if (nodeList) deleteGNodeList(nodeList, null);
		return null;
	}
	RootList* rootList = getRootListFromGNodeList(nodeList, name, errorLog);
	if (numErrors != lengthList(errorLog)) {
		deleteGNodeList(nodeList, null); // TODO: GET DELETE DONE RIGHT.
		if (rootList) deleteGNodeList(rootList, null); // TODO: GET DELETE DONE RIGHT.
		return null;
	}
	return rootList;
}

// getGNodeListFromString reads a String with Gedcom records and creates and converts them into
// a GNodeList.
// TODO: The string functions have not caught up.
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
			GNodeListEl* el = createGNodeListEl(gnode, (void*)(long) level);
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
