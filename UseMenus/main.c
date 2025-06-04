//
//  DeadEnds UseMenus
//
//  main.c is the main program of the UseMenus test program.
//
//  Created by Thomas Wetmore on 31 July 2024.
//  Last changed on 4 June 2025.
//

#include "ask.h"
#include <stdio.h>
#include "errors.h"
#include "gedcom.h"
#include "gnode.h"
#include "menu.h"
#include "list.h"
#include "database.h"
#include "utils.h"
#include "import.h"
#include "name.h"
#include "sequence.h"
#include "validate.h"

#define gms getMsecondsStr()

static void getEnvironment(String*);
static void getArguments(int, char**, String*);
static Menu* createLoadDatabaseMenu(void);
static void usage(void);

Database* database = null;
static bool debugging = true;
static bool timing = true;

// main is the main program for the DeadEnds command line program.
int main(int argc, char** argv) {
	// Get the files.
	if (timing) fprintf(stderr, "%s: UseMenus started.\n", gms);
	String gedcomFile = null;
	String gedcomPath = null;
	// Check for environment variables.
	getArguments(argc, argv, &gedcomFile);
	getEnvironment(&gedcomPath);
	if (debugging) fprintf(stderr, "gedcomFile, gedcomPath = %s, %s\n", gedcomFile, gedcomPath);

	AskReturn rcode = 0;
	if (!gedcomFile) askForString("Enter name of Gedcom file with database", 0, &gedcomFile);
	if (debugging) fprintf(stderr, "gedcomFile = %s\n", gedcomFile);
	// Build the Database from the Gedcom file.
	gedcomFile = resolveFile(gedcomFile, gedcomPath);
	if (debugging) fprintf(stderr, "resolved gedcomFile = %s\n", gedcomFile);
	ErrorLog* errorLog = createErrorLog();
	int vcodes = VCclosedKeys | VClineageLinking | VCnamesAndSex;
	Database* database = getDatabaseFromFile(gedcomFile, vcodes, errorLog);
	if (lengthList(errorLog)) {
		showErrorLog(errorLog);
		exit(1);
	}
	if (debugging) summarizeDatabase(database);
	if (timing) fprintf(stderr, "%s: Database created.\n", gms);
	//Menu* loadMenu = createLoadDatabaseMenu();
	//menuMachine(loadMenu);
	// FOLLOWING EXPERIMENTS TO FIND THE CODE TO PICK A PERSON USING NAMES
	RecordIndex* index = database->recordIndex;
	NameIndex* nindex = database->nameIndex;
	int count = 0;
	String* keys = personKeysFromName("Thomas Trask /Wetmore/", index, nindex, &count);
	printf("personKeysFromName returned %d keys. They are:\n", count);
	for (int i = 0; i < count; i++) {
		printf("%s ", keys[i]);
	}
	printf("\n");
	// Get the names of the persons.
	for (int i = 0; i < count; i++) {
		GNode* name = (NAME(keyToPerson(keys[i], index)));
		String namstr = name->value ? nameString(name->value) : "no name";
		printf("%s\n", namstr);
	}

	// TRY IT ANOTHER WAY USING SEQUENCES
	Sequence* seq = nameToSequence("Thomas Trask /Wetmore/", index, nindex);
	//Sequence* seq = nameToSequence("*/wetmore/", database);
	showSequence(seq, "A TITLE FOR A SEQUENCE");
	FORSEQUENCE(seq, element, n)
	GNode* root = element->root;
	printf("%d %s\n", n, element->name);
	ENDSEQUENCE

	int answer;
	//AskReturn rcode = 0;

//	rcode = askForInteger("Enter an integer", askQuit, &answer);
//	if (rcode == askOkay) printf("The answer is %d\n", answer);
//	else printf("The user did not answer\n");
//	String stringAnswer;
//
//	List* list = createStringList("alpha", "beta", "delta", "gamma", null);
//	rcode = askForStringInSet("Choose from alpha, beta, delta or gamma", list, 0, &stringAnswer);
//	if (rcode == askOkay) printf("The answer is %s\n", stringAnswer);
//	else printf("Not an okay answer\n");
//
//	String emailPattern = "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$";
//	rcode = askForPattern("Enter an email address", emailPattern, askQuit, &stringAnswer);
//	if (rcode == askOkay) printf("email address okay: %s\n", stringAnswer);
//	else printf("email address not okay\n");

	

	return 0;
}

// getArguments handles the command line arguments.
static void getArguments(int argc, char* argv[], String* gedcom) {
	int ch;
	*gedcom = null;
	while ((ch = getopt(argc, argv, "g:")) != -1) {
		switch(ch) {
		case 'g':
			*gedcom = strsave(optarg);
			break;
		case '?':
		default:
			usage();
			exit(1);
		}
	}
}

// getEnvironment checks for DE_GEDCOM_PATH in the environment.
static void getEnvironment(String* gedcom) {
	*gedcom = getenv("DE_GEDCOM_PATH");
	if (!*gedcom) *gedcom = ".";
}

// loadDatabaseAction is the action that happens when a user choose to load a Database from
// a Gedcom file.
void* loadDatabaseAction(Menu* menu, String selection) {
	printf("What is the name of the database? ");
	while (true) {
		// get response from user.
		// see if there is a Gedcom file there.
		// if there read the database
		//    If reading database was successful return in a way to goes to the next menu.
		// if user responded with nothing return to the originating menu
	}
	return null;
	// Request user to supply a database name.
}

void* quitProgramAction(Menu* menu, String selection) {
	printf("Quitting DeadEnds.\n");
	exit(0);
}

// createLoadDatabaseMenu creates the load database menu. This might be the first menu the user
// sees after starting up the command line version of DeadEnds.
static Menu* createLoadDatabaseMenu(void) {
	MenuItem* one = createMenuItem("d", "Load database", loadDatabaseAction);
	MenuItem* two = createMenuItem("q", "Quit program", quitProgramAction);
	List* items = createList(null, null, null, null);
	appendToList(items, one);
	appendToList(items, two);
	Menu *menu = createMenu("Load Database", items);
	return menu;
}

// creates a Menu of persons who match a given name.
static Menu* createNameMenu(Database* database, String name) {
	return null;
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: usemenus [-g gedcomfile]\n");
}
