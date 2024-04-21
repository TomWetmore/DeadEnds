//  test.c -- Test program.
//
//  Created by Thomas Wetmore on 5 October 2023.
//  Last changed on 19 April 2024.

#include <stdio.h>
#include "standard.h"
#include "parse.h"
#include "interp.h"
#include "functiontable.h"
#include "recordindex.h"
#include "pnode.h"
#include "errors.h"
#include "sequence.h"
#include "list.h"
#include "path.h"
#include "readnode.h"
#include "validate.h"
#include "utils.h"

FILE* debugFile = null;
bool useDebugFile = true;

extern String currentProgramFileName;
extern int currentProgramLineNumber;
extern FunctionTable *procedureTable;

extern Database *importFromFile(String, ErrorLog*);
static Database *createDatabaseTest(String, int, ErrorLog*);
static void listTest(Database*, int);
static void forHashTableTest(Database*, int);
static void parseAndRunProgramTest(Database*, int);
static bool validateDatabaseTest(Database*, int);
static void forTraverseTest(Database*, int);
static void showHashTableTest(HashTable*, int);
static void indexNamesTest(Database*, int);
static void testNewReadLayer(String);
extern bool validateDatabase(Database*, ErrorLog*);
static void countNodesBeforeTest(Database*, int);

// main is the main function of a batch program that tests the DeadEnds infrastructure.
int main(void) {
	// Open the debugging file.
	if (useDebugFile) {
		debugFile = fopen("/Users/ttw4/debug.txt", "w");
		if (debugFile == null) {
			printf("Could not open the debug file: quitting\n");
			exit(2);
		}
	}
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/bad.ged";
	String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/main.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/test.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/small.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/notthere.ged";
	//String gedcomFile = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/threezeros.ged";
	int testNumber = 0;
	ErrorLog *errorLog = createErrorLog();

	Database *database = createDatabaseTest(gedcomFile, ++testNumber, errorLog);
	bool validated = false;
	showErrorLog(errorLog);

	//if (database) listTest(database, ++testNumber);
	//if (database) forHashTableTest(database, ++testNumber);
	//if (database) showHashTableTest(database->personIndex, ++testNumber);
	//if (database) indexNamesTest(database, ++testNumber);
	if (database) validated = validateDatabaseTest(database, ++testNumber);
	//if (database) forTraverseTest(database, ++testNumber);
	if (database && validated) parseAndRunProgramTest(database, ++testNumber);
	//if (database && validated) countNodesBeforeTest(database, ++testNumber);
	fclose(debugFile);
	return 0;
}

// createDatabaseTest creates a test database from a Gedcom file.
Database *createDatabaseTest(String gedcomFile, int testNumber, ErrorLog *errorLog) {
	printf("%d: START OF CREATE DATABASE TEST -- Create database from %s\n", testNumber, gedcomFile);
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	Database *database = importFromFile(gedcomFile, errorLog);
	printf("The number of persons in the database is %d.\n", numberPersons(database));
	printf("The number of families in the database is %d.\n", numberFamilies(database));
	printf("END OF CREATE DATABASE TEST\n");
	return database;
}

static String getKey(void* element) {
	return ((GNode*) element)->key;
}

// compare compare two record keys.
static int compare(String a, String b) {
	return compareRecordKeys(a, b);
}

// listTest creates a list of all the persons in the database; sort the list by tags, and then
// prints the record tags in sorted order.
void listTest(Database *database, int testNumber) {
	printf("\n%d: START OF LIST TEST\n", testNumber);
	int i, j;  //  State variables used to iterate the person index hash table.
	int count = 0;
	GNode *person;
	//  Create a List of all the persons in the database.
	List *personList = createList(getKey, compare, null, false);
	void* element = firstInHashTable(database->personIndex, &i, &j);
	while (element) {
		person = ((RecordIndexEl*) element)->root;
		// Alternate between prepend and append to be sure key sorting works.
		if (count++ % 2)
			appendToList(personList, person);
		else
			prependToList(personList, person);
		element = nextInHashTable(database->personIndex, &i, &j);
	}
	printf("The list has %d elements in it.\n", lengthList(personList));
	sortList(personList);
	count = 0;
	FORLIST(personList, person)
		printf("%s\n", ((GNode*) person)->key);
		count++;
	ENDLIST
	printf("%d persons are in the list\n", count);
	printf("END OF LIST TEST\n");
}

