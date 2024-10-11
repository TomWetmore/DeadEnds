// connect.c
// Partition
//
// Created by Thomas Wetmore on 5 October 2024.
// Last changed on 10 October 2024.

#include "connect.h"
#include "gnodeindex.h"

static int getNumAncestors(GNode*, GNodeIndex*);
static int getNumDescendents(GNode*, GNodeIndex*);

// getConnections finds the numbers of ancestors and descendents of each person in a list.
// The numbers are put in the ConnectData structures in the GNodeIndex. This function uses
// memoization to minimize computation.
void getConnections(List* list, GNodeIndex* index) {
	// Loop over the persons in the list.
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
	if (!element) FATAL(); // Can't happen.
	ConnectData* data = element->data;
	(data->numVisits)++;
	if (data->ancestorsDone) return data->numAncestors; // Already found.

	// Find number of ancestors.
	int ancestors = 0;
	for (GNode* pnode = root->child; pnode; pnode = pnode->sibling) {
		if (eqstr("FAMC", pnode->tag)) { // Families this person is a child in.
			GNodeIndexEl* felement = searchHashTable(index, pnode->value);
			if (!felement) FATAL(); // Can't happen.
			GNode* family = felement->root;
			for (GNode* fnode = family->child; fnode; fnode = fnode->sibling) {
				if (eqstr("HUSB", fnode->tag) || eqstr("WIFE", fnode->tag)) {
					GNodeIndexEl* pelement = searchHashTable(index, fnode->value);
					if (!pelement) FATAL(); // Can't happen.
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
	if (!element) FATAL();
	ConnectData* data = element->data;
	(data->numVisits)++;
	if (data->descendentsDone) return data->numDescendents; // Already found.

	// Find number of descendents.
	int descendents = 0;
	for (GNode* pnode = root->child; pnode; pnode = pnode->sibling) {
		if (eqstr("FAMS", pnode->tag)) { // Families this person is a spouse/parent in.
			GNodeIndexEl* felement = searchHashTable(index, pnode->value);
			if (!felement) FATAL(); // Can't happen.
			GNode* family = felement->root;
			for (GNode* fnode = family->child; fnode; fnode = fnode->sibling) {
				if (eqstr("CHIL", fnode->tag)) { // Children in this family are descendents.
					GNodeIndexEl* pelement = searchHashTable(index, fnode->value);
					if (!pelement) FATAL(); // Can't happen.
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
// NOTE: The numVisits field can be removed. It was added to validate the overall algorithm.
ConnectData* createConnectData(void) {
	ConnectData* data = (ConnectData*) stdalloc(sizeof(ConnectData));
	data->ancestorsDone = data->descendentsDone = false;
	data->numAncestors = data->numDescendents = data->numVisits = 0;
	return data;
}

// show is a static function passed to showGNodeIndex in order to show the ConnectData struct.
static void show(void* data) {
	ConnectData* connectData = data;
	if (connectData->ancestorsDone) printf("%d : ", connectData->numAncestors);
	else printf("- : ");
	if (connectData->descendentsDone) printf("%d : ", connectData->numDescendents);
	else printf("- : ");
	printf("%d\n", connectData->numVisits);
}

// debugGNodeIndex prints the contents of a GNodeIndex.
void debugGNodeIndex(GNodeIndex* index) {
	showGNodeIndex(index, show);
}
