// connect.c
// Partition
//
// Created by Thomas Wetmore on 5 October 2024.
// Last changed on 11 October 2024.

#include "connect.h"
#include "gnodeindex.h"

static int getNumAncestors(GNode*, GNodeIndex*);
static int getNumDescendents(GNode*, GNodeIndex*);

// getConnections finds the numbers of ancestors and descendents of the persons in a list. The
// numbers are kept in ConnectData structs in the GNodeIndex. list is a List of GNode* roots,
// and index is an index of all all persons and families.
void getConnections(List* list, GNodeIndex* index) {
	FORLIST(list, el)
		GNode* root = (GNode*) el;
		GNodeIndexEl* el  = (GNodeIndexEl*) searchHashTable(index, root->key);
		ConnectData* data = el->data;
		if (!data->ancestorsDone) getNumAncestors(root, index);
		if (!data->descendentsDone) getNumDescendents(root, index);
	ENDLIST
}

// getNumAncestors returns the number of ancestors a person has.
static int getNumAncestors(GNode* root, GNodeIndex* index) {
	GNodeIndexEl* element = searchHashTable(index, root->key);
	ConnectData* data = element->data;
	if (data->ancestorsDone) return data->numAncestors; // Memoized.

	// Find number of ancestors.
	int ancestors = 0;
	for (GNode* pnode = root->child; pnode; pnode = pnode->sibling) {
		if (eqstr("FAMC", pnode->tag)) { // Families this person is a child in.
			GNodeIndexEl* felement = searchHashTable(index, pnode->value);
			GNode* family = felement->root;
			for (GNode* fnode = family->child; fnode; fnode = fnode->sibling) {
				if (eqstr("HUSB", fnode->tag) || eqstr("WIFE", fnode->tag)) {
					GNodeIndexEl* pelement = searchHashTable(index, fnode->value);
					ancestors += 1 + getNumAncestors(pelement->root, index);
				}
			}
		}
	}
	data->ancestorsDone = true;
	data->numAncestors = ancestors;
	return ancestors;
}

// getNumDescendents returns the number of descendents a person has.
static int getNumDescendents(GNode* root, GNodeIndex* index) {
	GNodeIndexEl* element = searchHashTable(index, root->key);
	ConnectData* data = element->data;
	if (data->descendentsDone) return data->numDescendents; // Memoized.

	// Find number of descendents.
	int descendents = 0;
	for (GNode* pnode = root->child; pnode; pnode = pnode->sibling) {
		if (eqstr("FAMS", pnode->tag)) { // Families this person is a spouse/parent in.
			GNodeIndexEl* felement = searchHashTable(index, pnode->value);
			GNode* family = felement->root;
			for (GNode* fnode = family->child; fnode; fnode = fnode->sibling) {
				if (eqstr("CHIL", fnode->tag)) { // Children in this family are descendents.
					GNodeIndexEl* pelement = searchHashTable(index, fnode->value);
					descendents += 1 + getNumDescendents(pelement->root, index);
				}
			}
		}
	}
	data->descendentsDone = true;
	data->numDescendents = descendents;
	return descendents;
}

// createConnectData creates the data field used in GNodeIndexEls in the Partition program.
ConnectData* createConnectData(void) {
	ConnectData* data = (ConnectData*) stdalloc(sizeof(ConnectData));
	data->ancestorsDone = data->descendentsDone = false;
	data->numAncestors = data->numDescendents = 0;
	return data;
}

// show is a static function passed to showGNodeIndex in order to show the ConnectData struct.
static void show(void* data) {
	ConnectData* connectData = data;
	if (connectData->ancestorsDone) printf("%d : ", connectData->numAncestors);
	else printf("- : ");
	if (connectData->descendentsDone) printf("%d\n", connectData->numDescendents);
	else printf("-\n : ");
}

// debugGNodeIndex prints the contents of a GNodeIndex.
void debugGNodeIndex(GNodeIndex* index) {
	showGNodeIndex(index, show);
}
