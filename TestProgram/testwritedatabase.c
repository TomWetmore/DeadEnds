// DeadEnds
//
// testwritedatabase.c has the functions that test writing Databases to files.
//
// Created by Thomas Wetmore on 30 May 2024.
// Last changed on 7 June 2024.

#include "database.h"
#include "utils.h"

void testWriteDatabase(String fileName, Database* database) {
	printf("START WRITE DATABASE TEST: %2.3f\n", getMseconds());
	writeDatabase(fileName, database);
	printf("END WRITE DATABASE TESTS: %2.3f\n", getMseconds());
}
