//
//  DeadEnds Library
//
//  validate.c has the functions that validate Gedcom records.
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 3 June 2025.
//

#include "errors.h"
#include "gnode.h"
#include "gedcom.h"
#include "hashtable.h"
#include "integertable.h"
#include "lineage.h"
#include "recordindex.h"
#include "refnindex.h"
#include "validate.h"

static bool validateSource(GNode*, Database*, ErrorLog*);
static bool validateEvent(GNode*, Database*, ErrorLog*);
static bool validateOther(GNode*, Database*, ErrorLog*);

int numValidations = 0; // DEBUG.

// validateSource validates a source record. TODO: Write me.
static bool validateSource(GNode* source, Database* database, ErrorLog* elog) { return true; }

// validateSourceIndex validates the sources in a Database's source index. TODO: Write me.
bool validateSourceIndex(Database* database, ErrorLog* elog) { return true; }

// validateEventIndex validates the events in a Database's event index. TODO: Write me.
bool validateEventIndex(Database* db, ErrorLog* elog) { return true; }

// validateOtherIndex validates the records in a Database's other index. TODO: Write me.
bool validateOtherIndex(Database* database, ErrorLog* elog) { return true; }

// TODO: Write these.
static bool validateEvent(GNode* event, Database* db, ErrorLog* elog) {return true;}
static bool validateOther(GNode* other, Database* db, ErrorLog* elog) {return true;}

// getReferenceIndex creates the reference index while validating the 1 REFN nodes in a Database.
// TODO: This file isn't the right location for this function.
RefnIndex* getReferenceIndex(RecordIndex *index, String fname, IntegerTable* keymap, ErrorLog* elog) {
	RefnIndex* refnIndex = createRefnIndex();
	FORHASHTABLE(index, element)
		GNode* root = (GNode*) element;
		GNode* refn = findTag(root->child, "REFN");
		while (refn) {
			String value = refn->value;
			if (value == null || strlen(value) == 0) {
				Error* err = createError(gedcomError, fname, LN(root, keymap, refn),
										   "Missing REFN value");
				addErrorToLog(elog, err);
			} else if (!addToRefnIndex (refnIndex, value, root->key)) {
				Error *err = createError(gedcomError, fname, LN(root, keymap, refn),
										   "REFN value already defined");
				addErrorToLog(elog, err);
			}
			refn = refn->sibling;
			if (refn && nestr(refn->tag, "REFN")) refn = null;
		}
	ENDHASHTABLE
	return refnIndex;
}

// rootLine returns the line number where a root node was located in its Gedcom file.
// TODO: Should this function exist.
int rootLine(GNode* root, IntegerTable* keymap) {
	return root->key ? searchIntegerTable(keymap, root->key) : 0;
}
