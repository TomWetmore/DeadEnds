// DeadEnds
//
// database.h is the header file for the Database type.
//
// Created by Thomas Wetmore on 10 November 2022.
// Last changed on 13 December 2024.

#ifndef database_h
#define database_h

#include "standard.h"
#include "hashtable.h"
#include "recordindex.h"
#include "nameindex.h"
#include "refnindex.h"
#include "gnode.h"
#include "errors.h"
#include "rootlist.h"

typedef HashTable RecordIndex; // Forward references.
typedef HashTable NameIndex;
typedef List RootList;

// DBaseAction is a "Database action" that customizes Database processing.
typedef void (*DBaseAction)(Database*, ErrorLog*);

// Database is the structure that hold DeadEnds databases.
typedef struct Database {
	String filePath;  // Path to Gedcom file this Database was built from.
	String name; // Use last segment of the path for the name of the Database.
	GNode* header; // Root of header record.
	RecordIndex* recordIndex; // Index of all keyed records.
	NameIndex *nameIndex; // Index of the names of the persons in this database.
	RefnIndex *refnIndex; // Index of the REFN values in this database.
	RootList *personRoots; // List of all person roots in the database.
	RootList *familyRoots; // List of all family roots in the database.
} Database;

Database *createDatabase(String fileName); // Create an empty database.
void deleteDatabase(Database*); // Delete a database.
void writeDatabase(String fileName, Database*);

void indexNames(Database*);      // Index person names after reading the Gedcom file.
int numberPersons(Database*);    // Return the number of persons in the database.
int numberFamilies(Database*);   // Return the number of families in the database.
int numberSources(Database*);    // Return the number of sources in the database.
int numberEvents(Database*);     // Return the number of events in the database.
int numberOthers(Database*);     // Return the number of other records in the database.
bool isEmptyDatabase(Database*);  // Return true if the database has not persons or families.
GNode *keyToPerson(String key, RecordIndex*); // Get a person from record index.
GNode *keyToFamily(String key, RecordIndex*); // Get a family GNode from a RecordIndex.
GNode *keyToSource(String key, RecordIndex*); // Get a source record from the database.
GNode *keyToEvent(String key, RecordIndex*); // Get an event record from the database.
GNode *keyToOther(String key, RecordIndex*); // Get an other record from the database.
GNode *getRecord(String key, RecordIndex*);  // Get an arbitraray record from the database.
bool storeRecord(Database*, GNode*, int lineno, ErrorLog*); // Add a record to the database.
void summarizeDatabase(Database*);

String generateFamilyKey(Database*);
String generatePersonKey(Database*);

#endif // database_h
