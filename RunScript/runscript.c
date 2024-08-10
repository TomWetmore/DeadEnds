// DeadEnds
//
// main.c is the DeadEnds RunScript program.
// 1. RunScript creates a Database from a Gedcom file.
// 2. It parses a script file and builds the internal form of the program.
// 3. It runs the script on the database and writes ouput to stdout.
//
// usage: RunScript -g gedcomfile -s scriptfile
//
// If DE_GEDCOM_PATH or DE_SCRIPTS_PATH are in the environment, they may be used to find the files.
//
// Created by Thomas Wetmore on 21 July 2024
// Last changed on 10 August 2024.

#include "runscript.h"

static void usage(void);
static void getArguments(int, char**, String*, String*);
static void getEnvironment(String*, String*);
static void getDatabase(void);
static void runScript(Database*, String);

// main is the main program for the RunScript command line program.
int main(int argc, char* argv[]) {
	// Get the files.
	fprintf(stderr, "%s: RunScript started.\n", getMillisecondsString());
	String gedcomFile = null;
	String scriptFile = null;
	String gedcomPath = null;
	String scriptPath = null;
	getArguments(argc, argv, &gedcomFile, &scriptFile);
	getEnvironment(&gedcomPath, &scriptPath);
	// Build the Database from the Gedcom file.
	gedcomFile = resolveFile(gedcomFile, gedcomPath);
	ErrorLog* errorLog = createErrorLog();
	Database* database = gedcomFileToDatabase(gedcomFile, errorLog);
	fprintf(stderr, "%s: Database created.\n", getMillisecondsString());
	if (lengthList(errorLog)) {
		showErrorLog(errorLog);
		exit(1);
	}
	// Parse and run the script.
	parseProgram(scriptFile, scriptPath);
	fprintf(stderr, "%s: Script parsed.\n", getMillisecondsString());
	runScript(database, scriptFile);
	fprintf(stderr, "%s: RunScript done.\n", getMillisecondsString());
}

// getFileArguments gets the file names from the command line.
void getArguments(int argc, char* argv[], String* gedcom, String* script) {
	int ch;
	while ((ch = getopt(argc, argv, "g:s:")) != -1) {
		switch(ch) {
		case 'g':
			*gedcom = strsave(optarg);
			break;
		case 's':
			*script = strsave(optarg);
			break;
		case '?':
		default:
			usage();
			exit(1);
		}
	}
	if (!*gedcom || !*script) {
		usage();
		exit(1);
	}
}

// getEnvironment checks for DE_GEDCOM_PATH or DE_SCRIPTS_PATH in the environment.
static void getEnvironment(String* gedcom, String* script) {
	*gedcom = getenv("DE_GEDCOM_PATH");
	*script = getenv("DE_SCRIPTS_PATH");
	if (!*gedcom) *gedcom = ".";
	if (!*script) *script = ".";
}

// runScript runs a script by interpreting the main proc.
extern String currentFileName;
extern int currentLine;
void runScript(Database* database, String fileName) {
	// Create a PNode to call the main proc.
	currentFileName = "internal";
	currentLine = 1;
	PNode* pnode = procCallPNode("main", null);
	// Call the main proc.
	SymbolTable* symbols = createSymbolTable();
	Context* context = createContext(symbols, database);
	interpret(pnode, context, null); // Call main proc.
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: RunScript -g gedcomfile -s scriptfile\n");
}
