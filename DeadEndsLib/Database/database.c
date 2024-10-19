// DeadEnds
//
// database.c has functions that provide an in-RAM database for Gedcom records. Each record is a
// GNode tree. The backing store is a Gedcom file. When DeadEnds starts the Gedcom file is read
// and used to build an internal database.
//
// Created by Thomas Wetmore on 10 November 2022.
// Last changed 12 October 2024.

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

extern bool importDebugging;
bool indexNameDebugging = false;

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
	if (database->recordIndex) deleteRecordIndex(database->recordIndex);
	if (database->personIndex) deleteRecordIndex(database->personIndex);
	if (database->familyIndex) deleteRecordIndex(database->familyIndex);
	if (database->nameIndex) deleteNameIndex(database->nameIndex);
	if (database->refnIndex) deleteRefnIndex(database->refnIndex);
	if (database->personRoots) deleteList(database->personRoots);
	if (database->familyRoots) deleteList(database->familyRoots);
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
	FORHASHTABLE(database->sourceIndex, element)
		writeGNodeRecord(file, ((RecordIndexEl*) element)->root, false);
	ENDHASHTABLE
//	FORHASHTABLE(database->eventIndex, element)
//		writeGNodeRecord(file, ((RecordIndexEl*) element)->root, false);
//	ENDHASHTABLE
	FORHASHTABLE(database->otherIndex, element)
		writeGNodeRecord(file, ((RecordIndexEl*) element)->root, false);
	ENDHASHTABLE
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

// numberEvents returns the number of (top level) events in the database.
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

// keyToRecordOfType returns the root of the GNode tree with given key and record type.
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

// showTableSizes is a debug function that shows the sizes of the database tables.
void showTableSizes(Database *database) {
	printf("Size of recordIndex: %d\n", sizeHashTable(database->recordIndex));
	printf("Size of personIndex: %d\n", sizeHashTable(database->personIndex));
	printf("Size of familyIndex: %d\n", sizeHashTable(database->familyIndex));
	printf("Size of sourceIndex: %d\n", sizeHashTable(database->sourceIndex));
	printf("Size of eventIndex:  %d\n", sizeHashTable(database->eventIndex));
	printf("Size of otherIndex:  %d\n", sizeHashTable(database->otherIndex));
}

// getNameIndexForDatabase indexes all person names in a database.
void getNameIndexForDatabase(Database* database) {
	int numNamesFound = 0; // For debugging.
	NameIndex* nameIndex = createNameIndex();
	FORHASHTABLE(database->personIndex, element) // Loop over all persons.
		RecordIndexEl* el = element;
		GNode* root = el->root;
		String recordKey = root->key; // Key of record, used as is in name index.
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				numNamesFound++; // For debugging.
				String nameKey = nameToNameKey(name->value); // MNOTE: points to static memory.
				insertInNameIndex(nameIndex, nameKey, recordKey);
			}
		}
	ENDHASHTABLE
	database->nameIndex = nameIndex;
	if (indexNameDebugging) printf("the number of names encountered is %d.\n", numNamesFound);
}

// keyLineNumber returns the line in the Gedcome file where the record with the given key began.
// If the key does not exist returns 0.
//static int keyLineNumber (Database *database, String key) {
//	RecordIndexEl* el = (RecordIndexEl*) searchHashTable(database->recordIndex, key);
//	if (!el) return 0;
//	return el->line;
//}

// getRecord gets a record from the database given a key.
GNode* getRecord(String key, Database* database) {
	return searchRecordIndex(database->recordIndex, key);
}

// summarizeDatabase writes a short summary of a Database to standard output.
void summarizeDatabase(Database* database) {
	if (!database) {
		printf("No database to summarize.\n");
		return;
	}
	printf("Summary of database: %s.\n", database->filePath);
	if (database->personIndex) printf("\tPerson index: %d records.\n", sizeHashTable(database->personIndex));
	if (database->familyIndex) printf("\tFamily index: %d records.\n", sizeHashTable(database->familyIndex));
	if (database->recordIndex) printf("\tRecord index: %d records.\n", sizeHashTable(database->recordIndex));
	if (database->nameIndex) {
		int numNames, numRecords;
		getNameIndexStats(database->nameIndex, &numNames, &numRecords);
		printf("\tName index: %d name keys in %d records.\n", numNames, numRecords);

	}
}
