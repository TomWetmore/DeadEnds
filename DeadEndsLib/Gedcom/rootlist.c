// DeadEnds
//
// rootlist.c holds the RootList datatype. A RootList is a List whose elements are pointers to
// the root GNodes of Gedcom records.
//
// Created by Thomas Wetmore on 2 March 2024.
// Last changed on 7 May 2025.

#include "rootlist.h"
#include "gnodelist.h"
#include "gnode.h"
#include "writenode.h"

// getKey is the get key function for RootLists.
static String getKey(void* element) {
	return ((GNode*) element)->key;
}

// compare is the compare function for RootLists.
static int compare(String a, String b) {
	ASSERT(a && b && strlen(a) > 2 && strlen(b) > 2);
	if (strlen(a) != strlen(b)) return (int) (strlen(a) - strlen(b));
	for (int i = 0; i < strlen(a); i++)
		if (a[i] != b[i]) return a[i] - b[i];
	return 0;
}

// createRootList creates and returns a RootList; a RootList is a sorted List.
RootList *createRootList(void) {
	return createList(getKey, compare, null, true);
}

void deleteRootList(RootList* rootlist) {
    deleteList(rootlist);
}

// insertInRootList adds a GNode* to a RootList.
void insertInRootList(RootList* list, GNode* root) {
	int index = -1;
	if (findInList(list, list->getKey(root), &index)) {
		printf("THERE IS A ERROR -- DUPLICATE ROOT KEYS\n");
		return;
	}
	insertInList(list, root, index);
}

// getNodeTreesFromNodeList processes the GNodeList of all GNodes from a Gedcom source into
// A RootList of all GNode records. It uses a state machine to track levels and errors, so the
// data field in the GNodeList must be level numbers. The input GNodeLisit is not deleted.
RootList* getRootListFromGNodeList(GNodeList *gnodes, String name, ErrorLog *elog) {
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
		cnode = el->node; // TODO: how is this possible; how is the type of el known?
		plevel = clevel;
		clevel = ((int)(long)(el->data));
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
				appendToList(roots, rnode);
				rnode = cnode; // Set current root.
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
			if (((int)(long)(el->data)) != 0) continue;
			state = MainState;
			continue;

		default:
			ASSERT(false);
		}
	}
	if (state == MainState) { // Add last tree to list.
		appendToList(roots, rnode);
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

// showRootList show the root GNodes of the trees in the RootList.
void showRootList(RootList* list) {
	printf("Showing a Root List.\n");
	FORLIST(list, element)
	GNode* node = (GNode*) element;
	printf("%s %s\n", node->key ? node->key : "no key", node->tag);
	ENDLIST
}
