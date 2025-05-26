// DeadEnds
//
// parse.c contains two functions, parseProgram and parseFile, which parse DeadEnds scripts.
//
// Created by Thomas Wetmore on 4 January 2023.
// Last changed on 27 July 2024.

#include "parse.h"
#include <stdarg.h>
#include "functiontable.h"
#include "integertable.h"
#include "gedcom.h"
#include "interp.h"
#include "list.h"
#include "stringset.h"
#include "path.h"
#include "pnode.h"
#include <unistd.h>  // sleep.

static bool debugging = false;
extern bool programParsing;

// Shared global variables.
SymbolTable* globalTable; // Global variables.
FunctionTable* functionTable; // User functions.
FunctionTable* procedureTable; // User procedures.
List* pendingFiles; // Files to be parsed.
String curFileName = null; // File being parsed
FILE* currentFile = null; // FILE being parsed.
int curLine = 1; // Line number in current file.

static void parseFile(String file, String path);

// parseProgram parses a DeadEnds script and prepares for interpreting. A file name and search
// path are passed in. The file may include other files. parseFile is called on each.
void parseProgram(String fileName, String searchPath) {
    pendingFiles = createList(null, null, null, false);
    prependToList(pendingFiles, fileName);
	Set* included = createStringSet(); // Parsed so far.
    programParsing = true;

    globalTable = createSymbolTable();
    procedureTable = createFunctionTable();
    functionTable = createFunctionTable();

    while (!isEmptyList(pendingFiles)) { // Iterate the files.
        String nextFile = (String) getLastListElement(pendingFiles);
        if (!isInSet(included, nextFile)) {
            addToSet(included, nextFile);
            parseFile(nextFile, searchPath); // May add to pendingFiles.
        }
        removeLastListElement(pendingFiles);
    }
    deleteList(pendingFiles);
    pendingFiles = null;
    programParsing = false;

    // If there were errors in the program say something about it.
    if (Perrors) { printf("The program contains errors.\n"); }
}

// parseFile parses a single script file with the yacc-generated parser. This function is static because it should
// only be called by parseProgram.
static void parseFile(String fileName, String searchPath) {
    if (!fileName || *fileName == 0) return;
    curFileName = fileName;
    currentFile = fopenPath(fileName, "r", searchPath);
    if (!currentFile) {
        printf("Error: file \"%s\" cannot be found.\n", fileName);
        curFileName = null;
        Perrors++;
        return;
    }
    if (debugging) printf("Parsing %s.\n", fileName);
    curLine = 1;
    yyparse(); // Yacc parser.
    fclose(currentFile);
}
