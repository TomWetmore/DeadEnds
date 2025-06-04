//
//  DeadEnds Library
//
//  rootlist.h
//
//  Created by Thomas Wetmore on 2 March 2024.
//  Last changed on 4 June 2025.

#ifndef rootlist_h
#define rootlist_h

#include "standard.h"

typedef struct GNode GNode;
typedef struct List List;
typedef List GNodeList;
typedef List ErrorLog;

// RootList is a List of GNode* roots.
typedef List RootList;

RootList *createRootList(void);  // Create a root list.
void deleteRootList(RootList*);
void insertInRootList(RootList*, GNode*);
//RootList* getRootListFromFile(File*, IntegerTable*, ErrorLog*);
RootList* getRootListFromGNodeList(GNodeList*, String file, ErrorLog*);
void showRootList(RootList*);


#endif /* rootlist_h */
