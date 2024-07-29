// DeadEnds
//
// import.c has functions that import Gedcom files into internal structures.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 21 July 2024.

#include "import.h"
#include "validate.h"
#include "utils.h"

extern FILE* debugFile;
static bool timing = false;
bool importDebugging = false;

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
	if (timing) printf("%s: start of gedcomFileToDatabase.\n", getMillisecondsString());
	// Open the Gedcom file.
	File* file = openFile(path, "r");
	if (!file) {
		addErrorToLog(log, createError(systemError, path, 0, "Could not open file."));
		return null;
	}
	// Get the GNode records from the file. If errors the error log will hold Errors.
	GNodeList* rootList = getGNodeTreesFromFile(file, log);
	closeFile(file);
	if (timing) printf("%s: got list of records.\n", getMillisecondsString());
	if (importDebugging) printf("rootList contains %d records.\n", lengthList(rootList));
	if (lengthList(log)) {
		deleteGNodeList(rootList, true);
		return null;
	}
	// Check all keys and their references.
	checkKeysAndReferences(rootList, file->name, log);
	if (timing) printf("%s: checked keys.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteGNodeList(rootList, true);
		return null;
	}
	// Create the record indexes and root lists.
	RecordIndex* personIndex = createRecordIndex();
	RecordIndex* familyIndex = createRecordIndex();
	RecordIndex* recordIndex = createRecordIndex();
	RootList* personRoots = createRootList();
	RootList* familyRoots = createRootList();
	FORLIST(rootList, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		if (root->key) addToRecordIndex(recordIndex, root->key, root, el->line);
		if (recordType(root) == GRPerson) {
			addToRecordIndex(personIndex, root->key, root, el->line);
			insertInRootList(personRoots, root);
		} else if (recordType(root) == GRFamily) {
			addToRecordIndex(familyIndex, root->key, root, el->line);
			insertInRootList(familyRoots, root);
		}
	ENDLIST
	deleteGNodeList(rootList, false);
	// Create the Database and add the indexes.
	Database* database = createDatabase(path);
	database->recordIndex = recordIndex;
	database->personIndex = personIndex;
	database->familyIndex = familyIndex;
	database->personRoots = personRoots;
	database->familyRoots = familyRoots;
	if (importDebugging) summarizeDatabase(database);
	if (timing) printf("%s: database created.\n", getMillisecondsString());
	// Validate persons and families.
	validatePersons(database, log);
	if (timing) printf("%s: validated persons.\n", getMillisecondsString());
	validateFamilies(database, log);
	if (timing) printf("%s: validated families.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteDatabase(database);
		return null;
	}
	// Create the name index.
	getNameIndexForDatabase(database);
	if (timing) printf("%s: indexed names.\n", getMillisecondsString());
	// Create the REFN index and validate it.
	validateReferences(database, log);
	if (timing) printf("Indexed REFNs: %s.\n", getMillisecondsString());
	if (timing) printf("%s: end of gedcomFileToDatabase.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteDatabase(database);
		return null;
	}
	return database;
}

// checkKeysAndReferences checks record keys and their references. Creates a table of all keys
// and checks for duplicates. Checks that all keys found as values refer to records. Returns
// the set of all keys detected.
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
	deleteStringSet(keySet, false);
}

