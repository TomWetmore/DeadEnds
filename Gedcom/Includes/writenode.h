//
//  writenode.h
//  DeadEnds
//
//  Created by Thomas Wetmore on 2 May 2023.
//  Last changed on 27 October 2023.
//

#ifndef writenode_h
#define writenode_h

#include <stdio.h>

void writeGNodes(FILE*, int level, GNode*, bool indent, bool kids, bool sibs);
void writeGNode(FILE*, int level, GNode*, bool indent);

#endif /* writenode_h */
