//
//  DeadEnds Library
//
//  writenode.h
//
//  Created by Thomas Wetmore on 2 May 2023.
//  Last changed on 4 June 2025.

#ifndef writenode_h
#define writenode_h

void writeGNodeRecord(FILE*, GNode*, bool indent);
void writeGNodes(FILE*, int level, GNode*, bool indent, bool kids, bool sibs);
void writeGNode(FILE*, int level, GNode*, bool indent);

#endif // writenode_h
