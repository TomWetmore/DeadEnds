//
//  main.c
//  GenerateSExpressions
//  This is the DeadEnds GenerateSExpressions program. The program parses a DeadEnds script file (and those it
//  includes), which loads a globalTable, functionTable, and procedureTable. The function and procedure tables
//  hold abstract syntax trees in the form of PNode trees. The program then write those PNode trees to standard
//  output as S-Expressions. The program uses the DeadEndsLib to access to the script parser and PNode class.
//
//  usage: gensexpr -s scriptfile
//
//  Created by Thomas Wetmore on 4 March 2025.
//  Last changed on 21 March 2025.
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

// External functions (avoids a header file).
String getMsecondsStr(void);

// main.c is the main program of  DeadEnds GenerateSExpressions program.
int main(int argc, char* argv[]) {
	// Get the script file.
	fprintf(stderr, "%s: CreateSExpressions started.\n", getMsecondsStr());
	String scriptFile = null;
	String scriptPath = null;
	getArguments(argc, argv, &scriptFile);
	getEnvironment(&scriptPath);
	// Parse the script into PNode trees.
	parseProgram(scriptFile, scriptPath);
	fprintf(stderr, "%s: Script parsed.\n", getMsecondsStr());
	// Generate S-Expressions from the PNode trees.
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

// genSExpressions generates the S-Expressions from the PNode trees in the procedure and function tables.
static void genSExpressions(void) {
	printf("procedureTable holds %d procedures\n", sizeHashTable(procedureTable));
	printf("functionTable holds %d functions\n", sizeHashTable(functionTable));
    printf("globalTable holds %d variables\n", sizeHashTable(globalTable));

    printf("{\n");
	FORHASHTABLE(procedureTable, element)
		FunctionElement* functionElement = (FunctionElement*) element;
		PNode* root = functionElement->function;
		printPNodeTreeAsSExpr(stdout, root);
	ENDHASHTABLE

	FORHASHTABLE(functionTable, element)
		FunctionElement* functionElement = (FunctionElement*) element;
		PNode* root = functionElement->function;
		printPNodeTreeAsSExpr(stdout, root);
	ENDHASHTABLE

    FORHASHTABLE(globalTable, element)
        Symbol* symbol = (Symbol*) element;
        printf("(global %s\n)\n", symbol->ident);
    ENDHASHTABLE
    printf("\n}");
}

// usage prints the RunScript usage message.
static void usage(void) {
	fprintf(stderr, "usage: gensexpr -s scriptfile\n");
}


