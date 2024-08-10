//
// main.c
// UseMenus
//
// Created by Thomas Wetmore on 31 July 2024.
// Last changed on 8 August 2024.

#include <stdio.h>
#include "menu.h"
#include "list.h"
#include "database.h"
#include "ask.h"

static void getEnvironment(String*, String*);
static Menu* createLoadDatabaseMenu(void);
Database* database = null;
static bool debugging = true;

// main is the main program for the DeadEnds command line program.
int main(int argc, const char * argv[]) {
	String gedcomPath, scriptsPath;
	// Check for environment variables.
	getEnvironment(&gedcomPath, &scriptsPath);
	if (debugging) printf("gedcomPath is %s\n", gedcomPath);
	Menu* loadMenu = createLoadDatabaseMenu();
	//menuMachine(loadMenu);

	int answer;
	AskReturn rcode = 0;
	//AskReturn rcode = askForInteger("Enter an integer", askQuit, &answer);
	//if (rcode == askOkay)
		//printf("The answer is %d\n", answer);
	//else
		//printf("The user did not answer\n");
	String stringAnswer;
	//List* list = createStringList("alpha", "beta", "delta", "gamma", null);
	//rcode = askForStringInSet("Choose from alpha, beta, delta or gamma", list, 0, &stringAnswer);

	String emailPattern = "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$";
	rcode = askForPattern("Enter an email address", emailPattern, askQuit, &stringAnswer);
	printf("email address oka: %s\n", stringAnswer);
	return 0;
}

// getEnvironment checks for DE_GEDCOM_PATH or DE_SCRIPTS_PATH in the environment.
static void getEnvironment(String* gedcom, String* script) {
	*gedcom = getenv("DE_GEDCOM_PATH");
	*script = getenv("DE_SCRIPTS_PATH");
	if (!*gedcom) *gedcom = ".";
	if (!*script) *script = ".";
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
