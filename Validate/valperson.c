// DeadEnds
//
// valperson.c contains functions that validate person records in a Database.
//
// Created by Thomas Wetmore on 17 December 2023.
// Last changed on 5 July 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"
#include "splitjoin.h"

static bool validatePerson(RecordIndexEl*, Database*, ErrorLog*);
extern bool importDebugging;

// validatePersons validates the persons in a Database.
void validatePersons(Database* database, ErrorLog* errlog) {
	int numPersonsValidated = 0;
	FORHASHTABLE(database->personIndex, element)
		validatePerson(element, database, errlog);
		numPersonsValidated++;
	ENDHASHTABLE
	if (importDebugging) printf("Number of persons validated is %d.\n", numPersonsValidated);
}

// validatePerson validates a person record. Persons require at least one NAME and one SEX line
// with valid values. All FAMC and FAMS links must link to families that link back to the person.
static bool validatePerson(RecordIndexEl* personEl, Database* database, ErrorLog* errorLog) {
	String segment = database->lastSegment;
	GNode* person = personEl->root;
	normalizePerson(person);
	int errorCount = 0;
	static char s[512];
	FORFAMCS(person, family, key, database) // Check FAMC links to families.
		if (!family) {
			int lineNumber = rootLineNumber(person, database);
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
				int lineNumber = rootLineNumber(person, database);
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
			int lineNumber = rootLineNumber(person, database);
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
					rootLineNumber(family, database),
					sex == sexMale ? "HUSB" : "WIFE",
					person->key,
					rootLineNumber(person, database));
			addErrorToLog(errorLog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
a:;
	ENDFAMSS


	//  Validate existance of NAME and SEX lines.
	//  Find all other links in the record and validate them.
	return errorCount == 0;
}