// forHashTableTest tests FORHASHTABLE macro by showing all persons in the database's person index.
void forHashTableTest(Database* database, int testNumber) {
	printf("\n%d: START OF FORHASHTABLE test\n", testNumber);
	int numberPersons = 0;
	FORHASHTABLE(database->personIndex, element)
		numberPersons++;
		RecordIndexEl *rel = (RecordIndexEl*) element;
		GNode *person = rel->root;
		printf("%s: %s\n", person->key, NAME(person)->value);
	ENDHASHTABLE
	printf("%d persons were found in the index.\n", numberPersons);
	printf("END OF FORHASHTABLE TEST\n");
}

//  parseAndRunProgramTest -- Parse a DeadEndScript program and run it. In order to call the
//    main procedure of a DeadEndScript, create a PNProcCall program node, and interpret it.
//-------------------------------------------------------------------------------------------------
void parseAndRunProgramTest(Database *database, int testNumber)
//  database -- The database the script runs on.
{
	printf("\n%d: START OF PARSE AND RUN PROGRAM TEST: %2.3f\n", testNumber, getMilliseconds());
	double milli = getMilliseconds();
	parseProgram("llprogram", "/Users/ttw4/Desktop/DeadEnds/Reports/");
	printf("Finished parsing: %2.3f\n", getMilliseconds());

//  Create a PNProcCall node to call the main procedure.
	currentProgramFileName = "internal";
	currentProgramLineNumber = 1;
	PNode *pnode = procCallPNode("main", null);

	//  Call the main procedure.
	SymbolTable *symbolTable = createSymbolTable();
	Context *context = createContext(symbolTable, database);
	PValue returnPvalue;
	interpret(pnode, context, &returnPvalue);
	printf("END OF PARSE AND RUN PROGRAM TEST: %2.3f\n", getMilliseconds());
}

//  validateDatabaseTest -- Validate the a database.
//-------------------------------------------------------------------------------------------------
bool validateDatabaseTest(Database *database, int testNumber)
{
	printf("\n%d: START OF VALIDATE DATABASE TEST\n", testNumber);
	ErrorLog* errorLog = createErrorLog();
	bool validated = validateDatabase(database, errorLog);
	printf("END OF VALIDATE DATABASE TEST\n");
	return validated;
}

// forTraverseTest checks that the FORTRAVERSE macro works.
static void forTraverseTest(Database *database, int testNumber) {
	printf("\n%d: START OF FORTRAVERSE TEST\n", testNumber);
	GNode* person = keyToPerson("@I1@", database);
	FORTRAVERSE(person, node)
		printf("%s\n", node->tag);
	ENDTRAVERSE
	printf("END OF FORTRAVERSE TEST\n");
}

static void showPersonName(void* element)
{
	GNode *person = ((RecordIndexEl*) element)->root;
	ASSERT(person);
	GNode *name = NAME(person);
	ASSERT(name && person->key && name->value);
	printf("%s: %s\n", person->key, name->value);
}

// showHashTableTest tests showHashTable by showing the names of all persons in the database..
static void showHashTableTest(RecordIndex *index, int testNumber) {
	printf("\n%d: START OF SHOW HASH TABLE TEST\n", testNumber);
	showHashTable(index, showPersonName);
	printf("END OF SHOW HASH TABLE TEST\n");
}

// indexNamesTest tests the indexNames fumction.
static void indexNamesTest(Database *database, int testNumber) {
	printf("\n%d: START OF INDEX NAMES TEST\n", testNumber);
	indexNames(database);
	printf("END OF INDEX NAMES TEST\n");
}

extern int personLineNumber(GNode*, Database*);

static void countNodesBeforeTest(Database *database, int testNumber)
{
	printf("\n%d: START OF COUNT NODES BEFORE TEST\n", testNumber);
	GNode *person = keyToPerson("@I1@", database);
	int startLineNumber = personLineNumber(person, database);
	if (person) {
		FORTRAVERSE(person, node)
			printf(" %d %s %s\n", startLineNumber + countNodesBefore(node), node->tag, node->value ? node->value : "");
		ENDTRAVERSE
	}
}
