// DeadEnds
//
// valfamily.c has the functions that validate family records.
//
// Created by Thomas Wetmore on 18 December 2023.
// Last changed on 26 November 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"
#include "splitjoin.h"

static bool validateFamily(RecordIndexEl*, Database*, IntegerTable*, ErrorLog*);
extern bool importDebugging;

// validateFamilyIndex validates the family records in a database.
void validateFamilies(Database *dbase, IntegerTable* keymap, ErrorLog *elog) {
	int numFamiliesValidated = 0;
	FORHASHTABLE(dbase->familyIndex, element)
		validateFamily(element, dbase, keymap, elog);
		numFamiliesValidated++;
	ENDHASHTABLE
	if (importDebugging) printf("The number of families validated is %d.\n", numFamiliesValidated);
}

// validateFamily validates a family; it checks that all HUSBs, WIFEs and CHILs refer to existing
// persons, and that the return links exist.
static bool validateFamily(RecordIndexEl* famel, Database *dbase, IntegerTable* keymap, ErrorLog* elog) {
	GNode* family = famel->root;
	normalizeFamily(family);
	String segment = dbase->lastSegment;
	int errorCount = 0;
	static char s[4096];

	// All HUSB, WIFE and CHIL must link to persons.
	//printf("ALL FAM KEYS FROM FORHUSBS\n"); // DEBUG
	FORHUSBS(family, husband, key, dbase)
		if (!husband) {
			//printf("%s ", family->key); // DEBUG
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): HUSB %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
	ENDHUSBS
	//printf("\nALL FAM KEYS FROM FORWIVES\n"); // DEBUG
	FORWIFES(family, wife, key, dbase)
		//printf("%s ", family->key); // DEBUG
		if (!wife) {
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): WIFE %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
	ENDWIFES
	//printf("\nALL FAM KEYS FROM FORCHILDREN\n"); // DEBUG
	FORCHILDREN(family, child, key, n, dbase)
	if (!child) {
			//printf("%s ", family->key); // DEBUG
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): CHIL %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDCHILDREN

	// If there were errors above then the following code should not run.
	if (errorCount) return false;

	FORHUSBS(family, husband, hkey, dbase)
		// Husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, fkey, dbase)
			if (family == fam) numOccurences++;
		ENDFAMSS
		if (numOccurences != 1) {
			addErrorToLog(elog, createError(linkageError, segment, 111, "GET ERROR MESSAGE"));
			errorCount++;
		}
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, wkey, dbase) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, fkey, dbase)
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, chilKey, n, dbase)
		int numOccurences = 0;
		FORFAMCS(child, fam, key, dbase)
			if (family == fam) numOccurences++;
		ENDFAMCS
		ASSERT(numOccurences == 1);
	ENDFAMCS

	bool hasHusb = HUSB(family) != null;
	bool hasWife = WIFE(family) != null;
	bool hasChild = CHIL(family) != null;
	if (!(hasHusb || hasWife || hasChild)) {
		addErrorToLog(elog, createError(linkageError, segment, 0, "Family has no HUSB, WIFE or CHIL links"));
	}
	//printf("validate family: %s\n", family->gKey);
	//  Validate existance of at least one of HUSB, WIFE, CHIL.
	//  Validate that the HUSBs are male.
	//  Validate that the WIFEs are female.
	//  Validate all other links.
	return true;  // TODO: Deal with the errors properly.
}
