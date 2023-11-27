//
//  nodeutils.h
//  ImportGedcom
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changedd on 4 March 2023.
//

#ifndef nodeutils_h
#define nodeutils_h

// Prototypes of functions defined in nodeutils.c

GNode* unique_nodes(GNode*, bool);
GNode* union_nodes(GNode* node1, GNode* node2, bool kids, bool copy);
GNode* intersect_nodes(GNode*, GNode*, bool);
void classify_nodes(GNode**, GNode**, GNode**);
GNode* difference_nodes(GNode*, GNode*, bool);
bool value_in_nodes(GNode*, String);
bool equal_tree(GNode*, GNode*);
bool equal_node(GNode*, GNode*);
bool iso_list(GNode*, GNode*);
bool equal_nodes(GNode*, GNode*, bool, bool);
bool iso_nodes(GNode*, GNode*, bool, bool);


#endif /* nodeutils_h */
