//  testset.c -- Test program for the Set data type.
//
//  Created by Thomas Wetmore on 3 November 2023.
//  Last changed on 25 March 2024.

#include <stdio.h>
#include "set.h"
#include "gedcom.h"

int compare(String element1, String element2) {
	return compareRecordKeys(element1, element2);
}

String getKey(void* element) {
	return (String) element;
}

int notmain (void)
{
	// Create a Set.
	Set *set = createSet(getKey, compare, null);
	String keys[] = { "I1", "I1", "I2", "I4", "I10", "I21", "I4", "I5", "I300", "I299" };
	int n = sizeof(keys)/sizeof(String);
	for (int i = 0; i < n; i++) {
		printf("add %s\n", keys[i]);
		addToSet(set, keys[i]);
		showSet(set, null);
		printf("\n");
	}
	return 0;
}
