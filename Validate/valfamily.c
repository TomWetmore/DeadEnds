//
//  DeadEnds
//
//  valfamily.c -- Functions that validate family records.
//
//  Created by Thomas Wetmore on 18 December 2023.
//  Last changed on 18 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

static bool validateFamily(GNode*, Database*, ErrorLog*);

static bool debugging = true;

//  validateFamilyIndex -- Validate all family records in a database.
//-------------------------------------------------------------------------------------------------
bool validateFamilyIndex(Database *database, ErrorLog *errorLog)
{
	bool valid = true;
	FORHASHTABLE(database->familyIndex, element)
		GNode *family = ((RecordIndexEl*) element)->root;
		if (!validateFamily(family, database, errorLog)) valid = false;
	ENDHASHTABLE
	return valid;
}

//  validateFamily -- Validate a family node tree record. Check all HUSB, WIFE and CHIL links
//    to persons.
//--------------------------------------------------------------------------------------------------
static bool validateFamily(GNode *family, Database *database, ErrorLog* errorLog)
{
	if (debugging) printf("validateFamily(%s)\n", family->key);
	String segment = database->lastSegment;
	// For each HUSB line in the family (multiples in non-traditional cases).
	FORHUSBS(family, husband, database)
		// The husband must have one FAMS link back to this family.
		int numOccurences = 0;
		FORFAMSS(husband, fam, key, database)
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	ENDHUSBS

	//  For each WIFE line in the family (multiples in non-traditional cases)...
	FORWIFES(family, wife, database) {
		int numOccurences = 0;
		FORFAMSS(wife, fam, key, database)
			if (family == fam) numOccurences++;
		ENDFAMSS
		ASSERT(numOccurences == 1);
	} ENDWIFES

	//  For each CHIL node in the family.
	FORCHILDREN(family, child, n, database)
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