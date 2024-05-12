// DeadEnds
//
// validate.c has the functions that validate Gedcom records.
//
// Created by Thomas Wetmore on 12 April 2023.
// Last changed on 9 May 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "refnindex.h"

bool validateDatabase(Database*, ErrorLog*);

//static bool debugging = true;

static void validateSource(GNode*, Database*, ErrorLog*);
static void validateEvent(GNode*, Database*, ErrorLog*);
static void validateOther(GNode*, Database*, ErrorLog*);
static void validateReferences(Database *database, ErrorLog*);

int numValidations = 0;  //  DEBUG.

// validateDatabase validates a Database.
bool validateDatabase(Database* database, ErrorLog* errorLog) {
	ASSERT(database);
	validatePersonIndex(database, errorLog);
	validateFamilyIndex(database, errorLog);
	//if (!validateIndex(database->sourceIndex)) isOkay = false;
	//if (!validateIndex(database->eventIndex)) isOkay = false;
	//if (!validateIndex(database->otherIndex)) isOkay = false;
	//return isOkay;
	validateReferences(database, errorLog);
	if (lengthList(errorLog)) {
		printf("THERE WERE ERRORS DURING VALIDATION:\n");
		showErrorLog(errorLog);
		return false;
	}
	return true;
}

extern String nameString(String);
void validateSource(GNode* source, Database* database, ErrorLog* errorLog) {}
void validateEvent(GNode* event, Database* database, ErrorLog* errorLog) {}
void validateOther(GNode* other, Database* database, ErrorLog* errorLog) {}

static GNode* getFamily(String key, RecordIndex* index) {
	GNode* root = searchRecordIndex(index, key);
	return root && recordType(root) == GRFamily ? root : null;
}

static GNode* getPerson(String key, RecordIndex* index) {
	GNode* root = searchRecordIndex(index, key);
	return root && recordType(root) == GRPerson ? root : null;
}

#define LN(person, database, node)\
	personLineNumber(person, database) + countNodesBefore(node)

// validateReferencesInIndex validates the 1 REFN nodes in a RecordIndex.
static void validateReferencesInIndex(Database *database, RecordIndex* recordIndex,
									  List *errorLog) {
	FORHASHTABLE(recordIndex, element)
	RefnIndex* refnIndex = database->refnIndex;
	GNode* root = ((RecordIndexEl*) element)->root;
	GNode* refn = findTag(root->child, "REFN");
	while (refn) {
		String refString = refn->value;
		if (refString == null || strlen(refString) == 0) {
			Error *error = createError(gedcomError, database->lastSegment, LN(root, database, refn),
									   "Missing REFN value");
			addErrorToLog(errorLog, error);
		} else if (!addToRefnIndex (refnIndex, refString, root->key)) {
			Error *error = createError(gedcomError, database->lastSegment, LN(root, database, refn),
									   "REFN value already in index");
			addErrorToLog(errorLog, error);
		}
		refn = refn->sibling;
		if (refn && nestr(refn->tag, "REFN")) refn = null;
	}
	ENDHASHTABLE
}

// validateReferences validates the 1 REFN nodes in a Database.
static void validateReferences(Database *database, ErrorLog* errorLog) {
	String segment = database->lastSegment;
	RefnIndex *refnIndex = database->refnIndex;
	validateReferencesInIndex(database, database->personIndex, errorLog);
	validateReferencesInIndex(database, database->familyIndex, errorLog);
	validateReferencesInIndex(database, database->sourceIndex, errorLog);
	validateReferencesInIndex(database, database->eventIndex, errorLog);
	validateReferencesInIndex(database, database->otherIndex, errorLog);
}
