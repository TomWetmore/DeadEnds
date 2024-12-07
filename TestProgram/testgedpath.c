// testgedpath.c
// TestProgram
//
// Created by Thomas Wetmore on 15 October 2024.
// Last changed on 16 October 2024.

#include "database.h"
#include "gedpath.h"
#include "gnodelist.h"
#include "utils.h"

void testGedPaths(Database* database, int testNumber) {
	printf("%d: START OF TEST GED PATHS: %2.3f\n", testNumber, getMseconds());
	GedPath* path = buildGedPath("INDI->ANY*->DATE*");
	showGedPath(path);

	// Get the first person in the database.
	GNode* person = searchRecordIndex(database->recordIndex, "@I1@");
	// Get the GNodes referred to by the GedPath.
	int count = 0;
	GNodeList* matches = createGNodeList();
	traverseGedPath(person, path, matches, &count);
	//showGNodeTree(person);
	int ii = 1;
	FORLIST(matches, element)
		printf("Match number %d\n", ii++);
		GNode* node = (GNode*) element;
		showGNode(0, node);
		showGNodePath(node, 0);
	ENDLIST
	printf("%d: END OF TEST GED PATHS: %2.3f\n", testNumber, getMseconds());

}
