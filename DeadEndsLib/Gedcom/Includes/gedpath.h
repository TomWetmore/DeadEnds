// gedpath.h
// DeadEnds
//
// gedpath.h is the header file for the GedPath feature.
//
// Created by Thomas Wetmore on 13 October 2024.
// Last changed on 17 October 2024.

#ifndef gedpath_h
#define gedpath_h

#include "standard.h"
#include "gnodelist.h"

// GedPath is an element in a Gedcom path list.
typedef struct GedPath {
	String tag; // Tag to match.
	bool all; // Make all matches.
	bool any; // Allow any tag.
	struct GedPath *next;
} GedPath;

GedPath* createGedPath(void); // Create an empty GedPath list node.
GedPath* buildGedPath(String); // Build a GedPath list from a String expression.
void traverseGedPath(GNode*, GedPath*, GNodeList*, int*); // Search a GNode tree with a GedPath.

void showGedPath(GedPath*); // Debug function that shows a GedPath.
int showGNodePath(GNode*, int level); // Debug function that shows GNode path from root to node.

#endif // gedpath_h
