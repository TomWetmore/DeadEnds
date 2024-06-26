// DeadEnds
//
// importone.c has a test program for importing a single Gedcom file into a Database.
//
// Created by Thomas Wetmore on 21 June 2024.
// Last changed on 26 June 2024.

#include "standard.h"
#include "errors.h"

//#include <unistd.h>
#include <errno.h>
//#include <sys/param.h>
//#include <sys/errno.h>
//#include <stdlib.h>
//#include "import.h"
//#include "gnode.h"
//#include "stringtable.h"
//#include "recordindex.h"
//#include "gedcom.h"
#include "splitjoin.h"
//#include "database.h"
//#include "validate.h"
//#include "readnode.h"
#include "gnodelist.h"
#include "path.h"

static bool importDebugging = true;

GNodeList* newImportFromFile(String, ErrorLog*);

int importTest(void) {
	String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/051224.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/smallish.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/parenterror.ged";
	ErrorLog* errorLog = createErrorLog();
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	GNodeList* recordList = newImportFromFile(gedcomFile, errorLog);
	if (lengthList(errorLog)) {
		printf("Import aborted because of errors:\n");
		showErrorLog(errorLog);
	}
	return 0;
}

// importFromFile imports the records in a Gedcom file and returns them in a GNodeList. Gedcom
// syntax is checked; If errors occur the List is null and ErrorLog has the errors.
GNodeList* newImportFromFile(String filePath, ErrorLog* errorLog) {
	FILE* file = fopen(filePath, "r"); // Open Gedcom file.
	if (!file) {
		addErrorToLog(errorLog, createError(systemError, filePath, 0, "Could not open Gedcom file."));
		return null;
	}
	GNodeList* listOfNodes = getNodeListFromFile(file, errorLog); // Get all lines.
	if (!listOfNodes) return null;
	if (lengthList(errorLog)) {
		deleteList(listOfNodes);
		return null;
	}
	if (importDebugging) printf("listOfNodes contains %d records.\n", lengthList(listOfNodes));

	// Convert listOfNodes to a list of GNode trees.
	GNodeList* listOfTrees = getNodeTreesFromNodeList(listOfNodes, errorLog); // Get all trees.
	deleteList(listOfNodes);
	if (lengthList(errorLog)) {
		deleteList(listOfTrees);
		return null;
	}
	if (importDebugging) printf("listOfTrees contains %d records.\n", lengthList(listOfTrees));

//	Create person and family indexes and populate them.

	RecordIndex* personIndex = createRecordIndex();
	RecordIndex* familyIndex = createRecordIndex();
	FORLIST(listOfTrees, element)
	GNodeListElement* e = (GNodeListElement*) element;
	GNode* root = e->node;
	switch (recordType(root)) {
	case GRPerson:
		addToRecordIndex(personIndex, root->key, root, e->lineNo);
	case GRFamily:
	default:
	}
	ENDLIST






	Database* database = createDatabase(filePath); // Create database and add records to it.
	FORLIST(listOfTrees, element)
		GNodeListElement* e = (GNodeListElement*) element;
		storeRecord(database, normalizeRecord(e->node), e->lineNo, errorLog);
	ENDLIST
	printf("And now it is time to sort those RootList\n");
	sortList(database->personRoots);
	printf("Persons have been sorted\n");
	sortList(database->familyRoots);
	printf("Families have been sorted\n");
	return listOfTrees;
}

typedef HashTable KeyCheckTable;

typedef struct KeyCheckElement {
	String key;
	bool defined;
	bool used;
} KeyCheckElement;

static String getKey(void* element) {
	return ((KeyCheckElement*) element)->key;
}

KeyCheckElement* createKeyCheckElement(String key) {
	KeyCheckElement* element = (KeyCheckElement*) stdalloc(sizeof(KeyCheckElement));
	element->key = key;
	element->defined = false;
	element->used = false;
	return element;
}
