//
//  stringSet.c
//  CloneOne
//
//  Created by Thomas Wetmore on 4/20/24.
//

#include "stringset.h"

static String getKey(void* element) { return (String) element; }

static int compare(String element1, String element2) { return strcmp(element1, element2); }

StringSet* createStringSet(void) {
	return createSet(getKey, compare, null);
}
