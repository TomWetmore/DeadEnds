//
//  DeadEnds
//
//  validate.c -- Functions that validate Gedcom records.
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 16 November 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

bool validateDatabase(Database*, ErrorLog*);

static bool debugging = true;

static bool validatePersonIndex(Database*, ErrorLog*);
static bool validateFamilyIndex(Database*, ErrorLog*);
static void validatePerson(GNode*, Database*, ErrorLog*);
static void validateFamily(GNode*, Database*, ErrorLog*);
static void validateSource(GNode*, Database*, ErrorLog*);
static void validateEvent(GNode*, Database*, ErrorLog*);
static void validateOther(GNode*, Database*, ErrorLog*);

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
	return true;
}

//  validatePersonIndex -- Validate the person index of the current database.
//-------------------------------------------------------------------------------------------------
bool validatePersonIndex(Database *database, ErrorLog *errorLog)
{
	FORHASHTABLE(database->personIndex, element)
		GNode* person = ((RecordIndexEl*) element)->root;
		validatePerson(person, database, errorLog);
	ENDHASHTABLE
	return true;
}

//  validateFamilyIndex -- Validate the family index of the current database.
//-------------------------------------------------------------------------------------------------
bool validateFamilyIndex(Database *database, ErrorLog *errorLog)
{
	FORHASHTABLE(database->familyIndex, element)
		GNode *family = ((RecordIndexEl*) element)->root;
		validateFamily(family, database, errorLog);
	ENDHASHTABLE
	return true;
}

extern String nameString(String);

//  validatePerson -- Validate a person record. Check all FAMC and FAMS links to families.
//--------------------------------------------------------------------------------------------------
static void validatePerson(GNode *person, Database *database, ErrorLog *errorLog)
{
	if (debugging) { printf("Validating %s %s\n", person->key, NAME(person)->value); }

	//  Loop through the families the person is a child in.
	FORFAMCS(person, family, database)
		if (debugging) printf("Person is a child in family %s.\n", family->key);
		int numOccurrences = 0;
		FORCHILDREN(family, child, count, database)
			if (debugging) { printf("    Child %d: %s %s\n", count, child->key, NAME(child)->value); }
			if (person == child) numOccurrences++;
		ENDCHILDREN
		if (numOccurrences == 0) {
			addErrorToLog(errorLog, createError(linkageError, database->fileName, 0, "Child not found"));
		} else if (numOccurrences > 1) {
			addErrorToLog(errorLog, createError(linkageError, database->fileName, 0, "Too many children found"));
		}
	ENDFAMCS

	//  Loop through the families the person is a spouse in.
	if (debugging) printf("Doing the FAMS part.\n");
	SexType sex = SEXV(person);
	FORFAMILIES(person, family, database) {
		if (debugging) printf("  person should be a spouse in family %s.\n", family->key);
		GNode *parent = null;
		if (sex == sexMale) {
			parent = familyToHusband(family, database);
		} else if (sex == sexFemale) {
			parent = familyToWife(family, database);
		}
		ASSERT(person == parent);  // TODO: SHOULD NOT BE AS ASSERT HERE: SHOULD BE AN ERROR.
	} ENDFAMILIES

	if (debugging) printf("validate person: %s: \n", person->key);  //  REMOVE: FOR DEBUGGING.
	//  Validate existance of NAME and SEX lines.
	//  Find all other links in the record and validate them.
}

//  validateFamily -- Validate a family node tree record. Check all HUSB, WIFE and CHIL links
//    to persons.
//--------------------------------------------------------------------------------------------------
static void validateFamily(GNode *family, Database *database, ErrorLog* errorLog)
{
	if (debugging) {
		printf("validateFamily(%s)\n", family->key);
	}
	// For each HUSB line in the family (multiples in non-traditional cases).
	FORHUSBS(family, husband, database)
		// The husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, database)
			numValidations++;
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, database) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, database) {
			numValidations++;
			if (family == fam) numOccurences++;
		} ENDFAMSS
		ASSERT(numOccurences == 1);
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, n, database)
		int numOccurences = 0;
		FORFAMCS(child, fam, database)
			numValidations++;
			if (family == fam) numOccurences++;
		ENDFAMCS
		ASSERT(numOccurences == 1);
	ENDFAMCS
	//printf("validate family: %s\n", family->gKey);
	//  Validate existance of at least one of HUSB, WIFE, CHIL.
	//  Validate that the HUSBs are male.
	//  Validate that the WIFEs are female.
	//  Validate all other links.
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
