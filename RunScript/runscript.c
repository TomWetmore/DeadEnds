// DeadEnds
//
// main.c is the DeadEnds RunScript program. RunScript takes a Gedcom file and creates a Database.
// It then parses a script file and builds the internal form of the program. It then runs the
// script on the database and writes any ouput.
//
// usage: RunScript -g gedcomfile -s scriptfile
//
// If DE_GEDCOM_PATH or DE_SCRIPTS_PATH are in the environment, they can be used to find the
// Gedcom and/or script files.
//
// Created by Thomas Wetmore on 21 July 2024
// Last changed on 28 July 2021.

#include "unistd.h"
#include "runscript.h"
#include "parse.h"
#include "utils.h"
#include "pnode.h"
#include "path.h"

static bool timing = true;
static bool debugging = true;

static void usage(void);
static void getArguments(int, char**, String*, String*);
static void getEnvironment(String*, String*);
static void getDatabase(void);
static void runScript(Database*, String);

FILE* debugFile = null; // TODO: Need to get rid of this.

// main is the main program for the RunScript command.
int main(int argc, char* argv[]) {
	fprintf(stderr, "%s: RunScript started.\n", getMillisecondsString());
	String gedcomFile = null;
	String scriptFile = null;
	String gedcomPath = null;
	String scriptPath = null;
	getArguments(argc, argv, &gedcomFile, &scriptFile);
	getEnvironment(&gedcomPath, &scriptPath);
	if (debugging) {
		printf("gedcomFile: %s\n", gedcomFile);
		printf("scriptFile: %s\n", scriptFile);
		printf("gedcomPath: %s\n", gedcomPath);
		printf("scriptPath: %s\n", scriptPath);
	}

	// Build the Database from the Gedcom file.
	gedcomFile = resolveFile(gedcomFile, gedcomPath);
	if (debugging) printf("resolved gedcomFile: %s\n", gedcomFile);
	ErrorLog* errorLog = createErrorLog();
	Database* database = gedcomFileToDatabase(gedcomFile, errorLog);
	if (timing) fprintf(stderr, "%s: Database created.\n", getMillisecondsString());
	if (lengthList(errorLog)) {
		showErrorLog(errorLog);
		exit(1);
	}

	// Parse the script file.
	parseProgram(scriptFile, scriptPath);
	if (timing) fprintf(stderr, "%s: Script parsed.\n", getMillisecondsString());
	runScript(database, scriptFile);
	if (timing) fprintf(stderr, "%s: RunScript done.\n", getMillisecondsString());
}

// getFileArguments gets the file names from the command line. They are mandatory.
void getArguments(int argc, char* argv[], String* gedcomFile, String* scriptFile) {
	int ch;
	while ((ch = getopt(argc, argv, "g:s:")) != -1) {
		switch(ch) {
		case 'g':
			*gedcomFile = strsave(optarg);
			break;
		case 's':
			*scriptFile = strsave(optarg);
			break;
		case '?':
		default:
			usage();
			exit(1);
		}
	}
	if (!*gedcomFile || !*scriptFile) {
		usage();
		exit(1);
	}
}

// getEnvironment checks for the DE_GEDCOM_PATH and DE_SCRIPTS_PATH env variables.
static void getEnvironment(String* gedcomPath, String* scriptPath) {
	*gedcomPath = getenv("DE_GEDCOM_PATH");
	*scriptPath = getenv("DE_SCRIPTS_PATH");
	if (!*gedcomPath) *gedcomPath = ".";
	if (!*scriptPath) *scriptPath = ".";
}

// runScript runs a script by interpreting its main procedure.
extern String currentFileName;
extern int currentLine;
void runScript(Database* database, String fileName) {
	currentFileName = "internal";
	currentLine = 1;
	PNode *pnode = procCallPNode("main", null); // PNode to call main.
	if (!pnode) {
		fprintf(stderr, "%s: The script does not have a main procedure.\n", fileName);
		exit(1);
	}
	SymbolTable *symbolTable = createSymbolTable();
	Context *context = createContext(symbolTable, database);
	interpret(pnode, context, null); // Call main proc.
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: RunScript -g gedcomfile -s scriptfile\n");
}
