// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 26 November 2024.

#include "import.h"
#include "validate.h"
#include "utils.h"

static bool timing = true;
bool importDebugging = false;

// gedcomFilesToDatabases imports a list of Gedcom files into a List of Databases, one per file.
// If errors are found in a file its Database is not created and the errors are logged.
List* gedcomFilesToDatabases(List* filePaths, ErrorLog* errorLog) {
	List* databases = createList(null, null, null, false); // TODO: No delete function.
	Database* database = null;
	FORLIST(filePaths, path)
		if ((database = gedcomFileToDatabase(path, errorLog)))
			appendToList(databases, database);
	ENDLIST
	return databases;
}

// gedcomFileToDatabase returns the Database of a single Gedcom file. Returns null if no Database
// is created. errorLog holds any Errors found.
Database* gedcomFileToDatabase(String path, ErrorLog* log) {
	if (timing) printf("%s: start of gedcomFileToDatabase.\n", getMillisecondsString());
	// Open the Gedcom file.
	File* file = openFile(path, "r");
	if (!file) {
		addErrorToLog(log, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
	// Get the GNode records from the file. The error log will hold any Errors. keymap maps keys
	// to line numbers. The keys must not not be freed when the key map is deleted.
	IntegerTable* keymap = createIntegerTable(4097);
	GNodeList* roots = getGNodeTreesFromFile(file, keymap, log);
	closeFile(file);
	if (roots == null) {
		if (importDebugging) printf("%s: errors processing last file.\n", getMillisecondsString());
		//deleteIntegerTable(keymap, stdfree); // TODO: Delete the keymap
		return null;
	}
	if (timing) printf("%s: got list of records.\n", getMillisecondsString());
	if (importDebugging) printf("rootList contains %d records.\n", lengthList(roots));
	if (lengthList(log)) {
		deleteGNodeList(roots, null); // TODO: Clean up. This situation can't happen.
		return null;
	}
	// Check all keys and their references.
	checkKeysAndReferences(roots, file->name, keymap, log);
	if (timing) printf("%s: checked keys.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteGNodeList(roots, null); // TODO: NEED TO GET A GOOD DELETE FUNCTION IN HERE.
		return null;
	}
	// If the first record is a header remember it.
	GNode* header = ((GNodeListEl*) getFirstListElement(roots))->node;
	if (nestr(header->tag, "HEAD")) {
		header = null;
	}
	// Create the record indexes and root lists.
	RecordIndex* personIndex = createRecordIndex();
	RecordIndex* familyIndex = createRecordIndex();
	RecordIndex* recordIndex = createRecordIndex();
	RecordIndex* sourceIndex = createRecordIndex();
	RootList* personRoots = createRootList();
	RootList* familyRoots = createRootList();
	FORLIST(roots, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		if (root->key) addToRecordIndex(recordIndex, root->key, root);
		if (recordType(root) == GRPerson) {
			addToRecordIndex(personIndex, root->key, root);
			insertInRootList(personRoots, root);
		} else if (recordType(root) == GRFamily) {
			addToRecordIndex(familyIndex, root->key, root);
			insertInRootList(familyRoots, root);
		} else if (recordType(root) == GRSource) {
			addToRecordIndex(sourceIndex, root->key, root);
		}
	ENDLIST
	deleteGNodeList(roots, false);
	// Create the Database and add the indexes.
	Database* database = createDatabase(path);
	database->header = header;
	database->recordIndex = recordIndex;
	database->personIndex = personIndex;
	database->familyIndex = familyIndex;
	database->sourceIndex = sourceIndex;
	database->personRoots = personRoots;
	database->familyRoots = familyRoots;
	if (importDebugging) summarizeDatabase(database);
	if (timing) printf("%s: database created.\n", getMillisecondsString());
	// Validate persons and families.
	validatePersons(database, keymap, log);
	if (timing) printf("%s: validated %d persons.\n", getMillisecondsString(),
					   sizeHashTable(database->personIndex));
	validateFamilies(database, keymap, log);
	if (timing) printf("%s: validated %d families.\n", getMillisecondsString(),
					   sizeHashTable(database->familyIndex));
	if (lengthList(log)) {
		deleteDatabase(database);
		return null;
	}
	// Create the name index.
	getNameIndexForDatabase(database);
	if (timing) printf("%s: indexed names.\n", getMillisecondsString());
	// Create the REFN index and validate it.
	validateReferences(database, keymap, log);
	if (timing) printf("%s: indexed REFNs.\n", getMillisecondsString());
	if (timing) printf("%s: end of gedcomFileToDatabase.\n", getMillisecondsString());
	if (lengthList(log)) {
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
