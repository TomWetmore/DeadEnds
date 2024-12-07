// DeadEnds
//
// database.c has functions that provide an in-RAM database for Gedcom records. Each record is a
// GNode tree. The backing store is a Gedcom file. When DeadEnds starts the Gedcom file is read
// and used to build an internal database.
//
// Created by Thomas Wetmore on 10 November 2022.
// Last changed 4 December 2024.

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
	database->nameIndex = null;
	database->refnIndex = null;
	database->personRoots = createRootList(); // null?
	database->familyRoots = createRootList(); // null?
	return database;
}

// deleteDatabase deletes a Database.
void deleteDatabase(Database* database) {
	if (database->recordIndex) deleteRecordIndex(database->recordIndex);
	if (database->nameIndex) deleteNameIndex(database->nameIndex);
	if (database->refnIndex) deleteRefnIndex(database->refnIndex);
	if (database->personRoots) deleteList(database->personRoots);
	if (database->familyRoots) deleteList(database->familyRoots);
}

// writeDatabase writes the contents of a Database to a Gedcom file.
void writeDatabase(String fileName, Database* database) {
	FILE* file = fopen(fileName, "w");
	if (file == null) {
		printf("Can't open file to write the database\n");
		return;
	}
	FORHASHTABLE(database->recordIndex, element)
		writeGNodeRecord(file, (GNode*) element, false);
	ENDHASHTABLE
	fclose(file);
}

// numberRecordsOfType returns the number of records of given type.
static int numberRecordsOfType(Database* database, RecordType recType) {
	int numRecords = 0;
	FORHASHTABLE(database->recordIndex, element)
		if (recordType((GNode*) element) == recType) numRecords++;
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
static GNode* keyToRecordOfType(String key, RecordIndex* index, RecordType recType) {
	GNode* gnode = searchRecordIndex(index, key);
	if (!gnode) return null;
	if (recordType(gnode) != recType) return null;
	return gnode;
}

// keyToPerson gets a person record from a database.
GNode* keyToPerson(String key, RecordIndex* index) {
	return keyToRecordOfType(key, index, GRPerson);
}

// keyToFamily gets a family record from a database.
GNode* keyToFamily(String key, RecordIndex* index) {
	return keyToRecordOfType(key, index, GRFamily);
}

// keyToSource gets a source record from a database.
GNode* keyToSource(String key, RecordIndex* index) {
	return keyToRecordOfType(key, index, GRSource);
}

// keyToEvent gets an event record from a database.
GNode* keyToEvent(String key, RecordIndex* index) {
	return keyToRecordOfType(key, index, GREvent);
}

// keyToOther gets an other record from a database.
GNode* keyToOther(String key, RecordIndex* index) {
	return keyToRecordOfType(key, index, GROther);
}

// getNameIndexFromPersons indexes all person names in a database.
NameIndex* getNameIndexFromPersons(RootList* persons) {
	int numNamesFound = 0; // Debugging.
	NameIndex* nameIndex = createNameIndex();
	FORLIST(persons, element) // Loop over persons.
		GNode* root = (GNode*) element;
		String recordKey = root->key; // Key of record, used as is in name index.
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				numNamesFound++; // For debugging.
				String nameKey = nameToNameKey(name->value); // MNOTE: points to static memory.
				insertInNameIndex(nameIndex, nameKey, recordKey);
			}
		}
	ENDLIST
	if (indexNameDebugging) printf("the number of names encountered is %d.\n", numNamesFound);
	return nameIndex;
}

// getNameIndexFromRecordIndex indexes all person names in a RecordIndex and returns the NameIndex.
NameIndex* getNameIndexFromRecordIndex(RecordIndex* index) {
	int numNamesFound = 0; // Debugging.
	NameIndex* nameIndex = createNameIndex();
	FORHASHTABLE(index, element) // Loop over all persons.
		GNode* root = (GNode*) element;
		String recordKey = root->key; // Key of record, used as is in name index.
		for (GNode* name = NAME(root); name && eqstr(name->tag, "NAME"); name = name->sibling) {
			if (name->value) {
				numNamesFound++; // Debugging.
				String nameKey = nameToNameKey(name->value); // MNOTE: points to static memory.
				insertInNameIndex(nameIndex, nameKey, recordKey);
			}
		}
	ENDHASHTABLE
	if (indexNameDebugging) printf("the number of names encountered is %d.\n", numNamesFound);
	return nameIndex;
}


// getRecord gets a record from the database given a key.
GNode* getRecord(String key, Database* database) {
	return searchRecordIndex(database->recordIndex, key);
}

// summarizeDatabase writes a short summary of a Database to standard output.
void summarizeDatabase(Database* database) {
	if (!database) {
		printf("Database does not exist.\n");
		return;
	}
	printf("Summary of database: %s.\n", database->filePath);
	if (database->recordIndex) {
		printf("\tThe record index has %d records.\n", sizeHashTable(database->recordIndex));
		printf("\tThere are %d persons and %d families in the database.\n",
			   numberPersons(database), numberFamilies(database));
	}
	if (database->nameIndex) {
		int numNames, numRecords;
		getNameIndexStats(database->nameIndex, &numNames, &numRecords);
		printf("\tName index: %d name keys and %d record keys.\n", numNames, numRecords);
	}
}
