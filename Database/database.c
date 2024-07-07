// DeadEnds
//
// database.c holds the functions that provide an in-RAM database for Gedcom records. Each record
// is a GNode tree. The backing store is a Gedcom file. When DeadEnds starts the Gedcom file is
// read and used to build an internal database.
//
// Created by Thomas Wetmore on 10 November 2022.
// Last changed 5 July 2024.

#include "database.h"
#include "gnode.h"
#include "name.h"
#include "recordindex.h"
#include "stringtable.h"
#include "nameindex.h"
#include "path.h"
#include "errors.h"
#include "rootlist.h"
#include "writenode.h"

extern FILE* debugFile;
extern bool importDebugging;
bool indexNameDebugging = true;
static int keyLineNumber(Database*, String key);

// createDatabase creates a database.
Database *createDatabase(String filePath) {
	Database *database = (Database*) stdalloc(sizeof(Database));
	database->filePath = strsave(filePath);
	database->lastSegment = strsave(lastPathSegment(filePath));
	database->recordIndex = null;
	database->personIndex = null;
	database->familyIndex = null;
	database->nameIndex = null;
	database->refnIndex = null;
	database->personRoots = createRootList(); // null?
	database->familyRoots = createRootList(); // null?
	return database;
}

//  deleteDatabase deletes a database.
void deleteDatabase(Database* database) {
	deleteRecordIndex(database->recordIndex);
	deleteRecordIndex(database->personIndex);
	deleteRecordIndex(database->familyIndex);
	//deleteRecordIndex(database->sourceIndex);
	//deleteRecordIndex(database->eventIndex);
	//deleteRecordIndex(database->otherIndex);
	deleteNameIndex(database->nameIndex);
}

// writeDatabase writes the contents of a Database to a Gedcom file.
void writeDatabase(String fileName, Database* database) {
	FILE* file = fopen(fileName, "w");
	if (file == null) {
		printf("Can't open file to write database to\n");
		return;
	}
	FORLIST(database->personRoots, element)
		writeGNodeRecord(file, (GNode*) element, false);
	ENDLIST
	FORLIST(database->familyRoots, element)
		writeGNodeRecord(file, (GNode*) element, false);
	ENDLIST
	fclose(file);
}

// numberRecordsOfType returns the number of records of given type.
static int numberRecordsOfType(Database* database, RecordType recType) {
	int numRecords = 0;
	FORHASHTABLE(database->recordIndex, element)
	RecordIndexEl* el = (RecordIndexEl*) element;
	if (recordType(el->root) == recType) numRecords++;
	ENDHASHTABLE
	return numRecords;
}

// numberPersons returns the number of persons in a database.
int numberPersons(Database* database) {
	return numberRecordsOfType(database, GRPerson);
}

// numberFamilies returns the number of families in a database.
int numberFamilies(Database* database) {
	return numberRecordsOfType(database, GRFamily);
}

// numberSources returns the number of sources in a database.
int numberSources(Database* database) {
	return numberRecordsOfType(database, GRSource);
}

// numberEvents returnw the number of (top level) events in the database.
int numberEvents(Database* database) {
	return numberRecordsOfType(database, GREvent);
}

// numberOthers return the number of other records in the database.
int numberOthers(Database* database) {
	return numberRecordsOfType(database, GROther);
}

// isEmptyDatabase returns true if the database has no persons or families.
bool isEmptyDatabase(Database* database) {
	return numberPersons(database) + numberFamilies(database) == 0;
}

static GNode* keyToRecordOfType(String key, Database* database, RecordType recType) {
	RecordIndexEl* el = (RecordIndexEl*) searchHashTable(database->recordIndex, key);
	if (el == null) return null;
	GNode* node = el->root;
	if (recordType(node) != recType) return null;
	return node;
}

// keyToPerson gets a person record from a database.
GNode* keyToPerson(String key, Database* database) {
	return keyToRecordOfType(key, database, GRPerson);
}

// keyToFamily gets a family record from a database.
GNode* keyToFamily(String key, Database* database) {
	return keyToRecordOfType(key, database, GRFamily);
}

// keyToSource gets a source record from a database.
GNode* keyToSource(String key, Database* database) {
	return keyToRecordOfType(key, database, GRSource);
}

// keyToEvent gets an event record from a database.
GNode* keyToEvent(String key, Database* database) {
	return keyToRecordOfType(key, database, GREvent);
}

// keyToOther gets an other record from a database.
GNode* keyToOther(String key, Database* database) {
	return keyToRecordOfType(key, database, GROther);
}

// storeRecord stores a GNode tree/record in a database by adding it to a RecordIndex.
// lineNumber is the line number of the root node in the Gedcom file.
bool storeRecord(Database* database, GNode* root, int lineNumber, ErrorLog* errorLog) {
	RecordType type = recordType(root);
	if (type == GRHeader || type == GRTrailer) return true; // Ignore HEAD and TRLR.
	if (!root->key) {
		Error *error = createError(syntaxError, database->lastSegment, lineNumber, "This record has no key.");
		addErrorToLog(errorLog, error);
		return false;
	}
	String key = root->key; // MNOTE: insertInRecord copies the key.
	int previousLine = keyLineNumber(database, key); // Duplicate key check.
	if (previousLine) {
		char scratch[MAXLINELEN];
		sprintf(scratch, "A record with key %s exists at line %d.", key, previousLine);
		Error *error = createError(gedcomError, database->lastSegment, lineNumber, scratch);
		addErrorToLog(errorLog, error);
	}
	switch (type) {
		case GRPerson:
			addToRecordIndex(database->personIndex, key, root, lineNumber);
			insertInRootList(database->personRoots, root);
			return true;
		case GRFamily:
			addToRecordIndex(database->familyIndex, key, root, lineNumber);
			insertInRootList(database->familyRoots, root);
			return true;
		case GRSource:
			addToRecordIndex(database->sourceIndex, key, root, lineNumber);
			return true;
		case GREvent:
			addToRecordIndex(database->eventIndex, key, root, lineNumber);
			return true;
		case GROther:
			addToRecordIndex(database->otherIndex, key, root, lineNumber);
			return true;
		default:
			ASSERT(false);
			return false;
	}
}

