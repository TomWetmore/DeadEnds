// DeadEnds
//
// valfamily.c has the functions that validate family records.
//
// Created by Thomas Wetmore on 18 December 2023.
// Last changed on 5 July 2024.

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

static bool validateFamily(RecordIndexEl*, Database*, ErrorLog*);
extern bool importDebugging;

// validateFamilyIndex validates the family records in a database.
void validateFamilies(Database *database, ErrorLog *errorLog) {
	int numFamiliesValidated = 0;
	FORHASHTABLE(database->familyIndex, element)
		validateFamily(element, database, errorLog);
		numFamiliesValidated++;
	ENDHASHTABLE
	if (importDebugging) printf("The number of families validated is %d.\n", numFamiliesValidated);
}

// validateFamily validates a family; it checks that all HUSBs, WIFEs and CHILs refer to existing
// persons, and that the return links exist.
static bool validateFamily(RecordIndexEl* familyEl, Database *database, ErrorLog* errorLog) {
	GNode* family = familyEl->root;
	String segment = database->lastSegment;
	int errorCount = 0;
	static char s[4096];

	// All HUSB, WIFE and CHIL must link to persons.
	FORHUSBS(family, husband, key, database)
		if (!husband) {
			int lineNumber = familyLineNumber(family, database);
			sprintf(s, "FAM %s (line %d): HUSB %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
	ENDHUSBS

	FORWIFES(family, wife, key, database)
		if (!wife) {
			int lineNumber = familyLineNumber(family, database);
			sprintf(s, "FAM %s (line %d): WIFE %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, 0, s));
			errorCount++;
		}
	ENDWIFES

	FORCHILDREN(family, child, key, n, database)
	if (!child) {
			int lineNumber = familyLineNumber(family, database);
			sprintf(s, "FAM %s (line %d): CHIL %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(errorLog, createError(linkageError, segment, lineNumber, s));
			errorCount++;
		}
	ENDCHILDREN

	// If there were errors above then the following code should not run.
	if (errorCount) return false;

	FORHUSBS(family, husband, hkey, database)
		// Husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, fkey, database)
			if (family == fam) numOccurences++;
		ENDFAMSS
		if (numOccurences != 1) {
			addErrorToLog(errorLog, createError(linkageError, segment, 111, "GET ERROR MESSAGE"));
			errorCount++;
		}
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, wkey, database) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, fkey, database)
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, chilKey, n, database)
		int numOccurences = 0;
		FORFAMCS(child, fam, key, database)
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

// familyLineNumber returns the line number of a family root GNode in the original Gedcom file.
int familyLineNumber(GNode* family, Database* database) {
	RecordIndexEl* element = searchHashTable(database->familyIndex, family->key);
	if (!element) return 0;
	return element->line;
}
