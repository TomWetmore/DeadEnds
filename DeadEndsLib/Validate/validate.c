// DeadEnds
//
// validate.c has the functions that validate Gedcom records.
//
// Created by Thomas Wetmore on 12 April 2023.
// Last changed on 24 November 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "refnindex.h"

static bool validateSource(GNode*, Database*, ErrorLog*);
static bool validateEvent(GNode*, Database*, ErrorLog*);
static bool validateOther(GNode*, Database*, ErrorLog*);

int numValidations = 0; // DEBUG.

// validateSource validates a source record.
static bool validateSource(GNode* source, Database* database, ErrorLog* errorLog) {
	return true; // Write me.
}

// validateSourceIndex validates the sources in a Database's source index.
bool validateSourceIndex(Database* database, ErrorLog* errorLog) {
	bool isOkay = true;
	FORHASHTABLE(database->sourceIndex, element)
		GNode* source = ((RecordIndexEl*) element)->root;
		if (!validateSource(source, database, errorLog)) isOkay = false;
	ENDHASHTABLE
	return isOkay;
}

// validateEventIndex validates the events in a Database's event index.
bool validateEventIndex(Database* db, ErrorLog* elog) {
	bool isOkay = true;
	FORHASHTABLE(db->eventIndex, element)
		GNode* event = ((RecordIndexEl*) element)->root;
		if (!validateEvent(event, db, elog)) isOkay = false;
	ENDHASHTABLE
	return isOkay;
}

// validateOtherIndex validates the records in a Database's other index.
bool validateOtherIndex(Database* database, ErrorLog* errorLog) {
	bool isOkay = true;
	FORHASHTABLE(database->otherIndex, element)
		GNode* other = ((RecordIndexEl*) element)->root;
		if (!validateOther(other, database, errorLog)) isOkay = false;
	ENDHASHTABLE
	return isOkay;
}

// TODO: Write these.
static bool validateEvent(GNode* event, Database* db, ErrorLog* errorLog) {return true;}
static bool validateOther(GNode* other, Database* db, ErrorLog* errorLog) {return true;}


// validateReferences creates the reference index while validating the 1 REFN nodes in a Database.
void validateReferences(Database *db, ErrorLog* errorLog) {
	String fname = db->lastSegment;
	RefnIndex* refnIndex = createRefnIndex();
	FORHASHTABLE(db->recordIndex, element)
		GNode* root = ((RecordIndexEl*) element)->root;
		GNode* refn = findTag(root->child, "REFN");
		while (refn) {
			String value = refn->value;
			if (value == null || strlen(value) == 0) {
				Error* err = createError(gedcomError, fname, LN(root, db, refn),
										   "Missing REFN value");
				addErrorToLog(errorLog, err);
			} else if (!addToRefnIndex (refnIndex, value, root->key)) {
				Error *err = createError(gedcomError, fname, LN(root, db, refn),
										   "REFN value already defined");
				addErrorToLog(errorLog, err);
			}
			refn = refn->sibling;
			if (refn && nestr(refn->tag, "REFN")) refn = null;
		}
	ENDHASHTABLE
	db->refnIndex = refnIndex;
	return;
}

// rootLine returns the line number where a root node was located in its Gedcom file.
// Searches for the RecordIndexEl by the root's key and return the line property.
int rootLine(GNode* root, IntegerTable* keymap) {
	return root->key ? searchIntegerTable(keymap, root->key) : 0;
}

