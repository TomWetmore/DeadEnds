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

static bool validateSource(GNode*, Database*, ErrorLog*);
static bool validateEvent(GNode*, Database*, ErrorLog*);
static bool validateOther(GNode*, Database*, ErrorLog*);
static void validateReferences(Database *database, ErrorLog*);

int numValidations = 0;  //  DEBUG.

// validateDatabase validates a Database.
bool validateDatabase(Database* database, ErrorLog* errorLog) {
	ASSERT(database);
	bool isOkay = true;
	if (!validatePersonIndex(database, errorLog)) isOkay = false;
	if (!validateFamilyIndex(database, errorLog)) isOkay = false;
	if (!validateSourceIndex(database, errorLog)) isOkay = false;
	if (!validateEventIndex(database, errorLog)) isOkay = false;
	if (!validateOtherIndex(database, errorLog)) isOkay = false;
	//return isOkay;
	validateReferences(database, errorLog);
	if (lengthList(errorLog)) {
		printf("THERE WERE ERRORS DURING VALIDATION:\n");
		showErrorLog(errorLog);
		return false;
	}
	return true;
}

static bool validateSource(GNode* source, Database* database, ErrorLog* errorLog) {
	return true; // Write me.
}

bool validateSourceIndex(Database* database, ErrorLog* errorLog) {
	bool valid = true;
	FORHASHTABLE(database->sourceIndex, element)
		GNode* source = ((RecordIndexEl*) element)->root;
		int numErrors = lengthList(errorLog);
		if (!validateSource(source, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

bool validateEventIndex(Database* database, ErrorLog* errorLog) {
	bool valid = true;
	FORHASHTABLE(database->eventIndex, element)
		GNode* event = ((RecordIndexEl*) element)->root;
		int numErrors = lengthList(errorLog);
		if (!validateEvent(event, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

bool validateOtherIndex(Database* database, ErrorLog* errorLog) {
	bool valid = true;
	FORHASHTABLE(database->otherIndex, element)
		GNode* other = ((RecordIndexEl*) element)->root;
		int numErrors = lengthList(errorLog);
		if (!validateOther(other, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

extern String nameString(String);
static bool validateEvent(GNode* event, Database* database, ErrorLog* errorLog) {return true;}
static bool validateOther(GNode* other, Database* database, ErrorLog* errorLog) {return true;}

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
