//
//  DeadEnds GenSExprsions
//
//  main.c is the main program of the DeadEnds gensexprs program. It parses a DeadEnds script file (and those it
//  includes), which loads the procedures, functions, and globals tables. The function and procedure tables hold
//  PNode trees the make up the absract syntax form of the procedure and function bodies. The program then writes
//  those PNode trees to stdout as S-Expressions. The program uses DeadEndsLib to access to the script parser and
//  the PNode class.
//
//  usage: gensexpr -s scriptfile
//
//  Created by Thomas Wetmore on 4 March 2025.
//  Last changed on 18 August 2025.
//

#include <stdio.h>
#include "context.h"
#include "hashtable.h"
#include "list.h"
#include "parse.h"
#include "pnode.h"
#include "functiontable.h"
#include "symboltable.h"

extern void printPNodeTreeAsSExpr(FILE *out, PNode *root);

// Local functions.
static void usage(void);
static void getArguments(int, char**, String*);
static void getEnvironment(String*);
static void genSExpressions(Program*);

// External functions (avoid header file).
String getMsecondsStr(void);

// main.c is the main program of the DeadEnds gensexprs program.
int main(int argc, char* argv[]) {
	// Get the script file.
	fprintf(stderr, "%s: crtsexpr started.\n", getMsecondsStr());
	String scriptFile = null;
	String scriptPath = null;
	getArguments(argc, argv, &scriptFile);
	getEnvironment(&scriptPath);
    fprintf(stderr, "scriptFile is %s\n", scriptFile);
    fprintf(stderr, "scriptPath is %s\n", scriptPath);
	// Parse the script into PNode trees.
	Program* program = parseProgram(scriptFile, scriptPath);
	fprintf(stderr, "%s: Script parsed.\n", getMsecondsStr());
	// Generate S-Expressions from the PNode trees.
	genSExpressions(program);
    deleteProgram(program);
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
static void genSExpressions(Program* program) {
	fprintf(stderr, "procedures holds %d procedures\n", sizeHashTable(program->procedures));
	fprintf(stderr, "functions holds %d functions\n", sizeHashTable(program->functions));
    fprintf(stderr, "globals holds %d variables\n", lengthList(program->globalIdents));

    printf("{\n");
	FORHASHTABLE(program->procedures, element)
		FunctionElement* functionElement = (FunctionElement*) element;
		PNode* root = functionElement->function;
		printPNodeTreeAsSExpr(stdout, root);
	ENDHASHTABLE

	FORHASHTABLE(program->functions, element)
		FunctionElement* functionElement = (FunctionElement*) element;
		PNode* root = functionElement->function;
		printPNodeTreeAsSExpr(stdout, root);
	ENDHASHTABLE

    FORLIST(program->globalIdents, ident)
        printf("(global %s\n)\n", (String) ident);
    ENDLIST
    printf("\n}\n");
}

// usage prints the usage message.
static void usage(void) {
	fprintf(stderr, "usage: gensexpr -s scriptfile\n");
}


