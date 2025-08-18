//  DeadEnds (Test Program)
//
//  importone.c has the test function that tries to create a Database for the rest of the tests.
//
//  Created by Thomas Wetmore on 21 June 2024.
//  Last changed on 5 June 2025.
//

#include "deadends.h"

// importDatabaseTest tests the new import organization.
 Database* importDatabaseTest(ErrorLog* log, int testNumber) {
	printf("%d: START OF IMPORT DATABASE TEST: %s %s\n", testNumber, "modified.ged", getMsecondsStr());
	String gedcomFile = "/Users/ttw4/Desktop/DeadEndsVSCode/Gedfiles/unknownsex.ged";
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	Database* database = getDatabaseFromFile(gedcomFile, log);
	if (lengthList(log)) {
		printf("Import cancelled because of errors:\n");
		showErrorLog(log);
	}
	summarizeDatabase(database);
	printf("%d: END OF CREATE DATABASE TEST: %s\n", testNumber, getMsecondsStr());
	return database;
}
