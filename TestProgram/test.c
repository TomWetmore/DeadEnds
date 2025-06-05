//
//  DeadEnds (Test Program)
//
//  test.c holds test functions used during development.
//
//  Created by Thomas Wetmore on 5 October 2023.
//  Last changed on 5 June 2025.
//

#include "deadends.h"

#define gms getMsecondsStr()

//static bool debugging = false;
extern String curFileName;
extern int curLine;
extern FunctionTable *procedures;

extern Database *importFromFile(String, ErrorLog*);
extern void testSequence(Database*, int);
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
extern void testGedcomStrings(int);
extern void testWriteDatabase(String file, Database*);
extern void testGedPaths(Database*, int);

extern Database* importDatabaseTest(ErrorLog*, int);

// main is the main function of the DeadEnds testing program.
int main(void) {
	ErrorLog* errorLog = createErrorLog();
	int testNumber = 0;

	String file = "/Users/ttw4/Desktop/DeadEndsVSCode/Gedfiles/modified.ged";
	//RecordIndex* index = getRecordIndexFromFile(file, null, null, null, errorLog);
	Database* database = importDatabaseTest(errorLog, ++testNumber);
	//testGedcomStrings(++testNumber);
	bool validated = database ? true : false;
	showErrorLog(errorLog);

	//if (database) listTest(database, ++testNumber);
	//if (database) forHashTableTest(database, ++testNumber);
	//if (database) showHashTableTest(database->recordIndex, ++testNumber);
	//if (database) indexNamesTest(database, ++testNumber);
	if (database) testSequence(database, ++testNumber);
	//if (validated) testGedPaths(database, ++testNumber);
	//if (validated) forTraverseTest(database, ++testNumber);
	//if (validated) parseAndRunProgramTest(database, ++testNumber);
	//if (validated) testWriteDatabase("/Users/ttw4/output.ged", database);
	return 0;
}

// createDatabaseTest creates a test database from a Gedcom file.
Database *createDatabaseTest(String gedcomFile, int testNumber, ErrorLog *errorLog) {
	printf("%d: START OF CREATE DATABASE TEST: %s %s\n", testNumber, gedcomFile, gms);
	String lastSegment = lastPathSegment(gedcomFile);
	printf("lastPathSegment: %s\n", lastSegment);
	Database *database = importFromFile(gedcomFile, errorLog);
	printf("The number of persons in the database is %d.\n", numberPersons(database));
	printf("The number of families in the database is %d.\n", numberFamilies(database));
	printf("END OF CREATE DATABASE TEST: %2.3f\n", getMseconds());
	return database;
}

// getKey gets the key of a GNode element.
static String getKey(void* element) { return ((GNode*) element)->key; }

// compare compares two record keys.
static int compare(String a, String b) { return compareRecordKeys(a, b); }

// listTest creates a list of all persons in a Database, sorts the list by key, and then
// prints the records in key order.
void listTest(Database* database, int testNumber) {
	printf("\n%d: START OF LIST TEST: %s\n", testNumber, gms);
	int count = 0;
	GNode *person;
	List *personList = createList(getKey, compareRecordKeys, null, false); // List for the persons.
	int i, j;
	void* element = firstInHashTable(database->recordIndex, &i, &j);
	while (element) {
		person = (GNode*) element;
		if (count++ % 2) // Alternate to improve sorting.
			appendToList(personList, person);
		else
			prependToList(personList, person);
		element = nextInHashTable(database->recordIndex, &i, &j);
	}
	printf("The list has %d elements in it.\n", lengthList(personList));
	sortList(personList);
	count = 0;
	FORLIST(personList, person)
		printf("%s\n", ((GNode*) person)->key);
		count++;
	ENDLIST
	printf("%d persons are in the list\n", count);
	printf("END OF LIST TEST: %s\n", gms);
}

// forHashTableTest tests FORHASHTABLE by showing the keys and names of the persons in an index.
void forHashTableTest(Database* database, int testNumber) {
	printf("\n%d: START OF FORHASHTABLE test: %s\n", testNumber, gms);
	int numberPersons = 0;
	FORHASHTABLE(database->recordIndex, element)
		GNode *person = (GNode*) element;
		if (recordType(person) == GRPerson) {
			numberPersons++;
			printf("%s: %s\n", person->key, NAME(person) ? NAME(person)->value : "no name");
		}
	ENDHASHTABLE
	printf("%d persons were found in the index.\n", numberPersons);
	printf("%d: END OF FORHASHTABLE TEST: %s\n", testNumber, gms);
}

// parseAndRunProgramTest parses a DeadEndScript program and runs it. To call the script's
// main proc create a PNProcCall PNode and interpret it.
void parseAndRunProgramTest(Database *database, int testNumber) {
	printf("\n%d: START OF PARSE AND RUN PROGRAM TEST: %s\n", testNumber, gms);
	//parseProgram("llprogram", "/Users/ttw4/Desktop/DeadEnds/Reports/");
	//parseProgram("scriptindiseq", "/Users/ttw4/Desktop/DeadEnds/Reports/");
//	parseProgram("llprogram", "/Users/ttw4/Desktop/DeadEnds/Scripts");
//	printf("Finished parsing: %2.3f\n", getMseconds());
//
//	curFileName = "internal";
//	curLine = 1;
//	PNode *pnode = procCallPNode("main", null); // PNode to call main proc.
//	SymbolTable *symbolTable = createSymbolTable();
//	Context *context = createContext(symbolTable, database);
//	PValue returnPvalue;
//	interpret(pnode, context, &returnPvalue); // Call main proc.
	printf("END OF PARSE AND RUN PROGRAM TEST: %s\n", gms);
}

// validateDatabaseTest is a test function that validates a Database.
bool validateDatabaseTest(Database* database, int testNumber) {
	printf("\n%d: START OF VALIDATE DATABASE TEST: %s\n", testNumber, gms);
	ErrorLog* errorLog = createErrorLog();
	bool validated = validateDatabase(database, errorLog);
	printf("END OF VALIDATE DATABASE TEST: %s\n", gms);
	return validated;
}

// forTraverseTest checks the FORTRAVERSE macro.
static void forTraverseTest(Database* database, int testNumber) {
	printf("\n%d: START OF FORTRAVERSE TEST: %s\n", testNumber, gms);
	GNode* person = keyToPerson("@I1@", database->recordIndex);
	FORTRAVERSE(person, node)
		printf("%s %s\n", node->tag, node->value ? node->value : "");
	ENDTRAVERSE
	printf("END OF FORTRAVERSE TEST: %s\n", gms);
}

// showPersonName shows a person's name; used by the showHashTable test.
static void showPersonName(void* element) {
	GNode *person = (GNode*) element;
	ASSERT(person);
	GNode *name = NAME(person);
	//ASSERT(name && person->key && name->value);
	if (name && person->key && name->value)
		printf("%s: %s\n", person->key, name->value);
	else
		printf("%s: must not be a person\n", person->key);
}

// showHashTableTest tests showHashTable by showing the names of the persons in a Database.
static void showHashTableTest(RecordIndex* index, int testNumber) {
	printf("\n%d: START OF SHOW HASH TABLE TEST: %s\n", testNumber, gms);
	showHashTable(index, showPersonName);
	printf("%d: END OF SHOW HASH TABLE TEST: %s\n", testNumber, gms);
}

// indexNamesTest tests the indexNames function.
static void indexNamesTest(Database* database, int testNumber) {
	printf("\n%d: START OF INDEX NAMES TEST\n", testNumber);
	showNameIndexStats(getNameIndex(database->personRoots));
	printf("END OF INDEX NAMES TEST\n");
}

