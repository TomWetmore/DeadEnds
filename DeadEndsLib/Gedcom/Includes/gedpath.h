//
//  DeadEnds Library
//
//  gedpath.h is the header file for the GedPath feature.
//
//  Created by Thomas Wetmore on 13 October 2024.
//  Last changed on 4 June 2025.
//

#ifndef gedpath_h
#define gedpath_h

#include "standard.h"

typedef struct GNode GNode;
typedef struct List List;
typedef List GNodeList;

// GedPath is an element in a Gedcom path list.
typedef struct GedPath GedPath;
struct GedPath {
	String tag; // Tag to match.
	bool all; // Make all matches.
	bool any; // Allow any tag.
	GedPath *next;
};

GedPath* createGedPath(void); // Create empty GedPath.
GedPath* buildGedPath(String); // Build GedPath from an expression.
void traverseGedPath(GNode*, GedPath*, GNodeList*, int*); // Search a GNode tree with GedPath.

void showGedPath(GedPath*); // Debug function to show GedPath.
int showGNodePath(GNode*, int level); // Debug function to show path from root to node.

#endif // gedpath_h
