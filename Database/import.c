//
//  DeadEnds
//
//  import.c -- Read Gedcom files and build a database from them.
//
//  Created by Thomas Wetmore on 13 November 2022.
//  Last changed on 12 December 2023.
//

#include <unistd.h> // access
#include <errno.h> // ENOENT
#include <sys/param.h> // PATH_MAX
#include <stdlib.h> // realpath
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

//  importFromFiles -- Import Gedcom files into a List of Databases, one Database per file. The
//    List is returned. If errors were found in any of the files, the List of Databases will not
//    hold a Database for that file, and the ErrorLog will hold the List of Errors detected.      
//--------------------------------------------------------------------------------------------------
List *importFromFiles(String filePaths[], int count, ErrorLog *errorLog)
//  filePaths -- Paths to the files to import.
//  count -- Number of files to import.
//  errorLog -- ErrorLog hold all Errors found during import.
{
	List *listOfDatabases = createList(null, null, null);
	Database *database = null;

	for (int i = 0; i < count; i++) {
		if ((database = importFromFile(filePaths[i], errorLog)))
			appendListElement(listOfDatabases, database);
	}
	return listOfDatabases;
}

//  importFromFile -- Import the records in a Gedcom file into a new Database. If erros were
//    found the function returns null, and the Errors found will be appended to the ErrorLog.
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
	char pathBuffer[PATH_MAX];
	String realPath = realpath(filePath, pathBuffer);
	if (realPath) filePath = strsave(pathBuffer);

	String lastSegment = lastPathSegment(filePath); // MNOTE: strsave not needed.

	//  Make sure the file can be opened.
	FILE *file = fopen(filePath, "r");
	if (!file) {
		addErrorToLog(errorLog, createError(systemError, lastSegment, 0, "Could not open file."));
		return null;
	}

	Database *database = createDatabase(filePath);
	//int recordCount = 0;
	//int lineNo; // Line number kept up to date by the nodeTreeFromFile functions.

	// Read the lines from the Gedcom file into a NodeList of GNodes and Errors.
	NodeList *listOfNodes = getNodeListFromFile(file);

	// Convert the list of GNodes into a NodeList of GNode trees, to become the records in the
	//   database.
	NodeList *listOfTrees = getNodeTreesFromNodeList(listOfNodes, errorLog);

	// During the testing phase, bail out if there are any errors.
	int numErrors = numberErrorsInNodeList(listOfTrees);
	if (numErrors > 0) {
		printf("There are %d errors in the listOfTrees. Bailing out for the time being.\n", numErrors);
		exit(1);  // Bail at this point in the testing.
	}

	// Add the node trees to the database.
	FORLIST(listOfTrees, element)
		NodeListElement *e = (NodeListElement*) element;
		storeRecord(database, normalizeNodeTree(e->node), e->lineNo, errorLog);
	ENDLIST

	if (debugging) {
		//printf("Read %d records.\n", recordCount);
		printf("There were %d errors importing file %s.\n", lengthList(errorLog), lastSegment);
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
