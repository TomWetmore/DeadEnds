// DeadEnds
//
// place.c has the functions that handle Gedcom place values.
//
// Created by Thomas Wetmore on 12 February 2024.
// Last changed on 19 August 2024.

#include <stdio.h>
#include "standard.h"
#include "list.h"

static bool inString (int chr, String str);
bool valueToList (String str, List *list, String dlm);

// placeToList converts a Gedcom PLAC value to a list of phrases. A phrase is a string that
// occurs before the first comma, after the last comma, or between successive commas. White
// space is trimmed from the phrases.
bool placeToList(String place, List *list) {
	return valueToList(place, list, ",");
}

// valueToList converts a String to a list of trimmed phrases split by delimiters.
// Each phrase is trimmed of leading/trailing whitespace and added to the list.
// Returns true on success.
bool valueToList(String str, List* list, String dlm) {
    if (!str || *str == 0 || !list) return false;

    emptyList(list); // Clear the list before use

    int len = (int) strlen(str);
    String buf = (String) stdalloc(len + 2);  // Extra space for null terminator guard
    strcpy(buf, str);
    buf[len + 1] = 0;  // Safety guard for final string

    // Split the string in-place using nulls at delimiter locations
    int phraseCount = 1;
    for (char* p = buf; *p; p++) {
        if (inString(*p, dlm)) {
            *p = '\0';
            phraseCount++;
        }
    }

    // Extract and trim each phrase
    char* p = buf;
    for (int i = 0; i < phraseCount; i++) {
        char* n = p + strlen(p) + 1;

        // Trim leading whitespace
        while (*p && chartype(*p) == WHITE) p++;

        // Trim trailing whitespace
        char* q = p + strlen(p) - 1;
        while (q > p && chartype(*q) == WHITE)
            *q-- = '\0';

        // Add to list (even empty strings are valid phrases)
        appendToList(list, strsave(p));
        p = n;
    }

    stdfree(buf);  // Free the temporary buffer
    return true;
}
static bool inString (int chr, String str)
{
	while (*str && chr != *str)
		str++;
	return *str != 0;
}

