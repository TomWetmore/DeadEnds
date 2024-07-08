// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 27 May 2024.

#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <stdlib.h>
#include "standard.h"
#include "import.h"
#include "gnode.h"
#include "stringtable.h"
#include "recordindex.h"
#include "gedcom.h"
#include "splitjoin.h"
#include "database.h"
#include "validate.h"
#include "errors.h"
#include "readnode.h"
#include "gnodelist.h"
#include "path.h"
#include "utils.h"
#include "stringset.h"

extern FILE* debugFile;

static void checkKeysAndReferences(GNodeList*, String name, ErrorLog*);


// toString returns the GNode in a GNodeListElement as a string; for debugging.
static String toString(void* element) {
	GNode* gnode = ((GNodeListEl*) element)->node;
	return gnodeToString(gnode, 0);
}

// Local flags.
static bool timing = true; // Prints time at milestones.
static bool debugging = true;
bool importDebugging = true; // Detail debugging of import.

// importFromFiles imports a list of Gedcom files into a List of Databases, one per file. If errors
// are found in a file the file's Database is not created and the ErrorLog will hold the errors.
List* importFromFiles(String filePaths[], int count, ErrorLog* errorLog) {
	List* listOfDatabases = createList(null, null, null, false);
	Database* database = null;
	for (int i = 0; i < count; i++) {
		if ((database = gedcomFileToDatabase(filePaths[i], errorLog)))
			appendToList(listOfDatabases, database);
	}
	return listOfDatabases;
}

// gedcomFileToDatabase returns the Database of a single Gedcom file. Returns null if no Database
// is created. errorLog holds any Errors found.
Database* gedcomFileToDatabase(String path, ErrorLog* log) {
	if (timing) printf("Start of gedcomFileToDatabase: %s.\n", getMillisecondsString());
	// Open the Gedcom file.
	File* file = createFile(path, "r");
	if (!file) {
		addErrorToLog(log, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
	// Get the list of GNode roots from file. These are all records from the Gedcom file including
	// HEAD and TRLR. If errors occur listOfTrees will be null and the ErrorLog will hold Errors.
	// errors.
	GNodeList* listOfTrees = getGNodeTreesFromFile(file, log);
	deleteFile(file);
	if (timing) printf("Got listOfTrees: %s.\n", getMillisecondsString());
	if (importDebugging) printf("listOfTrees contains %d records.\n", lengthList(listOfTrees));
	if (lengthList(log)) {
		deleteGNodeList(listOfTrees, true);
		return null;
	}
	// Check all keys and the references to them. There can be no duplicate keys and all
	// references to keys must be to the record keys.
	checkKeysAndReferences(listOfTrees, file->name, log);
	if (timing) printf("Checked keys: %s.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteGNodeList(listOfTrees, true);
		return null;
	}
	// Create record indexes. personIndex holds the INDI records; familyIndex holds the FAM
	// records, and recordIndex holds all keyed records.
	RecordIndex* personIndex = createRecordIndex();
	RecordIndex* familyIndex = createRecordIndex();
	RecordIndex* recordIndex = createRecordIndex();
	int otherRecords = 0; // Of debugging interest.
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
	deleteGNodeList(listOfTrees, false);
	if (timing) printf("Created the three indexes: %s.\n", getMillisecondsString());
	if (importDebugging) {
		printf("The person index holds %d records.\n", sizeHashTable(personIndex));
		printf("The family index holds %d records.\n", sizeHashTable(familyIndex));
		printf("The record index holds %d records.\n", sizeHashTable(recordIndex));
		printf("There are %d other records.\n", otherRecords);
	}
	// Create the Database and add the indexes.
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
static void checkKeysAndReferences(GNodeList* records, String name, ErrorLog* log) {
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

