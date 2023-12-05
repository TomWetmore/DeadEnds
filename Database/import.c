//
//  DeadEnds
//
//  import.c -- Read Gedcom files and build a database from them.
//
//  Created by Thomas Wetmore on 13 November 2022.
//  Last changed on 1 December 2023.
//

#include <unistd.h> // access
#include <errno.h> // ENOENT
#include "standard.h"
#include "import.h"
#include "gnode.h"
#include "stringtable.h"
#include "recordindex.h"
#include "gedcom.h"
#include "splitjoin.h"
#include "database.h"
#include "validate.h"
#include "errors.h"
#include "readnode.h"
#include "path.h"

extern int errno; // UNIX error code.

extern bool validateIndex(RecordIndex *index);
static String updateKeyMap(GNode *root, StringTable* keyMap);
static void rekeyIndex(RecordIndex*, StringTable *keyMap);
static void setupDatabase(List *recordIndexes);
static void addIndexToDatabase(RecordIndex *index, Database *database);

Database *importFromFile(String, ErrorLog*);

// Error messages defined elsewhere.
extern String idgedf, gdcker, gdnadd, dboldk, dbnewk, dbodel, cfoldk, dbdelk, dbrdon;

static GNode *normalizeNodeTree (GNode*);
static bool debugging = true;

//  importFromFiles -- Import Gedcom files into a list of Databases.
//--------------------------------------------------------------------------------------------------
List *importFromFiles(String filePaths[], int count, ErrorLog *errorLog)
//  filesPaths -- Paths to the files to import.
//  count -- Number of files to import.
//  errorLog -- Error log.
{
	List *listOfDatabases = createList(null, null, null);
	Database *database = null;

	for (int i = 0; i < count; i++) {
		if ((database = importFromFile(filePaths[i], errorLog)))
			appendListElement(listOfDatabases, database);
	}
	return listOfDatabases;
}

//  importFromFile -- Import the records in a Gedcom file into a Database.
//--------------------------------------------------------------------------------------------------
Database *importFromFile(String filePath, ErrorLog *errorLog)
{
	//  Make sure the file exists.
	if (debugging) printf("    IMPORT FROM FILE: %s\n", filePath);
	if (access(filePath, F_OK)) {
		if (errno == ENOENT) {
			addErrorToLog(errorLog, createError(systemError, filePath, 0, "File does not exist."));
			return null;
		}
	}
	String fileName = lastPathSegment(filePath); // MNOTE: strsave not needed.

	//  Make sure the file can be opened.
	FILE *file = fopen(filePath, "r");
	if (!file) {
		addErrorToLog(errorLog, createError(systemError, fileName, 0, "Could not open file."));
		return null;
	}

	Database *database = createDatabase(fileName);
	int recordCount = 0;
	int lineNo; // Line number kept up to date by the nodeTreeFromFile functions.

	//  Read the records and add them to the database.
	GNode *root = firstNodeTreeFromFile(file, fileName, &lineNo, errorLog);
	while (root) {
		storeRecord(database, normalizeNodeTree(root), lineNo, errorLog);
		recordCount += 1;
		root = nextNodeTreeFromFile(file, &lineNo, errorLog);
	}
	if (debugging) {
		printf("Read %d records.\n", recordCount);
		printf("There were %d errors importing file %s.\n", lengthList(errorLog), fileName);
		showErrorLog(errorLog);
	}
	if (lengthList(errorLog) > 0) {
		deleteDatabase(database);
		return null;
	}
	return database;
}

String misnam = (String) "Missing NAME line in INDI record; record ignored.\n";
String noiref = (String) "FAM record has no INDI references; record ignored.\n";

// normalizeNodeTree -- Normalize node tree records to standard format.
//--------------------------------------------------------------------------------------------------
static GNode *normalizeNodeTree (GNode *root)
//  root -- Root of a Gedcom tree record.
{
	switch (recordType(root)) {
		case GRHeader: return root;
		case GRTrailer: return root;
		case GRPerson: return normalizePerson(root);
		case GRFamily:  return normalizeFamily(root);
		case GREvent: return normalizeEvent(root);
		case GRSource: return normalizeSource(root);
		case GROther: return normalizeOther(root);
		default: FATAL();
	}
	return null;
}