//
//  main.c
//  GenerateSExpressions
//
//  Created by Thomas Wetmore on 4 March 2025.
//  Last changed on 4 March 2025.
//

#include <stdio.h>
#include "pnode.h"
#include "functiontable.h"

// Global tables filled by the parser.
extern SymbolTable* globalTable; // Global variables.
extern FunctionTable* functionTable; // User functions.
extern FunctionTable* procedureTable; // User procedures.
extern void printPNodeTreeAsSExpr(FILE *out, PNode *root);

// Local functions.
static void usage(void);
static void getArguments(int, char**, String*);
static void getEnvironment(String*);
static void genSExpressions(void);

// External functions (or how to avoid a header file).
String getMsecondsStr(void);

// main.c is the DeadEnds GenerateSExpressions program. The program first reads a DeadEnds script file (and those it
// includs), loading the globalTable, functionTable, and procedureTable. The function and procedure tables hold
// PNode trees. Second the program writes the PNode trees from the two tables as S-Expressions to standard output.
// TODO: Also handle the globalTable.
// usage: gensexpr -s scriptfile

int main(int argc, char* argv[]) {
	// Find the script file.
	fprintf(stderr, "%s: CreateSExpressions started.\n", getMsecondsStr());
	String scriptFile = null;
	String scriptPath = null;
	getArguments(argc, argv, &scriptFile);
	getEnvironment(&scriptPath);
	// Parse and run the script.
	parseProgram(scriptFile, scriptPath);
	fprintf(stderr, "%s: Script parsed.\n", getMsecondsStr());
	genSExpressions();
}

// getArguments gets the arguments from the command line.
void getArguments(int argc, char* argv[], String* script) {
	int ch;
	while ((ch = getopt(argc, argv, "s:")) != -1) {
		switch(ch) {
		case 's':
			*script = strsave(optarg);
			break;
		case '?':
		default:
			usage();
			exit(1);
		}
	}
	if (!*script) {
		usage();
		exit(1);
	}
}

// getEnvironment looks for DE_SCRIPTS_PATH in the environment.
static void getEnvironment(String* script) {
	*script = getenv("DE_SCRIPTS_PATH");
	if (!*script) *script = ".";
}

// genSExpressions generates the S-Expressions from the last parsing phase.
// TODO: The global variable table is not printed.
static void genSExpressions(void) {
	printf("procedureTable holds %d procedures\n", sizeHashTable(procedureTable));
	printf("functionTable holds %d functions\n", sizeHashTable(functionTable));

	FORHASHTABLE(procedureTable, element)
	FunctionElement* functionElement = (FunctionElement*) element;
		PNode* root = functionElement->function;
		printPNodeTreeAsSExpr(stdout, root);
	ENDHASHTABLE
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: gensexpr -s scriptfile\n");
}


