// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 9 May 2025.

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
Database* getDatabaseFromFile(String path, int vcodes, ErrorLog* errlog) {
	if (timing) printf("%s: getDatabaseFromFile: started\n", gms);
	IntegerTable* keymap = createIntegerTable(4097); // Map keys to lines; for error messages.
    int numErrors = lengthList(errlog);
	RootList* records = getRecordListFromFile(path, keymap, errlog);
    if (lengthList(errlog) != numErrors) return null;
    if (timing) printf("%s: getDatabaseFromFile: record list created\n", gms);
    checkKeysAndReferences(records, path, keymap, errlog);
    if (lengthList(errlog) != numErrors) {
        deleteRootList(records);
        return null;
    }
    if (timing) printf("%s: getDatabaseFromFile: checkKeysAndReferences called\n", gms);
    // Create the database.
	Database* database = createDatabase(path, records, keymap, errlog);
	if (lengthList(errlog)) {
		deleteDatabase(database);
		return null;
	}
    validatePersons(database->recordIndex, database->name, keymap, errlog);
    validateFamilies(database->recordIndex, database->name, keymap, errlog);
    if (lengthList(errlog)) {
        deleteDatabase(database);
        return null;
    }
    if (timing) printf("%s: getDatabaseFromFile: database created.\n", gms);
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
	// Keys used as values must be in the key set.
	int numReferences = 0; // Debugging variables.
	int nodesTraversed = 0;
	int recordsVisited = 0;
	int nodesCounted = 0;
	FORLIST(records, element)
		recordsVisited++;
		GNode* root = (GNode*) element;
		nodesCounted += countGNodes(root);
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

// getRecordListFromFile reads a Gedcom file and creates a RootList of all records in the file. Each record is the
// root GNode of the record's GNode tree.
RootList* getRecordListFromFile(String path, IntegerTable* keymap, ErrorLog* elog) {
    if (timing) printf("%s: getRecordIndexFromFile: started.\n", gms);
    File* file = openFile(path, "r"); // Open the Gedcom file.
    if (!file) {
        addErrorToLog(elog, createError(systemError, path, 0, "Could not open file."));
        return null;
    }
    if (!keymap) keymap = createIntegerTable(4097); // Create line number map if needed.
    int initialErrorCount = lengthList(elog);

    // Read Gedcom file into flat GNode list
    GNodeList* nodes = getGNodeListFromFile(file, keymap, elog);
    closeFile(file);
    if (lengthList(elog) > initialErrorCount || !nodes) {
        if (nodes) deleteGNodeList(nodes, null); // GET DELETE DONE RIGHT
        deleteHashTable(keymap);
        return null;
    }
    // Convert GNode list to RootList (records)
    RootList* roots = getRootListFromGNodeList(nodes, file->name, elog);
    if (lengthList(elog) > initialErrorCount || !roots) {
        deleteGNodeList(nodes, null);
        deleteHashTable(keymap);
        if (roots) deleteGNodeList(roots, null);  // GET DELETE DONE RIGHT
        return null;
    }
    if (timing) printf("%s: getRecordIndexFromFile: got list of records.\n", gms);
    if (importDebugging) printf("rootList contains %d records.\n", lengthList(roots));
    return roots;
}

