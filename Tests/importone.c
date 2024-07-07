// DeadEnds
//
// importone.c has a test program for importing a single Gedcom file into a Database.
//
// Created by Thomas Wetmore on 21 June 2024.
// Last changed on 7 July 2024.

#include "standard.h"
#include "errors.h"

//#include <unistd.h>
#include <errno.h>
//#include <sys/param.h>
//#include <sys/errno.h>
//#include <stdlib.h>
//#include "import.h"
//#include "gnode.h"
//#include "stringtable.h"
//#include "recordindex.h"
//#include "gedcom.h"
#include "splitjoin.h"
//#include "database.h"
#include "validate.h"
//#include "readnode.h"
#include "stringset.h"
#include "gnodelist.h"
#include "path.h"
#include "utils.h"

static bool importDebugging = true;
static bool timing = true;

void checkKeysAndReferences(GNodeList*, String name, ErrorLog*);
Database* gedcomFileToDatabase(String, ErrorLog*);

int importTest(void) {
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/051224.ged";
	String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/07022024.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/smallish.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/parenterror.ged";
	ErrorLog* errorLog = createErrorLog();
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	Database* database = gedcomFileToDatabase(gedcomFile, errorLog);
	if (lengthList(errorLog)) {
		printf("Import cancelled because of errors:\n");
		showErrorLog(errorLog);
	}
	summarizeDatabase(database);
	return 0;
}

// FOLLOWING FUNCTION WILL BE MOVED TO ITS PROPER PLACE LATER.

// gedcomFileToDatabase returns the Database of a single Gedcom file. Returns null if no Database
// is created; the errorLog holds any errors encountered while processing the file.
Database* gedcomFileToDatabase(String path, ErrorLog* log) {
	if (timing) printf("Start of gedcomFileToDatabase: %s.\n", getMillisecondsString());
	// Open Gedcom file.
	File* file = createFile(path, "r");
	if (!file) {
		addErrorToLog(log, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
	// Get list of GNode roots from file.
	GNodeList* listOfTrees = getGNodeTreesFromFile(file, log);
	deleteFile(file);
	if (timing) printf("Got listOfTrees: %s.\n", getMillisecondsString());
	if (importDebugging) printf("listOfTrees contains %d records.\n", lengthList(listOfTrees));
	if (lengthList(log)) {
		deleteGNodeList(listOfTrees, true);
		return null;
	}
	// Check all keys and the references to them.
	checkKeysAndReferences(listOfTrees, file->name, log);
	if (timing) printf("Checked keys: %s.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteGNodeList(listOfTrees, true);
		return null;
	}
	// Create the indexes.
	RecordIndex* personIndex = createRecordIndex();
	RecordIndex* familyIndex = createRecordIndex();
	RecordIndex* recordIndex = createRecordIndex();
	int otherRecords = 0;
	FORLIST(listOfTrees, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		if (root->key)
			addToRecordIndex(recordIndex, root->key, root, el->line);
		if (recordType(root) == GRPerson)
			addToRecordIndex(personIndex, root->key, root, el->line);
		else if (recordType(root) == GRFamily)
			addToRecordIndex(familyIndex, root->key, root, el->line);
		else
			otherRecords++;
	ENDLIST
	if (timing) printf("Created the three indexes: %s.\n", getMillisecondsString());
	if (importDebugging) {
		printf("The person index holds %d records.\n", sizeHashTable(personIndex));
		printf("The family index holds %d records.\n", sizeHashTable(familyIndex));
		printf("The record index holds %d records.\n", sizeHashTable(recordIndex));
		printf("There are %d other records.\n", otherRecords);
	}
	// Create Database with the three indexes.
	Database* database = createDatabase(path);
	database->recordIndex = recordIndex;
	database->personIndex = personIndex;
	database->familyIndex = familyIndex;
	// Validate persons and families.
	validatePersons(database, log);
	if (timing) printf("Validated persons: %s.\n", getMillisecondsString());
	validateFamilies(database, log);
	if (timing) printf("Validated families: %s.\n", getMillisecondsString());
	validateReferences(database, log);
	if (lengthList(log)) {
		deleteDatabase(database);
		return null;
	}
	// Create name index.
	indexNames(database);
	if (timing) printf("Indexed names: %s.\n", getMillisecondsString());
	// Create the REFN index and validate it.
	validateReferences(database, log);
	if (timing) printf("Indexed REFNs: %s.\n", getMillisecondsString());
	if (timing) printf("End of gedcomFileToDatabase: %s.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteDatabase(database);
		return null;
	}
	return database;
}

// checkKeysAndReferences checks record keys and their references. Creates a table of all keys
// and checks for duplicates. CHecks that all keys found as values refer to records.
void checkKeysAndReferences(GNodeList* records, String name, ErrorLog* log) {
	StringSet* keySet = createStringSet();
	FORLIST(records, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		String key = root->key;
		if (!key) {
			RecordType rtype = recordType(root);
			if (rtype == GRHeader || rtype == GRTrailer) continue;
			addErrorToLog(log, createError(gedcomError, name, el->line, "record missing a key"));
			continue;
		}
		if (isInSet(keySet, key)) {
			addErrorToLog(log, createError(gedcomError, name, el->line, "duplicate key"));
			continue;
		}
		addToSet(keySet, key);
	ENDLIST
	// Check that keys used as values are in the set.
	int numReferences = 0; // Debug and sanity.
	int nodesTraversed = 0; // Debug and sanity.
	int recordsVisited = 0; // Debug and sanity.
	int nodesCounted = 0; // Debug and sanity.
	FORLIST(records, element)
		recordsVisited++;
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		nodesCounted += countNodes(root);
		FORTRAVERSE(root, node)
			nodesTraversed++;
			if (isKey(node->value)) numReferences++;
			if (isKey(node->value) && !isInSet(keySet, node->value)) {
				Error* error = createError(
								gedcomError,
								name,
								el->line + countNodesBefore(node),
								"invalid key value");
					addErrorToLog(log, error);
					printf("Didn't find key: %s\n", node->value);
				}
		ENDTRAVERSE
	ENDLIST
	if (importDebugging) {
		printf("The length of the key set is %d.\n", lengthSet(keySet));
		printf("The length of the error log is %d.\n", lengthList(log));
		printf("The number of references to keys is %d.\n", numReferences);
		printf("The number of records visited is %d.\n", recordsVisited);
		printf("The number of nodes traversed is %d.\n", nodesTraversed);
		printf("The number of nodes counted is %d.\n", nodesCounted);
	}
}
