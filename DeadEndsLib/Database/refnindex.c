//
//  DeadEnds Library
//
//  refnindex.c has the functions that implement the reference index. Gedcom records can have
//  1 REFN nodes whose values give records unique identifiers.
//
//  Created by Thomas Wetmore on 16 December 2023.
//  Last changed on 23 August 2025.
//

#include "errors.h"
#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "integertable.h"
#include "recordindex.h"
#include "refnindex.h"
#include "validate.h"

static int numRefnIndexBuckets = 1024;

/// Searches a RefnIndex for a 1 REFN value and returns the key of the record with that value.
String searchRefnIndex(RefnIndex* index, String refn) {
	if (!index || !refn) return null;
	RefnIndexEl* el = (RefnIndexEl*) searchHashTable(index, refn);
	return el ? el->key : null;
}

/// Creates a new reference index entry.
RefnIndexEl *createRefnIndexEl(String refn, String key) {
	RefnIndexEl *el = (RefnIndexEl*) stdalloc(sizeof(RefnIndexEl));
	el->refn = refn;
	el->key = key;
	return el;
}

/// Shows a RefnIndexEl, for debugging
static void showRefnIndexEl(RefnIndexEl* element) {
  printf("refn: %s, key: %s\n", element->refn, element->key);
}

/// Shows a RefnIndex, for debugging.
void showRefnIndex(RefnIndex* index) {
    showHashTable(index, (void*) showRefnIndexEl);
}

/// Compares two record keys.
static int compare (String a, String b) {
	return strcmp(a, b);
}

/// Returns the key of a RefnIndexEl which is a 1 REFN value.
static String getKey(void* a) {
	return ((RefnIndexEl*) a)->refn;
}

/// Frees a RefnIndexEl.
static void delete(void* element) {
    stdfree(element);
}

/// Creates a RefnIndex.
RefnIndex *createRefnIndex(void) {
	return (RefnIndex*) createHashTable(getKey, compare, delete, numRefnIndexBuckets);
}

/// Deletes a RefnIndex.
void deleteRefnIndex(RefnIndex *index) {
	deleteHashTable(index);
}

/// Adds a new RefnIndexEl to a RefnIndex. Returns true on success and false if the REFN value is
// already in the table.
bool addToRefnIndex(RefnIndex *index, String refn, String key) {
	RefnIndexEl* element = createRefnIndexEl(refn, key);
	bool added = addToHashTableIfNew(index, element);
	if (added) return true;
	delete(element);
	return false;
}

/// Creates a reference index from the records in a Database It checks that REFN values are defined once.
RefnIndex* getReferenceIndex(RecordIndex *index, String fname, IntegerTable* keymap, ErrorLog* elog) {
    RefnIndex* refnIndex = createRefnIndex();
    FORHASHTABLE(index, element)
        GNode* root = (GNode*) element;
        GNode* refn = findTag(root->child, "REFN");
        while (refn) {
            String value = refn->value;
            if (value == null || strlen(value) == 0) {
                Error* err = createError(gedcomError, fname, LN(root, keymap, refn), "Missing REFN value");
                addErrorToLog(elog, err);
            } else if (!addToRefnIndex (refnIndex, value, root->key)) {
                Error *err = createError(gedcomError, fname, LN(root, keymap, refn), "REFN value already defined");
                addErrorToLog(elog, err);
            }
            refn = refn->sibling;
            if (refn && nestr(refn->tag, "REFN")) refn = null;
        }
    ENDHASHTABLE
    return refnIndex;
}
