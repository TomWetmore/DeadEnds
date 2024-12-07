// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 6 December 2024.

#include "import.h"
#include "validate.h"
#include "utils.h"

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
	RootList* personRoots = createRootList();
	RootList* familyRoots = createRootList();
	IntegerTable* keymap = createIntegerTable(4097); // Map keys to lines; for error messages.
	RecordIndex* recordIndex = getRecordIndexFromFile(path, personRoots, familyRoots, keymap, elog);
	if (lengthList(elog)) return null; // TODO: Freeup structures.
	Database* database = createDatabase(path);
	database->recordIndex = recordIndex;
	database->personRoots = personRoots;
	database->familyRoots = familyRoots;
	// Create the name index.
	database->nameIndex = getNameIndexFromPersons(personRoots);
	if (timing) printf("%s: indexed names.\n", getMsecondsStr());
	// Create the REFN index and validate it.
	if (timing) printf("%s: indexed REFNs.\n", getMsecondsStr());
	if (timing) printf("%s: end of gedcomFileToDatabase.\n", getMsecondsStr());
	if (lengthList(elog)) {
		deleteDatabase(database);
		return null;
	}
	return database;
}

// checkKeysAndReferences checks record keys and their references. Creates a table of all keys
// and checks for duplicates. Checks that all keys found as values refer to records.
#define getline(key) (searchIntegerTable(keymap, key))
void checkKeysAndReferences(GNodeList* records, String name, IntegerTable* keymap, ErrorLog* log) {
	StringSet* keySet = createStringSet();
	FORLIST(records, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
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
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		nodesCounted += countNodes(root);
		FORTRAVERSE(root, node)
			nodesTraversed++;
			if (isKey(node->value)) numReferences++;
			if (isKey(node->value) && !isInSet(keySet, node->value)) {
				Error* error = createError(gedcomError, name,
								getline(el->node->key) + countNodesBefore(node), "invalid key value");
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
	if (timing) printf("%s: getRecordIndexFromFile: start.\n", getMsecondsStr());
	File* file = openFile(path, "r"); // Open the file.
	if (!file) {
		addErrorToLog(elog, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
	if (!keymap) keymap = createIntegerTable(4097); // MNOTE: HOW TO GET FREED!
	GNodeList* roots = getGNodeTreesFromFile(file, keymap, elog); // Get the records from file.
	closeFile(file);
	if (roots == null) {
		if (importDebugging) printf("%s: errors processing last file.\n", getMsecondsStr());
		//deleteIntegerTable(keymap, stdfree); // TODO: function not written yet.
		return null;
	}
	if (timing) printf("%s: got list of records.\n", getMsecondsStr());
	if (importDebugging) printf("rootList contains %d records.\n", lengthList(roots));
	if (lengthList(elog)) {
		deleteGNodeList(roots, null); // TODO: Clean up. This situation can't happen.
		return null;
	}
	// Check all keys and their references.
	checkKeysAndReferences(roots, file->name, keymap, elog);
	if (timing) printf("%s: getRecordIndexFromFile: checked keys.\n", getMsecondsStr());
	if (lengthList(elog)) {
		deleteGNodeList(roots, null); // TODO: NEED TO GET A GOOD DELETE FUNCTION IN HERE.
		return null;
	}
	// Create the RecordIndex and optional RootLists.
	RecordIndex* recordIndex = createRecordIndex();
	FORLIST(roots, element)
		GNode* root = ((GNodeListEl*) element)->node;
		if (root->key) addToRecordIndex(recordIndex, root);
		RecordType rtype = recordType(root);
		if (personRoots && rtype == GRPerson) insertInRootList(personRoots, root);
		if (familyRoots && rtype == GRFamily) insertInRootList(familyRoots, root);
	ENDLIST
	deleteGNodeList(roots, false);

	if (timing) printf("%s: getRecordIndexFromFile: record index created.\n",
					   getMsecondsStr());
	// Validate persons and families.
	validatePersons(recordIndex, file->name, keymap, elog);
	validateFamilies(recordIndex, file->name, keymap, elog);
	//alidateReferences(recordIndex, file->name, keymap, elog);
	RefnIndex* refnIndex = getReferenceIndex(recordIndex, file->name, keymap, elog);
	if (timing) printf("%s: getRecordIndexFromFile: persons & families validated: returning.\n",
					   getMsecondsStr());
	return recordIndex;
}
