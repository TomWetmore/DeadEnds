// DeadEnds
//
// gnodelist.c implements the GNodeList data type.
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 25 November 2024.

#include "gnodelist.h"
#include "readnode.h"
#include "writenode.h"
#include "integertable.h"
#include "file.h"

#define level(e) ((int)(long)(e->data))

static bool debugging = true;

// createNodeListElement creates a GNodeList element. gnode is the GNode, and data is arbitrary.
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

// getGNodeTreesFromFile returns the GNodeList of all GNode records from a Gedcom file, including
// HEAD and TRLR. If there are errors returns null with errors in the ErrorLog.
static void deleteEl(void* el) { stdfree(el); }
GNodeList* getGNodeTreesFromFile(File* file, IntegerTable* keymap, ErrorLog* elog) {
	int nerrors = lengthList(elog);
	// getGNodeListFromFile gets the full list of all GNodes in the file.
	GNodeList* nodes = getGNodeListFromFile(file, keymap, elog);
	if (nerrors != lengthList(elog)) {
		if (nodes) {
			deleteGNodeList(nodes, deleteEl);
			deleteHashTable(keymap);
		}
		return null;
	}
	// getGNodeTreesFromNodeList process the GNodeList of a GNodes into a GNodeList of GNode trees.
	GNodeList* roots = getGNodeTreesFromNodeList(nodes, file->name, elog);
	if (nerrors != lengthList(elog)) {
		deleteGNodeList(nodes, deleteEl);
		deleteHashTable(keymap);
		if (roots) deleteGNodeList(roots, deleteEl); // TODO: SHOULD ALSO DELETE GNODES.
		return null;
	}
	return roots;
}

// getGNodeListFromFile uses fileToLine to get the GNodeList of all GNodes in a Gedcom file. If
// the keymap is not null it is set to map record keys to the lines where defined. Syntax errors
// are added to the ErrorLog. The file is fully processed regardless of errors. However, if errors
// were found the list of nodes is deleted and null is returned.
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
	GNodeList* rootList = getGNodeTreesFromNodeList(nodeList, name, errorLog);
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

// showGNodeList shows the contents of a GNodeList. Debugging.
void showGNodeList(GNodeList* nodeList) {
	FORLIST(nodeList, element)
		GNodeListEl *el = (GNodeListEl*) element;
		printf("Node: "); showGNode(level(el), el->node);
	ENDLIST
}

// getNodeTreesFromNodeList processes a list of all GNodes from a Gedcom source into a GNodeList
// of all GNode record trees. It uses a state machine to track levels and errors, so the auxiliary
// field in the original GNodes must be level numbers.
// MNOTE: the input GNodeList is not deleted.
GNodeList* getGNodeTreesFromNodeList(GNodeList *gnodes, String name, ErrorLog *elog) {
	enum State { InitialState, MainState, ErrorState } state = InitialState;
	GNodeListEl* el = null;
	GNodeList* roots = createGNodeList(); // Holds list of roots.
	GNode* rnode = null; // cur root node
	GNode* cnode = null; // cur node
	GNode* pnode; // prev node
	int clevel = 0; // cur node's level
	int plevel = 0; // prev node's level
	Block* block = &(gnodes->block);
	void** els = block->elements; // Starting elements as simple array.
	for (int i = 0; i < block->length; i++) {
		el = els[i];
		pnode = cnode;
		cnode = el->node; // TODO: how is this possibe; how is the type of el known?
		plevel = clevel;
		clevel = level(el);
		switch (state) {

		case InitialState: // First element.
			if (clevel == 0) {
				rnode = cnode; // Set current root.
				state = MainState;
				break;
			}
			addErrorToLog(elog, createError(syntaxError, name, i, "Illegal line level."));
			state = ErrorState;
			continue;

		case MainState:
			if (clevel == 0) { // found next root
				appendToGNodeList(roots, rnode, null);
				rnode = cnode; // Set current root.
				String key = cnode->key;
				continue;
			}
			if (clevel == plevel) { // found sibling
				cnode->parent = pnode->parent;
				pnode->sibling = cnode;
				continue;
			}
			if (clevel == plevel + 1) { // found child.
				cnode->parent = pnode;
				pnode->child = cnode;
				continue;
			}
			if (clevel < plevel) { // found uncle (who must have prev sib).
				int count = 0;
				while (clevel < plevel) {
					if (count++ > 50 || !pnode->parent) ASSERT(false); // infinite loop?
					pnode = pnode->parent;
					plevel--;
				}
				cnode->parent = pnode->parent;
				pnode->sibling = cnode;
				continue;
			}
			// Anything else is an error.
			addErrorToLog(elog, createError(syntaxError, name, i, "Illegal level number."));
			appendToGNodeList(roots, rnode, null);
			state = ErrorState;
			continue;

		case ErrorState:
			if (level(el) != 0) continue;
			state = MainState;
			continue;

		default:
			ASSERT(false);
		}
	}
	if (state == MainState) { // Add last tree to list.
		appendToGNodeList(roots, rnode, null);
	}
	return roots;
}

// writeGNodeTreesToFile writes a GNodeList to a File in Gedcom format.
void writeGNodeTreesToFile(GNodeList* list, File* file) {
	FORLIST(list, element)
		GNode* root = ((GNodeListEl*) element)->node;
		writeGNodeRecord(file->fp, root, false);
	ENDLIST
}
