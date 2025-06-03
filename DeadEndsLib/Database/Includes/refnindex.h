//  DeadEnds Library
//
//  refnindex.h is the hearder file for the reference (REFN) index features.
//
//  Created on 17 December 2023.
//  Last changed on 3 June 2025.

#ifndef refnindex_h
#define refnindex_h

#include "standard.h"

// RefnIndexEl is the type for elements in RefnIndexes. refn is the value of a 1 REFN Gedcom line
// and key is the tag of the record it is in. Each refn key must be unique.
typedef struct RefnIndexEl {
	String refn;
	String key;
} RefnIndexEl;

// RefnIndex is a HashTable holding RefnIndexEls.
typedef HashTable RefnIndex;

// Interface to RefnIndexes.
RefnIndexEl *createRefnIndexEl(String refn, String key);
RefnIndex *createRefnIndex(void);
void deleteRefnIndex(RefnIndex*);
bool addToRefnIndex(RefnIndex*, String refn, String key);
String searchRefnIndex(RefnIndex*, String refn);
void showRefnIndex(RefnIndex*);

#endif // refnindex_h
