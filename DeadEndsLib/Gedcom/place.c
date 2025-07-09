// DeadEnds
//
// place.c has the functions that handle Gedcom place values.
//
// Created by Thomas Wetmore on 12 February 2024.
// Last changed on 14 May 2025.

#include <stdio.h>
#include "standard.h"
#include "list.h"

static bool inString (int chr, String str);
void valueToList (String str, List *list, String dlm);

// placeToList converts a Gedcom PLAC value to a list of phrases. A phrase is a string that
// occurs before the first comma, after the last comma, or between successive commas. White
// space is trimmed from the phrases.
void placeToList(String place, List *list) {
	valueToList(place, list, ",");
}

// valueToList converts a String to a list of trimmed phrases split by delimiters.
// Each phrase is trimmed of leading/trailing whitespace and added to the list.
// Returns true on success.
void valueToList(String str, List* list, String dlm) {
    emptyList(list); // Empty list before use.
    if (!str || *str == 0 || !list) return;
    // Create heap buffer to hold copy of string.
    int len = (int) strlen(str);
    String buf = (String) stdalloc(len + 2);
    strcpy(buf, str);
    buf[len + 1] = 0;  // Safety guard.
    int phraseCount = 1; // Split string in-place with nulls at delimiter locations.
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

        while (*p && chartype(*p) == WHITE) p++;
        char* q = p + strlen(p) - 1;
        while (q > p && chartype(*q) == WHITE) *q-- = '\0';

        appendToList(list, strsave(p)); // Add to list; empty strings are valid.
        p = n;
    }
    stdfree(buf);  // Free buffer.
    return;
}

static bool inString (int chr, String str)
{
	while (*str && chr != *str)	str++;
	return *str != 0;
}

