//
// DeadEnds
//
// import.c has the functions that imports Gedcom files into Databases.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 30 March 2024.
//

#include <unistd.h> // access
//#include <errno.h> // ENOENT
#include <sys/param.h> // PATH_MAX
#include <sys/errno.h> // errno
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
extern FILE* debugFile;

// toString returns the GNode in a NodeListElement as a string; for debugging.
static String toString(void* element) {
	GNode* gnode = ((NodeListElement*) element)->node;
	return gnodeToString(gnode, 0);
}

// Error messages defined elsewhere.
extern String idgedf, gdcker, gdnadd, dboldk, dbnewk, dbodel, cfoldk, dbdelk, dbrdon;

static GNode *normalizeNodeTree (GNode*);
static bool debugging = true;
bool importDebugging = true;

// importFromFiles imports a list of Gedcom files into a List of Databases, one per file. The list
// is returned. If errors are found in a file, the list will not have a Database for that file,
//  and the ErrorLog will hold the Errors.
List *importFromFiles(String filePaths[], int count, ErrorLog *errorLog)
{
	List *listOfDatabases = createList(null, null, null, false);
	Database *database = null;

	for (int i = 0; i < count; i++) {
		if ((database = importFromFile(filePaths[i], errorLog)))
			appendToList(listOfDatabases, database);
	}
	return listOfDatabases;
}

// importFromFile imports the records in a Gedcom file into a new Database. If errors are found
// the function returns null, and the ErrorLog holds the Errors.
Database *importFromFile(String filePath, ErrorLog *errorLog) {
	if (importDebugging) printf("    IMPORT FROM FILE: start: %s\n", filePath);
	if (access(filePath, F_OK)) {
		if (errno == ENOENT) {
			addErrorToLog(errorLog, createError(systemError, filePath, 0, "File does not exist."));
			return null;
		}
	}
	char pathBuffer[PATH_MAX];
	String realPath = realpath(filePath, pathBuffer);
	if (realPath) filePath = strsave(pathBuffer);
	String lastSegment = lastPathSegment(filePath);
	FILE *file = fopen(filePath, "r");
	if (!file) {
		addErrorToLog(errorLog, createError(systemError, lastSegment, 0, "Could not open file."));
		return null;
	}

	// Get the lines of the Gedcom file as a NodeList of GNodes and Errors.
	if (importDebugging) fprintf(debugFile, "importFromFile: calling getNodeListFromFile(%s,...\n", filePath);
	int numErrors = 0;
	NodeList *listOfNodes = getNodeListFromFile(file, &numErrors);
	if (!listOfNodes) return null;
	if (importDebugging) fprintf(debugFile, "importFromFile: back from getNodeListFromFile\n");
	if (importDebugging) {
		fprintf(debugFile, "importFromFile: listOfNodes contains\n");
		//fprintfBlock(debugFile, &(listOfNodes->block), toString);
	}

	// Convert the NodeList of GNodes and Errors into a NodeList of GNode trees.
	if (importDebugging) fprintf(debugFile, "importFromFile: calling getNodeTreesFromNodeList\n");
	NodeList *listOfTrees = getNodeTreesFromNodeList(listOfNodes, errorLog);
	if (importDebugging) fprintf(debugFile, "importFromFile: back from getNodeTreesFromNodeList\n");
	if (importDebugging) {
		fprintf(debugFile, "importFromFile: listOfGTrees contains\n");
		//fprintfBlock(debugFile, &(listOfTrees->block), toString);
	}
	if (numErrors) { // Temporary early exit.
		printf("There are %d errors in the listOfTrees. Bailing for the time being.\n", numErrors);
		exit(1);
	}
	Database *database = createDatabase(filePath); // Create database and add records to it.
	FORLIST(listOfTrees, element)
		NodeListElement *e = (NodeListElement*) element;
		storeRecord(database, normalizeNodeTree(e->node), e->lineNo, errorLog);
	ENDLIST


	printf("And now it is time to sort those RootList\n");
	sortList(database->personRoots);
	printf("Persons have been sorted\n");
	sortList(database->familyRoots);
	printf("Families have been sorted\n");

	if (debugging) {
		printf("There were %d gnode tree records extracted from the file.\n", numberNodesInNodeList(listOfTrees));
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
