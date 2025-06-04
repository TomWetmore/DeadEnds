//
//  DeadEnds Library
//  nodeutils.h
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changedd on 4 June 2025.
//

#ifndef nodeutils_h
#define nodeutils_h

// Prototypes of functions defined in nodeutils.c

GNode* uniqueNodes(GNode*, bool);
GNode* unionNodes(GNode* node1, GNode* node2, bool kids, bool copy);
GNode* intersect_nodes(GNode*, GNode*, bool);
void classifyNodes(GNode**, GNode**, GNode**);
GNode* difference_nodes(GNode*, GNode*, bool);
bool valueInNodes(GNode*, String);
bool equalTree(GNode*, GNode*);
bool equalNode(GNode*, GNode*);
bool isoList(GNode*, GNode*);
bool equalNodes(GNode*, GNode*, bool, bool);
bool isoGNodes(GNode*, GNode*, bool, bool);

#endif // nodeutils_h
