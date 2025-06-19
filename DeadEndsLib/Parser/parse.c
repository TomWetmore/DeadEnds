//
//  DeadEnds Library
//
//  parse.c contains two functions, parseProgram and parseFile, which parse DeadEnds scripts.
//
//  Created by Thomas Wetmore on 4 January 2023.
//  Last changed on 18 June 2025.
//

#include <stdarg.h>
#include <unistd.h>
#include "context.h"
#include "functiontable.h"
#include "gedcom.h"
#include "hashtable.h"
#include "integertable.h"
#include "interp.h"
#include "list.h"
#include "parse.h"
#include "path.h"
#include "pnode.h"
#include "pvalue.h"
#include "set.h"
#include "stringset.h"
#include "symboltable.h"

static bool debugging = false;
extern bool programParsing;

// Shared global variables. Memory ownership of the first four are taken over by the context.
SymbolTable* globals; // Global symbol table.
FunctionTable* functions; // User functions.
FunctionTable* procedures; // User procedures.
List* parsedFiles; // Parsed files (interned copies of file names.

List* pendingFiles; // Files to be parsed.
String curFileName = null; // File being parsed
FILE* currentFile = null; // FILE being parsed.
int curLine = 1; // Line number in current file.

static void parseFile(String file, String path); // Private function defined below.

// parseProgram parses a DeadEnds script and creates a context object for interpreting. The name of a script file
// and a search path are passed in. The file may include other files. parseFile is called on each. Notes:
// parsedFiles is a list of the names of all files parsed. These names are used as the targets for the file
// pointers in all the PNodes making up the script. The list becomes part of the context. pendingFiles
// is the queue of script files awaiting parsing. It is deleted at the end. globals, procedures, and
// functions are global variables shared with the parser, where the parsed objects, particularly the PNode
// trees of the procedures and functions are stored. Once parsing is done, links to the three tables are put in
// the context and the global variables are set to null. parsed files is the list of names of the parsed files.

Context* parseProgram(String fileName, String searchPath) {

    // Create the data structues needed to parse the script.
    parsedFiles = createList(null, null, null, false);  // List of parsed file names.
    pendingFiles = createList(null, null, null, false); // Queue of pending file names.
	Set* included = createStringSet(); // Set of parsed file names.
    globals = createSymbolTable(); // Table of global variables.
    procedures = createFunctionTable(); // Table of user-defined procedures.
    functions = createFunctionTable(); // Table of user-defined functions.

    // Initialize the pending files queue with the main program.
    enqueueList(pendingFiles, fileName);
    programParsing = true;

    // Process the queue by parsing each file.
    while (!isEmptyList(pendingFiles)) {
        // Process the next file from the pending files queue.
        String nextFile = (String) dequeueList(pendingFiles);
        if (!isInSet(included, nextFile)) {
            addToSet(included, nextFile);
            appendToList(parsedFiles, nextFile);
            parseFile(nextFile, searchPath); // May add to the pending files.
        }
    }
    // Delete the structures no longer needed.
    deleteList(pendingFiles);
    deleteSet(included);
    // If there were errors delete the other structures and return null.
    if (Perrors) {
        deleteList(parsedFiles);
        deleteSymbolTable(globals);
        deleteFunctionTable(procedures);
        deleteFunctionTable(functions);
        return null;
    }

    // Parsing was successful. Create and return a context.
    Context* context = createEmptyContext();
    context->fileNames = parsedFiles;
    context->globals = globals;
    context->procedures = procedures;
    context->functions = functions;
    // Null the globals after the context has taken ownership.
    globals = null;
    procedures = functions = null;
    parsedFiles = null;
    programParsing = false;
    return context;
}

// parseFile parses a single script file with the yacc-generated parser. This function private to this file.
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
