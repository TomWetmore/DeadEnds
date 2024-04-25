// DeadEnds
//
// rootlist.c holds the RootList datatype. A RootList is a List whose elements are pointers to
// the root GNodes of Gedcom records.
//
// Created by Thomas Wetmore on 2 March 2024.
// Last changed on 22 April 2024.

#include "rootlist.h"
#include "gnode.h"

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

// insertInRootList adds a GNode* to a RootList.
void insertInRootList(RootList* list, GNode* root) {
	int index = -1;
	if (findInList(list, list->getKey(root), &index)) {
		printf("THERE IS A ERROR -- DUPLICATE ROOT KEYS\n");
		return;
	}
	insertInList(list, root, index);
}
