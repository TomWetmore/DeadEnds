//
//  DeadEnds Library
//
//  database.c has functions that provide an in-RAM database for Gedcom records. Each record is a
//  GNode tree. The backing store is a Gedcom file. When DeadEnds starts the Gedcom file is read
//  and used to build an internal database.
//
//  Created by Thomas Wetmore on 10 November 2022.
//  Last changed on 4 June 2025.
//

#include "database.h"
#include "errors.h"
#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "import.h"
#include "name.h"
#include "nameindex.h"
#include "path.h"
#include "recordindex.h"
#include "refnindex.h"
#include "rootlist.h"
#include "stringtable.h"
#include "validate.h"
#include "writenode.h"

extern bool importDebugging;
bool indexNameDebugging = false;

// createDatabase creates a database.
Database *createDatabase(String path, RootList* records, IntegerTable* keymap, ErrorLog* errlog) {
	Database *database = (Database*) stdalloc(sizeof(Database));
	database->path = strsave(path);
	database->name = strsave(lastPathSegment(path));
	database->dirty = false;
    database->recordIndex = createRecordIndex();
    database->personRoots = createRootList();
    database->familyRoots = createRootList();
    database->sourceRoots = createRootList();
    database->eventRoots = createRootList();
    database->otherRoots = createRootList();

    FORLIST(records, element)
        GNode* root = (GNode*) element;
        if (root->key) addToRecordIndex(database->recordIndex, root);
        RecordType rtype = recordType(root);
        if (rtype == GRPerson) insertInRootList(database->personRoots, root);
        if (rtype == GRFamily) insertInRootList(database->familyRoots, root);
        if (rtype == GRSource) insertInRootList(database->sourceRoots, root);
        if (rtype == GREvent) insertInRootList(database->eventRoots, root);
        if (rtype == GROther) insertInRootList(database->otherRoots, root);
    ENDLIST
    deleteRootList(records);
	database->nameIndex = getNameIndex(database->personRoots);
    database->refnIndex = getReferenceIndex(database->recordIndex, path, keymap, errlog);
	return database;
}

// deleteDatabase deletes a Database.
void deleteDatabase(Database* database) {
	if (database->recordIndex) deleteRecordIndex(database->recordIndex);
	if (database->nameIndex) deleteNameIndex(database->nameIndex);
	if (database->refnIndex) deleteRefnIndex(database->refnIndex);
	if (database->personRoots) deleteList(database->personRoots);
	if (database->familyRoots) deleteList(database->familyRoots);
    if (database->sourceRoots) deleteList(database->sourceRoots);
    if (database->eventRoots) deleteList(database->eventRoots);
    if (database->otherRoots) deleteList(database->otherRoots);
}

// writeDatabase writes the contents of a Database to a Gedcom file.
void writeDatabase(String fileName, Database* database) {
	FILE* file = fopen(fileName, "w");
	if (file == null) {
		printf("Can't open file to write the database\n");
		return;
	}
    writeGNodeRecord(file, database->header, false);
	FORHASHTABLE(database->recordIndex, element)
		writeGNodeRecord(file, (GNode*) element, false);
	ENDHASHTABLE
    fprintf(file, "0 TRLR\n");
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

// getRecord gets a record from the database given a key.
GNode* getRecord(String key, RecordIndex* index) {
	return searchRecordIndex(index, key);
}

// summarizeDatabase writes a short summary of a Database to standard output.
void summarizeDatabase(Database* database) {
	if (!database) {
		printf("Database does not exist.\n");
		return;
	}
	printf("Summary of database: %s.\n", database->path);
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
