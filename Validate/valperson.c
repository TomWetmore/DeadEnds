//
//  DeadEnds
//
//  valperson.c -- Functions that validate person records.
//
//  Created by Thomas Wetmore on 17 December 2023.
//  Last changed on 18 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"

static bool validatePerson(GNode*, Database*, ErrorLog*);

static bool debugging = true;

//  validatePersonIndex -- Validate the person index of the current database.
//-------------------------------------------------------------------------------------------------
bool validatePersonIndex(Database *database, ErrorLog *errorLog)
{
	bool valid = true;
	FORHASHTABLE(database->personIndex, element)
		GNode* person = ((RecordIndexEl*) element)->root;
		int errors = lengthList(errorLog);
		if (debugging) printf("Start validating %s %s\n", person->key, NAME(person)->value);
		if (!validatePerson(person, database, errorLog)) valid = false;
		if (debugging && lengthList(errorLog) > errors)
			printf("There were %d errors validating %s\n", lengthList(errorLog) - errors, person->key);
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
	if (debugging) printf("Validating %s %s\n", person->key, NAME(person)->value);
	String segment = database->lastSegment;
	int errorCount = 0;
	static char s[4096];

	//  Make sure all FAMC and FAMS values link to families.
	FORFAMCS(person, family, key, database)
		if (!family) {
			int lineNumber = personLineNumber(person, database);
			sprintf(s, "INDI %s (line %d): FAMC %s (line %d) does not refer to a family.",
					person->key, lineNumber, key, lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDFAMCS
	FORFAMSS(person, family, key, database)
		if (!family) {
				int lineNumber = personLineNumber(person, database);
				sprintf(s, "INDI %s (line %d): FAMS %s (line %d) does not refer to a family.",
						person->key, lineNumber, key, lineNumber + countNodesBefore(__node));
				addErrorToLog(errorLog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDFAMSS
	if (errorCount) return false;

	// Loop through the families the person is a child in. Be sure the family has a CHIL link back.
	FORFAMCS(person, family, key, database)
		if (debugging) printf("DB: Person is a child in family %s.\n", key);
		int numOccurrences = 0;
		//  Loop through the children in a family the person should be a child in.
		FORCHILDREN(family, child, count, database)
			if (debugging) { printf("DB: Child %d: %s %s\n", count, child->key, NAME(child)->value); }
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
	if (debugging) printf("Doing the FAMS part.\n");
	SexType sex = SEXV(person);
	FORFAMILIES(person, family, database) {
		if (debugging) printf("  person should be a spouse in family %s.\n", family->key);
		GNode *parent = null;
		if (sex == sexMale) {
			parent = familyToHusband(family, database);
		} else if (sex == sexFemale) {
			parent = familyToWife(family, database);
		} else {
			addErrorToLog(errorLog, createError(linkageError, segment, 0, "Person used as spouse has no sex."));
			errorCount++;
		}
		if (person != parent) {
			addErrorToLog(errorLog, createError(linkageError, segment, 0, "Family does not link back to spouse."));
			errorCount++;
		}
	} ENDFAMILIES

	//  Validate existance of NAME and SEX lines.
	//  Find all other links in the record and validate them.
	return errorCount == 0;
}