// showTableSizes is a debug function that shows the sizes of the database tables.
void showTableSizes(Database *database) {
	printf("Size of recordIndex: %d\n", sizeHashTable(database->recordIndex));
	printf("Size of personIndex: %d\n", sizeHashTable(database->personIndex));
	printf("Size of familyIndex: %d\n", sizeHashTable(database->familyIndex));
	printf("Size of sourceIndex: %d\n", sizeHashTable(database->sourceIndex));
	printf("Size of eventIndex:  %d\n", sizeHashTable(database->eventIndex));
	printf("Size of otherIndex:  %d\n", sizeHashTable(database->otherIndex));
}

// indexNames indexes all person names in a database.
void oldIndexNames(Database* database) {
	if (indexNameDebugging) fprintf(debugFile, "Start indexNames\n");
	static int count = 0;
	int i, j;
	RecordIndexEl* entry = firstInHashTable(database->recordIndex, &i, &j);
	for (; entry; entry = nextInHashTable(database->recordIndex, &i, &j)) {
		GNode* root = entry->root;
		if (recordType(root) != GRPerson) continue;
		String recordKey = root->key;
		if (indexNameDebugging) fprintf(debugFile, "indexNames: recordKey: %s\n", recordKey);
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				if (indexNameDebugging) fprintf(debugFile, "indexNames: name->value: %s\n", name->value);
				// MNOTE: nameKey is in data space. It is heapified in insertInNameIndex.
				String nameKey = nameToNameKey(name->value);
				if (indexNameDebugging) fprintf(debugFile, "indexNames: nameKey: %s\n", nameKey);
				insertInNameIndex(database->nameIndex, nameKey, recordKey);
				count++;
			}
		}
		//entry = nextInHashTable(database->personIndex, &i, &j);
	}
	if (indexNameDebugging) showNameIndex(database->nameIndex);
	if (indexNameDebugging) printf("The number of names indexed was %d\n", count);
}

// indexNames indexes all person names in a database. NEW VERSION HASN'T BEEN TESTED.
void indexNames(Database* database) {
	//if (indexNameDebugging) fprintf(debugFile, "Start indexNames\n");
	int numNamesEncountered = 0;
	NameIndex* nameIndex = createNameIndex();
	FORHASHTABLE(database->personIndex, element)
		RecordIndexEl* el = element;
		GNode* root = el->root;
		String recordKey = root->key;
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				numNamesEncountered++;
				String nameKey = nameToNameKey(name->value);
				insertInNameIndex(nameIndex, nameKey, recordKey);
			}
		}
	ENDHASHTABLE
	database->nameIndex = nameIndex;
	//if (indexNameDebugging) showNameIndex(database->nameIndex);
	if (indexNameDebugging) printf("the number of names encountered is %d.\n", numNamesEncountered);
}

// keyLineNumber checks if a record with a key is in the database; if so it returns the line
// number where the record began in its Gedcom file.
static int keyLineNumber (Database *database, String key) {
	RecordIndexEl* el = (RecordIndexEl*) searchHashTable(database->recordIndex, key);
//	if (!element) element = searchHashTable(database->familyIndex, key);
//	if (!element) element = searchHashTable(database->sourceIndex, key);
//	if (!element) element = searchHashTable(database->eventIndex, key);
//	if (!element) element = searchHashTable(database->otherIndex, key);
	if (!el) return 0; // Record doesn't exist.
	return el->line;
}

// getRecord gets a record from the database given a key.
GNode* getRecord(String key, Database* database) {
	return searchRecordIndex(database->recordIndex, key);
//	GNode *root = keyToRecord(key, database);
//
//	if ((root = keyToPerson(key, database))) return root;
//	if ((root = keyToFamily(key, database))) return root;
//	if ((root = keyToSource(key, database))) return root;
//	if ((root = keyToEvent(key, database))) return root;
//	if ((root = keyToOther(key, database))) return root;
//	return null;
}

//  Some debugging functions.
//void showPersonIndex(Database *database) { showHashTable(database->personIndex, null); }
//void showFamilyIndex(Database *database) { showHashTable(database->familyIndex, null); }
//int getCount(void) { return count; }


// generateFamilyKey generates a new family key.
String generateFamilyKey(Database* database) {
	return "@Fxxxxxxx@";
}

// generatePersonKey generates a new person key.
String generatePersonKey(Database* database) {
	return "@Ixxxxxxx@";
}

// summarizeDatabase write a short text summary of a Database to standard output. Debug.
void summarizeDatabase(Database* database) {
	if (!database) {
		printf("No database to summarize.\n");
		return;
	}
	printf("summary of database: %s.\n", database->filePath);
	if (database->personIndex) printf("Person index: %d\n", sizeHashTable(database->personIndex));
	if (database->familyIndex) printf("Family index: %d\n", sizeHashTable(database->familyIndex));
	if (database->recordIndex) printf("Record index: %d\n", sizeHashTable(database->recordIndex));
	if (database->nameIndex) {
		int numNames, numRecords;
		getNameIndexStats(database->nameIndex, &numNames, &numRecords);
		printf("Name index: %d, %d\n", numNames, numRecords);

	}
}
