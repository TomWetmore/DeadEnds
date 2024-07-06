// DeadEnds
//
// validate.c has the functions that validate Gedcom records.
//
// Created by Thomas Wetmore on 12 April 2023.
// Last changed on 21 May 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "refnindex.h"

//static bool debugging = true;

static bool validateSource(GNode*, Database*, ErrorLog*);
static bool validateEvent(GNode*, Database*, ErrorLog*);
static bool validateOther(GNode*, Database*, ErrorLog*);
static bool validateRefns(Database *database, ErrorLog*);

int numValidations = 0; // DEBUG.

// validateDatabase validates a Database.
bool validateDatabase(Database* database, ErrorLog* errorLog) {
	ASSERT(database);
	bool isOkay = true;
	//if (!validatePersonIndex(database, errorLog)) isOkay = false;
	//if (!validateFamilyIndex(database, errorLog)) isOkay = false;
	//if (!validateSourceIndex(database, errorLog)) isOkay = false;
	//if (!validateEventIndex(database, errorLog)) isOkay = false;
	//if (!validateOtherIndex(database, errorLog)) isOkay = false;
	//if (!validateNameIndex(database, errorLog)) isOkay = false;
	//if (!validateRefns(database, errorLog)) isOkay = false;
	return isOkay;
}

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
bool validateEventIndex(Database* database, ErrorLog* errorLog) {
	bool isOkay = true;
	FORHASHTABLE(database->eventIndex, element)
		GNode* event = ((RecordIndexEl*) element)->root;
		if (!validateEvent(event, database, errorLog)) isOkay = false;
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

static bool validateEvent(GNode* event, Database* database, ErrorLog* errorLog) {return true;}
static bool validateOther(GNode* other, Database* database, ErrorLog* errorLog) {return true;}

#define LN(root, database, node) rootLineNumber(root, database) + countNodesBefore(node)

// validateReferences creates the reference index while validating the 1 REFN nodes in a Database.
void validateReferences(Database *database, ErrorLog* errorLog) {
	String fname = database->lastSegment;
	RefnIndex* refnIndex = createRefnIndex();
	FORHASHTABLE(database->recordIndex, element)
		GNode* root = ((RecordIndexEl*) element)->root;
		GNode* refn = findTag(root->child, "REFN");
		while (refn) {
			String refString = refn->value;
			if (refString == null || strlen(refString) == 0) {
				Error* error = createError(gedcomError, fname, LN(root, database, refn),
									   "Missing REFN value");
			addErrorToLog(errorLog, error);
		} else if (!addToRefnIndex (refnIndex, refString, root->key)) {
			Error *error = createError(gedcomError, fname, LN(root, database, refn),
									   "REFN value already defined");
			addErrorToLog(errorLog, error);
		}
		refn = refn->sibling;
		if (refn && nestr(refn->tag, "REFN")) refn = null;
	}
	ENDHASHTABLE
	database->refnIndex = refnIndex;
	return;
}

// rootLineNumber returns the line number where a root node was located in its Gedcom file.
int rootLineNumber(GNode* root, Database* database) {
	RecordIndexEl *el = searchHashTable(database->recordIndex, root->key);
	return el ? el->line : 0;
}

