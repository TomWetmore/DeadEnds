// DeadEnds
//
// rootlist.h
//
// Created by Thomas Wetmore on 2 March 2024.
// Last changed on 18 April 2024.

#ifndef rootlist_h
#define rootlist_h

#include "list.h"
#include "gnode.h"

// RootList is a List of GNode* roots.
typedef List RootList;

RootList *createRootList(void);  // Create a root list.
void insertInRootList(RootList*, GNode*);


#endif /* rootlist_h */
