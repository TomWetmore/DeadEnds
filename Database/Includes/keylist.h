//
//  keylist.h -- Extension to List for the key lists kept in Database objects.
//  DeadEnds
//
//  Created by Thomas Wetmore on 15 February 2024.
//  Last changed by Thomas Wetmore on 20 February 2024.
//

#ifndef keylist_h
#define keylist_h

#include "list.h"
#include "database.h"

typedef List KeyList;  // List of root keys values. Where does the database info come from?

typedef List RootList;  // List of root GNodes.

KeyList *createKeyList(Database*);  // Create a KeyList with its compare and getkey functions.
void insertInKeyList(KeyList*, String);  // Insert a key into a KeyList.

RootList *createRootList(void);  // Create a RootList with its compare and getkey functions.
void insertInRootList(RootList*, GNode*);  // Insert a new root GNode* into a RootList.

#endif /* keylist_h */
