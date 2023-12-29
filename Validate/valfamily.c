//
//  DeadEnds
//
//  valfamily.c -- Functions that validate family records.
//
//  Created by Thomas Wetmore on 18 December 2023.
//  Last changed on 29 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

static bool validateFamily(GNode *family, Database*, ErrorLog*);

static bool debugging = false;

//  validateFamilyIndex -- Validate all family records in a database.
//-------------------------------------------------------------------------------------------------
bool validateFamilyIndex(Database *database, ErrorLog *errorLog)
{
	bool valid = true;
	FORHASHTABLE(database->familyIndex, element)
		GNode* family = ((RecordIndexEl*) element)->root;
		int numErrors = lengthList(errorLog);  // Debugging.
		if (debugging) printf("Start validating %s\n", family->key);
		if (!validateFamily(family, database, errorLog)) valid = false;
		if (debugging && lengthList(errorLog) > numErrors) {
			printf("There were %d errors validating %s\n", lengthList(errorLog) - numErrors, family->key);
			numErrors = lengthList(errorLog);
		}
		if (debugging) printf("Done validating %s\n", family->key);
	ENDHASHTABLE
	return valid;
}

//  validateFamily -- Validate a family node tree record. Check that all HUSB, WIFE and CHIL
//    links refer to existing persons. If so, check that all return links also exist.
//--------------------------------------------------------------------------------------------------
static bool validateFamily(GNode *family, Database *database, ErrorLog* errorLog)
{
	String segment = database->lastSegment;
	int errorCount = 0;
	static char s[4096];

	// Make sure all HUSB, WIFE and CHIL link to persons.
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
		// The husband must have one FAMS link back to this family.
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

//  familyLineNumber -- Given a person root node, return its location (line number) in the
//    original Gedcom file. Uses searchHashTable to get the RecordIndexEl of the record and
//    takes the line number from there. Obviously won't work correctly for records that
//    arrived from another source.
//--------------------------------------------------------------------------------------------------
int familyLineNumber (GNode *family, Database* database)
{
	RecordIndexEl *element = searchHashTable(database->familyIndex, family->key);
	if (!element) return 0;  // Should not happen.
	return element->lineNumber;  // Assume this is zero for records not from Gedcom files.
}
