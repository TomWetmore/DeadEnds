//
//  pvaluelist.c
//  Library
//
//  Created by Thomas Wetmore on 27 July 2025.
//  Last changed on 15 August 2025.
//

#include "list.h"
#include "pvalue.h"
#include "pvaluelist.h"

/// Keeps track of PValueList allocs and frees. For debugging.
static int pvlistAllocs = 0;
static int pvlistFrees = 0;

/// Frees an element of a PValueList.
static void delete(void* element) { freePValue(element); }

/// Creates a PValueList.
PValueList* createPValueList(void) {
    List* list = createList(null, null, delete, false);
    pvlistAllocs++;
    return list;
}

/// Deletes a PValueList
void deletePValueList(PValueList* list) {
    deleteList(list); // TODO: MAKE SURE THE DELETE FUNCTION FOR THE LIST IS CORRECT.
    pvlistFrees++;
}
