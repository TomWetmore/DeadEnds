// DeadEnds
//
// stringset.h is the header file for the StringSet data type.
//
// Created by Thomas Wetmore on 20 April 2024.
// Last changed on 9 December 2024.

#ifndef stringset_h
#define stringset_h

#include "set.h"

typedef Set StringSet;

StringSet* createStringSet(void);
void deleteStringSet(StringSet*, bool del);
void showStringSet(StringSet*);


#endif // stringset_h
