//
//  DeadEnds
//
//  validate.c -- Functions that validate Gedcom records.
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 18 December 2023.
//

#include "validate.h"
#include "gnode.h"
#include "gedcom.h"
#include "recordindex.h"
#include "lineage.h"
#include "errors.h"

bool validateDatabase(Database*, ErrorLog*);

//static bool debugging = true;

static void validateSource(GNode*, Database*, ErrorLog*);
static void validateEvent(GNode*, Database*, ErrorLog*);
static void validateOther(GNode*, Database*, ErrorLog*);

//  personLineNumber -- Given a person root node, return its location (line number) in the
//    original Gedcom file. Uses searchHashTable to get the RecordIndexEl of the record and
//    takes the line number from there. Obviously won't work correctly for records that
//    arrived from another source.
//--------------------------------------------------------------------------------------------------
int personLineNumber (GNode *person, Database* database)
{
	RecordIndexEl *element = searchHashTable(database->personIndex, person->key);
	if (!element) return 0;  // Should not happen.
	return element->lineNumber;  // Assume this is zero for records not from Gedcom files.
}

int numValidations = 0;  //  DEBUG.

//  validateDatabase
//--------------------------------------------------------------------------------------------------
bool validateDatabase(Database *database, ErrorLog *errorLog)
{
	ASSERT(database);
	validatePersonIndex(database, errorLog);
	//validateFamilyIndex(database, errorLog);
	//if (!validateIndex(database->sourceIndex)) isOkay = false;
	//if (!validateIndex(database->eventIndex)) isOkay = false;
	//if (!validateIndex(database->otherIndex)) isOkay = false;
	//return isOkay;
	if (lengthList(errorLog)) {
		printf("THERE WERE ERRORS DURING VALIDATION:\n");
		showErrorLog(errorLog);
		return false;
	}
	return true;
}



extern String nameString(String);





void validateSource(GNode *source, Database *database, ErrorLog* errorLog) {}

void validateEvent(GNode *event, Database *database, ErrorLog* errorLog) {}

void validateOther(GNode *other, Database *database, ErrorLog* errorLog) {}


static GNode *getFamily(String key, RecordIndex *index)
{
	GNode *root = searchRecordIndex(index, key);
	return root && recordType(root) == GRFamily ? root : null;
}

static GNode *getPerson(String key, RecordIndex *index)
{
	GNode *root = searchRecordIndex(index, key);
	return root && recordType(root) == GRPerson ? root : null;
}
