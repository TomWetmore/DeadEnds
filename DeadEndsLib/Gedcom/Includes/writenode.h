// DeadEnds
//
// writenode.h
//
// Created by Thomas Wetmore on 2 May 2023.
// Last changed on 31 May 2024.

#ifndef writenode_h
#define writenode_h

#include <stdio.h>

void writeGNodeRecord(FILE*, GNode*, bool indent);
void writeGNodes(FILE*, int level, GNode*, bool indent, bool kids, bool sibs);
void writeGNode(FILE*, int level, GNode*, bool indent);

#endif // writenode_h
