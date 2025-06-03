// DeadEnds
//
// importone.c has the test function that tries to create a Database for the rest of the tests.
//
// Created by Thomas Wetmore on 21 June 2024.
// Last changed on 15 October 2024.

#include "database.h"
#include "errors.h"
#include "list.h"
#include "nameindex.h"
#include "standard.h"
#include "import.h"
#include "utils.h"

// importDatabaseTest tests the new import organization.
 Database* importDatabaseTest(ErrorLog* log, int testNumber) {
	printf("%d: START OF IMPORT DATABASE TEST: %s %s\n", testNumber, "modified.ged", getMsecondsStr());
	String gedcomFile = "/Users/ttw4/Desktop/DeadEndsVSCode/Gedfiles/unknownsex.ged";
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	int vcodes = 31; // First five bits sest.
	Database* database = getDatabaseFromFile(gedcomFile, vcodes, log);
	if (lengthList(log)) {
		printf("Import cancelled because of errors:\n");
		showErrorLog(log);
	}
	summarizeDatabase(database);
	printf("%d: END OF CREATE DATABASE TEST: %s\n", testNumber, getMsecondsStr());
	return database;
}
