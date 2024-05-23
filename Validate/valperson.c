// DeadEnds
//
// valperson.c contains functions that validate person records in a Database.
//
// Created by Thomas Wetmore on 17 December 2023.
// Last changed on 26 April 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"

static bool validatePerson(GNode*, Database*, ErrorLog*);

static bool debugging = false;

// validatePersonIndex validates the person index of a Database.
bool validatePersonIndex(Database* database, ErrorLog* errorLog) {
	bool valid = true;
	FORHASHTABLE(database->personIndex, element)
		GNode* person = ((RecordIndexEl*) element)->root;
		if (!validatePerson(person, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

// validatePerson validates a person record. Persons require at least one NAME and one SEX line
// with valid values. All FAMC and FAMS links must link to families that link back to the person.,
static bool validatePerson(GNode* person, Database* database, ErrorLog* errorLog) {
	String segment = database->lastSegment;
	int errorCount = 0;
	static char s[512];
	FORFAMCS(person, family, key, database) // Check FAMC links to families.
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
	FORFAMSS(person, family, key, database) // Check FAMS links to families.
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
	if (errorCount) return false;
	FORFAMCS(person, family, key, database) // Check FAMC links back to person.
		int numOccurrences = 0;
		FORCHILDREN(family, child, chilKey, count, database) // Find child that links to person.
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
	SexType sex = SEXV(person);
	FORFAMSS(person, family, key, database) // Check FAMS links back to person.
		GNode *parent = null;
		if (sex == sexMale) {
			parent = familyToHusband(family, database);
		} else if (sex == sexFemale) {
			parent = familyToWife(family, database);
		} else {
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

// personLineNumber returns the line number where a person was located in its Gedcom file.
int personLineNumber(GNode* person, Database* database) {
	RecordIndexEl *element = searchHashTable(database->personIndex, person->key);
	return element ? element->lineNumber : 0;
}
