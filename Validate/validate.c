//
//  DeadEnds
//
//  validate.c -- Functions that validate Gedcom records.
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 17 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

bool validateDatabase(Database*, ErrorLog*);

static bool debugging = true;

//static bool validateFamilyIndex(Database*, ErrorLog*);
static bool validateFamily(GNode*, Database*, ErrorLog*);
static void validateSource(GNode*, Database*, ErrorLog*);
static void validateEvent(GNode*, Database*, ErrorLog*);
static void validateOther(GNode*, Database*, ErrorLog*);

//  personLineNumber -- Given a person root node, return its location (line number) in the
//    original Gedcom file. Uses searchHashTable to get the RecordIndexEl of the record and
//    takes the line number from there. Obviously won't work correctly for records that
//    arrived from another source.
//--------------------------------------------------------------------------------------------------
int personLineNumber (GNode *person, Database* database)
{
	RecordIndexEl *element = searchHashTable(database->personIndex, person->key);
	if (!element) return 0;  // Should not happen.
	return element->lineNumber;  // Assume this is zero for records not from Gedcom files.
}

int numValidations = 0;  //  DEBUG.

//  validateDatabase
//--------------------------------------------------------------------------------------------------
bool validateDatabase(Database *database, ErrorLog *errorLog)
{
	ASSERT(database);
	validatePersonIndex(database, errorLog);
	//validateFamilyIndex(database, errorLog);
	//if (!validateIndex(database->sourceIndex)) isOkay = false;
	//if (!validateIndex(database->eventIndex)) isOkay = false;
	//if (!validateIndex(database->otherIndex)) isOkay = false;
	//return isOkay;
	if (lengthList(errorLog)) {
		printf("THERE WERE ERRORS DURING VALIDATION:\n");
		showErrorLog(errorLog);
		return false;
	}
	return true;
}

//  validateFamilyIndex -- Validate the family index of the current database.
//-------------------------------------------------------------------------------------------------
bool validateFamilyIndex(Database *database, ErrorLog *errorLog)
{
	bool valid = true;
	FORHASHTABLE(database->familyIndex, element)
		GNode *family = ((RecordIndexEl*) element)->root;
		if (!validateFamily(family, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

extern String nameString(String);



//  validateFamily -- Validate a family node tree record. Check all HUSB, WIFE and CHIL links
//    to persons.
//--------------------------------------------------------------------------------------------------
static bool validateFamily(GNode *family, Database *database, ErrorLog* errorLog)
{
	if (debugging) printf("validateFamily(%s)\n", family->key);
	String segment = database->lastSegment;
	// For each HUSB line in the family (multiples in non-traditional cases).
	FORHUSBS(family, husband, database)
		// The husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, key, database)
			numValidations++;
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, database) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, key, database) {
			numValidations++;
			if (family == fam) numOccurences++;
		} ENDFAMSS
		ASSERT(numOccurences == 1);
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, n, database)
		int numOccurences = 0;
		FORFAMCS(child, fam, key, database)
			numValidations++;
			if (family == fam) numOccurences++;
		ENDFAMCS
		ASSERT(numOccurences == 1);
	ENDFAMCS

	bool hasHusb = HUSB(family) != null;
	bool hasWife = WIFE(family) != null;
	bool hasChild = CHIL(family) != null;
	if (!(hasHusb || hasWife || hasChild)) {
		addErrorToLog(errorLog, createError(linkageError, segment, 0, "Family has no HUSB, WIFE or CHIL links"));
	}
	//printf("validate family: %s\n", family->gKey);
	//  Validate existance of at least one of HUSB, WIFE, CHIL.
	//  Validate that the HUSBs are male.
	//  Validate that the WIFEs are female.
	//  Validate all other links.
	return true;  // TODO: Deal with the errors properly.
}

void validateSource(GNode *source, Database *database, ErrorLog* errorLog) {}

void validateEvent(GNode *event, Database *database, ErrorLog* errorLog) {}

void validateOther(GNode *other, Database *database, ErrorLog* errorLog) {}


static GNode *getFamily(String key, RecordIndex *index)
{
	GNode *root = searchRecordIndex(index, key);
	return root && recordType(root) == GRFamily ? root : null;
}

static GNode *getPerson(String key, RecordIndex *index)
{
	GNode *root = searchRecordIndex(index, key);
	return root && recordType(root) == GRPerson ? root : null;
}
