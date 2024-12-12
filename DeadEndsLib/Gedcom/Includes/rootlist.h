// DeadEnds
//
// rootlist.h
//
// Created by Thomas Wetmore on 2 March 2024.
// Last changed on 11 December 2024.

#ifndef rootlist_h
#define rootlist_h

#include "list.h"
#include "gnode.h"
#include "gnodelist.h"
#include "integertable.h"
#include "errors.h"
#include "file.h"

typedef List GNodeList; // Forward reference.

// RootList is a List of GNode* roots.
typedef List RootList;

RootList *createRootList(void);  // Create a root list.
void insertInRootList(RootList*, GNode*);
RootList* getRootListFromFile(File*, IntegerTable*, ErrorLog*);
RootList* getRootListFromGNodeList(GNodeList*, String file, ErrorLog*);
void showRootList(RootList*);


#endif /* rootlist_h */
