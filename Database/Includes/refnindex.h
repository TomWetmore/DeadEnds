//
//  DeadEnds Project
//
//  refnindex.h -- Header file for the reference (REFN) index features.
//
//  Created on 17 December 2023.
//  Last changed on 1 January 2024.
//

#ifndef refnindex_h
#define refnindex_h
#include "hashtable.h"

typedef struct RefnIndexEl {
	String refn;  // Value of the REFN node.
	String key;  // Key of the record with the REFN node.
} RefnIndexEl;

typedef HashTable RefnIndex;

RefnIndexEl *createRefnIndexEl(String refn, String key);
RefnIndex *createRefnIndex(void);
void deleteRefnIndex(RefnIndex*);
bool insertInRefnIndex(RefnIndex*, String refn, String key);
String searchRefnIndex(RefnIndex*, String refn);
void showRefnIndex(RefnIndex*);

#endif // refnindex_h
