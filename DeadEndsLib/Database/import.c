// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 20 December 2024.

#include "import.h"
#include "validate.h"
#include "utils.h"

#define gms getMsecondsStr()
static bool timing = true;
bool importDebugging = false;

// getDatabasesFromFiles imports a list of Gedcom files into a List of Databases, one per file.
// If errors are found in a file its Database is not created and the errors are logged.
static void deletedbase(void* element) { deleteDatabase((Database*) element); }
List* getDatabasesFromFiles(List* filePaths, int vcodes, ErrorLog* errorLog) {
	List* databases = createList(null, null, deletedbase, false);
	Database* database = null;
	FORLIST(filePaths, path)
		if ((database = getDatabaseFromFile(path, vcodes, errorLog)))
			appendToList(databases, database);
	ENDLIST
	return databases;
}

// getDatabaseFromFile returns the Database of a single Gedcom file. Returns null if no Database
// is created, and errorLog holds the Errors found.
Database* getDatabaseFromFile(String path, int vcodes, ErrorLog* elog) {
	if (timing) printf("%s: getDatabaseFromFile: started\n", gms);
	RootList* personRoots = createRootList();
	RootList* familyRoots = createRootList();
	IntegerTable* keymap = createIntegerTable(4097); // Map keys to lines; for error messages.
	RecordIndex* recordIndex = getRecordIndexFromFile(path, personRoots, familyRoots, keymap, elog);
	if (timing) printf("%s: getDatabaseFromFile: record index created\n", gms);
	if (lengthList(elog)) return null; // TODO: Freeup structures.
	Database* database = createDatabase(path);
	database->recordIndex = recordIndex;
	database->personRoots = personRoots;
	database->familyRoots = familyRoots;
	// Create the name and REFN indexes.
	database->nameIndex = getNameIndex(personRoots);
	database->refnIndex = getReferenceIndex(recordIndex, path, keymap, elog);
	if (timing) printf("%s: getDatabaseFromFile: indexed names and REFNs.\n", gms);
	if (timing) printf("%s: getDatabaseFromFile: done.\n", gms);
	if (lengthList(elog)) {
		deleteDatabase(database);
		return null;
	}
	return database;
}

// checkKeysAndReferences checks record keys and their references. Creates a table of all keys
// and checks for duplicates. Checks that all keys found as values refer to records.
#define getline(key) (searchIntegerTable(keymap, key))
void checkKeysAndReferences(RootList* records, String name, IntegerTable* keymap, ErrorLog* log) {
	StringSet* keySet = createStringSet();
	FORLIST(records, element)
		GNode* root = (GNode*) element;
		String key = root->key;
		if (!key) {
			RecordType rtype = recordType(root);
			if (rtype == GRHeader || rtype == GRTrailer) continue;
			addErrorToLog(log, createError(gedcomError, name, getline(key), "record missing a key"));
			continue;
		}
		if (isInSet(keySet, key)) {
			addErrorToLog(log, createError(gedcomError, name, getline(key), "duplicate key"));
			continue;
		}
		addToSet(keySet, key);
	ENDLIST
	// Check that keys used as values are in the key set.
	int numReferences = 0; // These variables are for debugging.
	int nodesTraversed = 0;
	int recordsVisited = 0;
	int nodesCounted = 0;
	FORLIST(records, element)
		recordsVisited++;
		GNode* root = (GNode*) element;
		nodesCounted += countNodes(root);
		FORTRAVERSE(root, node)
			nodesTraversed++;
			if (isKey(node->value)) numReferences++;
			if (isKey(node->value) && !isInSet(keySet, node->value)) {
				Error* error = createError(gedcomError, name,
								getline(node->key) + countNodesBefore(node), "invalid key value");
					addErrorToLog(log, error);
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
	deleteStringSet(keySet, false);
}

// getRecordIndexFromFile reads a Gedcom file into a RecordIndex. If personRoots and/or
// familyRoots are not null they will be filled.
RecordIndex* getRecordIndexFromFile(String path, RootList* personRoots, RootList* familyRoots,
									IntegerTable* keymap, ErrorLog* elog) {
	if (timing) printf("%s: getRecordIndexFromFile: started.\n", gms);
	File* file = openFile(path, "r"); // Open the file.
	if (!file) {
		addErrorToLog(elog, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
    String name = strsave(file->name);
	if (!keymap) keymap = createIntegerTable(4097); // MNOTE: HOW TO GET FREED!
	RootList* roots = getRootListFromFile(file, keymap, elog); // Get the records from file.
	closeFile(file);
	if (roots == null) {
		if (importDebugging) printf("%s: errors processing last file.\n", gms);
		//deleteIntegerTable(keymap, stdfree); // TODO: function not written yet.
		stdfree(name);
		return null;
	}
	if (timing) printf("%s: getRecordIndexFromFile: got list of records.\n", gms);
	if (importDebugging) printf("rootList contains %d records.\n", lengthList(roots));
	if (lengthList(elog)) {
		deleteGNodeList(roots, null); // TODO: Clean up. This situation can't happen.
		stdfree(name);
		return null;
	}
	// Check all keys and their references.
	checkKeysAndReferences(roots, name, keymap, elog);
	if (timing) printf("%s: getRecordIndexFromFile: checked keys.\n", gms);
	if (lengthList(elog)) {
		deleteGNodeList(roots, null); // TODO: NEED TO GET A GOOD DELETE FUNCTION IN HERE.
		stdfree(name);
		return null;
	}
	// Create the RecordIndex and optional RootLists.
	RecordIndex* recordIndex = createRecordIndex();
	FORLIST(roots, element)
		GNode* root = (GNode*) element;
		if (root->key) addToRecordIndex(recordIndex, root);
		RecordType rtype = recordType(root);
		if (personRoots && rtype == GRPerson) insertInRootList(personRoots, root);
		if (familyRoots && rtype == GRFamily) insertInRootList(familyRoots, root);
	ENDLIST
	deleteGNodeList(roots, false);
	if (timing) printf("%s: getRecordIndexFromFile: record index created.\n", gms);
	// Validate persons and families.
	validatePersons(recordIndex, name, keymap, elog);
	validateFamilies(recordIndex, name, keymap, elog);
	if (timing) printf("%s: getRecordIndexFromFile: records validated: returning.\n", gms);
	stdfree(name);
	return recordIndex;
}
