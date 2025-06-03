//
//  DeadEnds Library
//
//  valfamily.c has the functions that validate family records.
//
//  Created by Thomas Wetmore on 18 December 2023.
//  Last changed on 3 June 2025.
//

#include "errors.h"
#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "lineage.h"
#include "recordindex.h"
#include "splitjoin.h"
#include "validate.h"

static bool validateFamily(GNode*, String name, RecordIndex*, IntegerTable*, ErrorLog*);
extern bool importDebugging;

// validateFamilies validates the family records in a database.
void validateFamilies(RecordIndex* index, String name, IntegerTable* keymap, ErrorLog *elog) {
	int numFamiliesValidated = 0;
	FORHASHTABLE(index, element)
		GNode* root = (GNode*) element;
		if (recordType(root) == GRFamily) {
			validateFamily(root, name, index, keymap, elog);
			numFamiliesValidated++;
		}
	ENDHASHTABLE
	if (importDebugging) printf("The number of families validated is %d.\n", numFamiliesValidated);
}

// validateFamily validates a family; it checks that all HUSBs, WIFEs and CHILs refer to existing
// persons, and that the return links exist.
static bool validateFamily(GNode* family, String name, RecordIndex* index, IntegerTable* keymap,
						   ErrorLog* elog) {
	normalizeFamily(family);
	int errorCount = 0;
	static char s[4096];

	// HUSB, WIFE and CHIL nodes must point to persons.
	FORHUSBS(family, husband, key, index)
		if (!husband) {
			//printf("%s ", family->key); // DEBUG
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): HUSB %s (line %d) does not exist.",
					family->key, lineNumber, key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, name, 0, s));
			errorCount++;
		}
	ENDHUSBS
	//printf("\nALL FAM KEYS FROM FORWIVES\n"); // DEBUG
	FORWIFES(family, wife, key, index)
		//printf("%s ", family->key); // DEBUG
		if (!wife) {
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): WIFE %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, name, 0, s));
			errorCount++;
		}
	ENDWIFES
	//printf("\nALL FAM KEYS FROM FORCHILDREN\n"); // DEBUG
	FORCHILDREN(family, child, key, n, index)
	if (!child) {
			//printf("%s ", family->key); // DEBUG
			int lineNumber = rootLine(family, keymap);
			sprintf(s, "FAM %s (line %d): CHIL %s (line %d) does not exist.",
					family->key,
					lineNumber,
					key,
					lineNumber + countNodesBefore(__node));
			addErrorToLog(elog, createError(linkageError, name, lineNumber, s));
			errorCount++;
		}
	ENDCHILDREN

	// If there were errors above then the following code should not run.
	if (errorCount) return false;

	FORHUSBS(family, husband, hkey, index)
		// Husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, fkey, index)
			if (family == fam) numOccurences++;
		ENDFAMSS
		if (numOccurences != 1) {
			addErrorToLog(elog, createError(linkageError, name, 111, "Husband married to same wife in multiple families"));
			errorCount++;
		}
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, wkey, index) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, fkey, index)
			if (family == fam) numOccurences++;
		ENDFAMSS
        if (numOccurences != 1) {
            addErrorToLog(elog, createError(linkageError, name, 111, "Wife married to same husband in multiple families"));
            errorCount++;
        }
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, chilKey, n, index)
		int numOccurences = 0;
		FORFAMCS(child, fam, key, index)
			if (family == fam) numOccurences++;
		ENDFAMCS
    if (numOccurences != 1) {
        addErrorToLog(elog, createError(linkageError, name, 111, "Person is a child in multiple families"));
        errorCount++;
    }
	ENDFAMCS

	bool hasHusb = HUSB(family) != null;
	bool hasWife = WIFE(family) != null;
	bool hasChild = CHIL(family) != null;
	if (!(hasHusb || hasWife || hasChild)) {
		addErrorToLog(elog, createError(linkageError, name, 0, "Family has no HUSB, WIFE or CHIL links"));
	}
	//printf("validate family: %s\n", family->gKey);
	//  Validate existance of at least one of HUSB, WIFE, CHIL.
	//  Validate that the HUSBs are male.
	//  Validate that the WIFEs are female.
	//  Validate all other links.
	return true;  // TODO: Deal with the errors properly.
}
