// DeadEnds
//
// main.c is the DeadEnds RunScript program. It has three steps.
//  1. Create a Database from a Gedcom file.
//  2. Parse a DeadEnds script file into its internal form.
//  3. Runs the script on the Ddatabase and writes any output to stdout.
//
// usage: runscript -g gedcomfile -s scriptfile
//
// If DE_GEDCOM_PATH and/or DE_SCRIPTS_PATH are defined, they may be used to find the files.
//
// Created by Thomas Wetmore on 21 July 2024
// Last changed on 16 August 2024.

#include "runscript.h"

// Local functions.
static void usage(void);
static void getArguments(int, char**, String*, String*);
static void getEnvironment(String*, String*);
static void getDatabase(void);
static void runScript(Database*, String);

// main is the main program of the RunScript program.
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
		if (lengthList(errorLog)) {
			showErrorLog(errorLog);
			exit(1);
		}
		fprintf(stderr, "%s: Database created.\n", getMillisecondsString());
	// Parse and run the script.
	parseProgram(scriptFile, scriptPath);
	fprintf(stderr, "%s: Script parsed.\n", getMillisecondsString());
	runScript(database, scriptFile);
	fprintf(stderr, "%s: RunScript done.\n", getMillisecondsString());
}

// getArguments gets the file names from the command line.
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

// getEnvironment looks for DE_GEDCOM_PATH or DE_SCRIPTS_PATH in the environment.
static void getEnvironment(String* gedcom, String* script) {
	*gedcom = getenv("DE_GEDCOM_PATH");
	*script = getenv("DE_SCRIPTS_PATH");
	if (!*gedcom) *gedcom = ".";
	if (!*script) *script = ".";
}

// runScript runs a script by interpreting its main proc. After a script is parsed there must
// be a proc named "main" in the global procedureTable. runScript creates a PNode to call that
// proc, then creates the Context to call it in, and then calls it. The Context consists of the
// Database and the main proc's SymbolTable.
extern String curFileName;
extern int curLine;
void runScript(Database* database, String fileName) {
	// Create a PNode to call the main proc.
	curFileName = "internal";
	curLine = 1;
	PNode* pnode = procCallPNode("main", null);
	// Call the main proc.
	SymbolTable* symbols = createSymbolTable();
	Context* context = createContext(symbols, database);
	interpret(pnode, context, null); // Call main proc.
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: runscript -g gedcomfile -s scriptfile\n");
}
