//
//  DeadEnds
//
//  valperson.c -- Functions that validate the person records in a Database.
//
//  Created by Thomas Wetmore on 17 December 2023.
//  Last changed on 29 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"

static bool validatePerson(GNode*, Database*, ErrorLog*);

static bool debugging = false;

//  validatePersonIndex -- Validate the person index of a database.
//-------------------------------------------------------------------------------------------------
bool validatePersonIndex(Database *database, ErrorLog *errorLog)
{
	bool valid = true;
	FORHASHTABLE(database->personIndex, element)
		GNode* person = ((RecordIndexEl*) element)->root;
		int numErrors = lengthList(errorLog);
		if (debugging) printf("Start validating %s %s\n", person->key, NAME(person)->value);
		if (!validatePerson(person, database, errorLog)) valid = false;
		if (debugging && lengthList(errorLog) > numErrors) {
			printf("There were %d errors validating %s\n", lengthList(errorLog) - numErrors, person->key);
			numErrors = lengthList(errorLog);
		}
		if (debugging) printf("Done validating %s %s\n", person->key, NAME(person)->value);
	ENDHASHTABLE
	return valid;
}

//  validatePerson -- Validate a person record. Persons do not require NAME or SEX lines, but
//    if there is a SEX line its value is checked. Check all FAMC and FAMS links to families,
//    and that the families have the correct links back.
//--------------------------------------------------------------------------------------------------
static bool validatePerson(GNode *person, Database *database, ErrorLog *errorLog)
{
	String segment = database->lastSegment;
	int errorCount = 0;
	static char s[512];

	//  Check that the FAMC and FAMS nodes link to existing families.
	FORFAMCS(person, family, key, database)
		if (!family) {
			int lineNumber = personLineNumber(person, database);
			sprintf(s, "INDI %s (line %d): FAMC %s (line %d) does not exist.",
					person->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDFAMCS
	FORFAMSS(person, family, key, database)
		if (!family) {
				int lineNumber = personLineNumber(person, database);
				sprintf(s, "INDI %s (line %d): FAMS %s (line %d) does not exist.",
						person->key,
						lineNumber,
						key,
						lineNumber + countNodesBefore(__node));
				addErrorToLog(errorLog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDFAMSS
	if (debugging && errorCount) printf("Returning from valpperson early.\n");
	if (errorCount) return false;

	// Loop through the families the person is a child in. Be sure the family has a CHIL link back.
	FORFAMCS(person, family, key, database)
		//if (debugging) printf("DB: Person is a child in family %s.\n", key);
		int numOccurrences = 0;
		//  Loop through the children in a family the person should be a child in.
		FORCHILDREN(family, child, chilKey, count, database)
			//if (debugging) { printf("DB: Child %d: %s %s\n", count, child->key, NAME(child)->value); }
			if (person == child) numOccurrences++;
		ENDCHILDREN
		if (numOccurrences == 0) {
			addErrorToLog(errorLog, createError(linkageError, segment, 0, "Child not found"));
			errorCount++;
		} else if (numOccurrences > 1) {
			addErrorToLog(errorLog, createError(linkageError, segment, 0, "Too many children found"));
			errorCount++;
		}
	ENDFAMCS
	if (errorCount) return false;

	// Loop through the families the person is a spouse in. Be sure the family has a HUSB or WIFE
	// link back.
	SexType sex = SEXV(person);
	FORFAMSS(person, family, key, database)
		//if (debugging) printf("  person should be a spouse in family %s.\n", family->key);
		GNode *parent = null;
		if (sex == sexMale) {
			parent = familyToHusband(family, database);
		} else if (sex == sexFemale) {
			parent = familyToWife(family, database);
		} else {
			//  Parents require a SEX node with M or F value.
			int lineNumber = personLineNumber(person, database);
			sprintf(s, "INDI %s (line %d) with FAMS %s (line %d) link has no sex value.",
					person->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, 0, s));
			errorCount++;
			goto a;
		}
		if (person != parent) {
			sprintf(s, "FAM %s (line %d) should have %s link to INDI %s (line %d).",
					key,
					familyLineNumber(family, database),
					sex == sexMale ? "HUSB" : "WIFE",
					person->key,
					personLineNumber(person, database));
			addErrorToLog(errorLog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
a:;
	ENDFAMSS

	//  Validate existance of NAME and SEX lines.
	//  Find all other links in the record and validate them.
	return errorCount == 0;
}

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
