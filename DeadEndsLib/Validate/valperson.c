// DeadEnds
//
// valperson.c contains functions that validate person records in a Database.
//
// Created by Thomas Wetmore on 17 December 2023.
// Last changed on 8 December 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "list.h"
#include "splitjoin.h"
#include "integertable.h"
#include "utils.h"

#define LC(line, node) (line + countNodesBefore(node))

static bool validatePerson(GNode*, String name, RecordIndex*, IntegerTable*, ErrorLog*);
static bool hasValidNameGNode(GNode* root, GNode** pname);
static bool hasValidSexGNode(GNode* root, GNode** psex);
static bool importDebugging = true;

// validatePersons validates the persons in a Database.
void validatePersons(RecordIndex* index, String name, IntegerTable* keymap, ErrorLog* elog) {
	int numPersonsValidated = 0;
	FORHASHTABLE(index, element)
		GNode* root = (GNode*) element;
		if (recordType(root) == GRPerson) {
			validatePerson(root, name, index, keymap, elog);
			numPersonsValidated++;
		}
	ENDHASHTABLE
	if (importDebugging) printf("%s: validatePersons: %d persons validated.\n", getMsecondsStr(), numPersonsValidated);
}

// validatePerson validates a person record. Persons require at least one NAME and one SEX line
// with valid values. All FAMC and FAMS links must link to families that link back to the person.
//
// Notes on generating errors. A String buffer, s, exists. The personEl argument links to both
// the root node and the location of the root node in the original Gedcom file.
static bool validatePerson(GNode* person, String name, RecordIndex* index, IntegerTable* keymap,
						   ErrorLog* elog) {
	int line = searchIntegerTable(keymap, person->key); // Used in error messages.
	ASSERT(line != NAN);
	normalizePerson(person);
	int errorCount = 0;
	static char s[512]; // For error strings.
	// Warning: use of __node is fragile because it uses internal details of the macros.
	FORFAMCS(person, family, key, index) // Check FAMC links to families.
		if (!family) {
			sprintf(s, "INDI %s (line %d): FAMC %s (line %d) does not exist.",
					person->key, line, key, line + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, name, line, s));
			errorCount++;
		}
	ENDFAMCS
	FORFAMSS(person, family, key, index) // Check FAMS links to families.
		if (!family) {
				//int lineNumber = rootLine(person, database);
				sprintf(s, "INDI %s (line %d): FAMS %s (line %d) does not exist.",
						person->key, line, key, LC(line, __node));
				addErrorToLog(elog, createError(linkageError, name, line, s));
			errorCount++;
		}
	ENDFAMSS
	if (errorCount) return false;
	FORFAMCS(person, family, key, index) // Check FAMC links back to person.
		int numOccurrences = 0;
		FORCHILDREN(family, child, chilKey, count, index) // Find child that links to person.
			if (person == child) numOccurrences++;
		ENDCHILDREN
		if (numOccurrences == 0) {
			addErrorToLog(elog, createError(linkageError, name, 0, "Child not found"));
			errorCount++;
		} else if (numOccurrences > 1) {
			addErrorToLog(elog, createError(linkageError, name, 0, "Too many children found"));
			errorCount++;
		}
	ENDFAMCS
	if (errorCount) return false;
	SexType sex = SEXV(person);
	FORFAMSS(person, family, key, index) // Check FAMS links back to person.
		GNode *parent = null;
		if (sex == sexMale) {
			parent = familyToHusband(family, index);
		} else if (sex == sexFemale) {
			parent = familyToWife(family, index);
		} else {
			int lineNumber = rootLine(person, keymap);
			sprintf(s, "INDI %s (line %d) with FAMS %s (line %d) link has no sex value.",
					person->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, name, 0, s));
			errorCount++;
			goto a;
		}
		if (person != parent) {
			sprintf(s, "FAM %s (line %d) should have %s link to INDI %s (line %d).",
					key,
					rootLine(family, keymap),
					sex == sexMale ? "HUSB" : "WIFE",
					person->key,
					line);
			addErrorToLog(elog, createError(linkageError, name, rootLine(family, keymap), s));
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
			addErrorToLog(elog, createError(linkageError, name, LC(line, nnode), s));
		} else {
			sprintf(s, "INDI %s (line %d) has no NAME line.", person->key, line);
			addErrorToLog(elog, createError(linkageError, name, line, s));
		}
	}
	nnode = null;
	if (!hasValidSexGNode(person, &nnode)) {
		if (nnode) {
			sprintf(s, "INDI %s (line %d) has invalid SEX line (line %d).\n", person->key, line,
					LC(line, nnode));
			addErrorToLog(elog, createError(linkageError, name, LC(line, nnode), s));
		} else {
			sprintf(s, "INDI %s has no SEX line.\n", person->key);
			addErrorToLog(elog, createError(linkageError, name, line, s));
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
