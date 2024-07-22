// DeadEnds
//
// valperson.c contains functions that validate person records in a Database.
//
// Created by Thomas Wetmore on 17 December 2023.
// Last changed on 10 July 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"
#include "splitjoin.h"

#define LC(line, node) (line + countNodesBefore(node))

static bool validatePerson(RecordIndexEl*, Database*, ErrorLog*);
static bool hasValidNameGNode(GNode* root, GNode** pname);
static bool hasValidSexGNode(GNode* root, GNode** psex);
extern bool importDebugging;

// validatePersons validates the persons in a Database.
void validatePersons(Database* database, ErrorLog* errlog) {
	int numPersonsValidated = 0;
	if (importDebugging)
		printf("Before validating, there are %d persons.\n", sizeHashTable(database->personIndex));
	FORHASHTABLE(database->personIndex, element)
		validatePerson(element, database, errlog);
		numPersonsValidated++;
	ENDHASHTABLE
	if (importDebugging) printf("Number of persons validated is %d.\n", numPersonsValidated);
}

// validatePerson validates a person record. Persons require at least one NAME and one SEX line
// with valid values. All FAMC and FAMS links must link to families that link back to the person.
//
// Notes on generating errors. A String buffer, s, exists. The personEl argument links to both
// the the root node and the location of the root node in the original Gedcom file.
static bool validatePerson(RecordIndexEl* personEl, Database* database, ErrorLog* errorLog) {
	String fname = database->lastSegment;
	GNode* person = personEl->root;
	int line = personEl->line; // Used in error messages.
	normalizePerson(person);
	int errorCount = 0;
	static char s[512]; // For error strings.
	// Warning: use of __node is fragile because it uses internal details of the macros.
	FORFAMCS(person, family, key, database) // Check FAMC links to families.
		if (!family) {
			sprintf(s, "INDI %s (line %d): FAMC %s (line %d) does not exist.",
					person->key,
					line,
					key,
					line + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, fname, line, s));
			errorCount++;
		}
	ENDFAMCS
	FORFAMSS(person, family, key, database) // Check FAMS links to families.
		if (!family) {
				//int lineNumber = rootLine(person, database);
				sprintf(s, "INDI %s (line %d): FAMS %s (line %d) does not exist.",
						person->key, line, key, LC(line, __node));
				addErrorToLog(errorLog, createError(linkageError, fname, line, s));
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
			addErrorToLog(errorLog, createError(linkageError, fname, 0, "Child not found"));
			errorCount++;
		} else if (numOccurrences > 1) {
			addErrorToLog(errorLog, createError(linkageError, fname, 0, "Too many children found"));
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
			int lineNumber = rootLine(person, database);
			sprintf(s, "INDI %s (line %d) with FAMS %s (line %d) link has no sex value.",
					person->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, fname, 0, s));
			errorCount++;
			goto a;
		}
		if (person != parent) {
			sprintf(s, "FAM %s (line %d) should have %s link to INDI %s (line %d).",
					key,
					rootLine(family, database),
					sex == sexMale ? "HUSB" : "WIFE",
					person->key,
					line);
			addErrorToLog(errorLog, createError(linkageError, fname, rootLine(family, database), s));
			errorCount++;
		}
a:;
	ENDFAMSS


	//  Validate NAME and SEX lines.
	GNode* nnode = null;
	if (!hasValidNameGNode(person, &nnode)) {
		if (nnode) {
			sprintf(s, "INDI %s (line %d) has invalid NAME line (line %d).",
					person->key, line, LC(line, nnode));
			addErrorToLog(errorLog, createError(linkageError, fname, LC(line, nnode), s));
		} else {
			sprintf(s, "INDI %s (line %d) has no NAME line.", person->key, line);
			addErrorToLog(errorLog, createError(linkageError, fname, line, s));
		}
	}
	nnode = null;
	if (!hasValidSexGNode(person, &nnode)) {
		if (nnode) {
			sprintf(s, "INDI %s (line %d) has invalid SEX line (line %d).\n", person->key, line,
					LC(line, nnode));
			addErrorToLog(errorLog, createError(linkageError, fname, LC(line, nnode), s));
		} else {
			sprintf(s, "INDI %s has no SEX line.\n", person->key);
			addErrorToLog(errorLog, createError(linkageError, fname, line, s));
		}
	}
	return errorCount == 0;
}

// hasValidNameGNode returns true if an INDI record has one or more valid NAME nodes. Returns
// false if there are no NAME lines or there is one or more invalid NAME lines. If there are
// invalid NAME nodes the first is returned through nnode.
bool hasValidNameGNode(GNode* root, GNode** nnode) {
	*nnode = null;
	if (!root) return false;
	GNode* name = NAME(root);
	if (!name) return false;
	while (name) {
		if (!name->value) {
			*nnode = name;
			return false;
		}
		GNode* sib = name->sibling;
		name = (sib && eqstr("NAME", sib->tag)) ? sib : null;
	}
	return true;
}

// hasValidSexGNode returns true if an INDI record has a valid SEX line. Note: does not check
// for multiple SEX lines.
bool hasValidSexGNode(GNode* root, GNode** psex) {
	*psex = null;
	if (!root) return false;
	GNode* sex = SEX(root);
	if (!sex) return false;
	*psex = sex;
	return validSexString(sex->value);
}
