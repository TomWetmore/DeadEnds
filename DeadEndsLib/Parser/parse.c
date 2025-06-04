//
//  DeadEnds Library
//
//  parse.c contains two functions, parseProgram and parseFile, which parse DeadEnds scripts.
//
//  Created by Thomas Wetmore on 4 January 2023.
//  Last changed on 4 June 2025.
//

#include <stdarg.h>
#include <unistd.h>
#include "context.h"
#include "functiontable.h"
#include "gedcom.h"
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

// Shared global variables. Memory ownership of the first four are taken over by the Script object.
SymbolTable* globals; // Global symbol table.
FunctionTable* functions; // User functions.
FunctionTable* procedures; // User procedures.
List* parsedFiles; // Parsed files (interned copies of file names.

List* pendingFiles; // Files to be parsed.
String curFileName = null; // File being parsed
FILE* currentFile = null; // FILE being parsed.
int curLine = 1; // Line number in current file.

static void parseFile(String file, String path);

// parseProgram parses a DeadEnds script and creates a Program object for interpreting. The name of a script file
// and a search path are passed in. The file may include other files. parseFile is called on each. Notes:
// parsedFiles is a list of the names of all files parsed. These names are used as the targets for the file
// pointers in all the PNodes making up the script. The list becomes part of the Program object. pendingFiles
// is the queue of script files awaiting parsing. It is deleted at the end. globalTable, procedureTable, and
// functionTable are the global variables shared with the parser where the parsed objects, particularly the PNode
// trees of the procedures and functions are stored. Once parsing is done, links to the three tables are put in
// the Program object and the global variables are set to null.
// parsed files is a simple list of the file names of the main script file and the files it includes
Context* parseProgram(String fileName, String searchPath) {

    parsedFiles = createList(null, null, null, false);
    pendingFiles = createList(null, null, null, false);
    enqueueList(pendingFiles, fileName);
	Set* included = createStringSet(); // Parsed so far.
    programParsing = true;

    globals = createSymbolTable();
    procedures = createFunctionTable();
    functions = createFunctionTable();

    while (!isEmptyList(pendingFiles)) { // Iterate the files.
        String nextFile = (String) dequeueList(pendingFiles);
        if (!isInSet(included, nextFile)) {
            addToSet(included, nextFile);
            appendToList(parsedFiles, nextFile);
            parseFile(nextFile, searchPath); // May add to pendingFiles.
        }
    }
    deleteList(pendingFiles);
    deleteSet(included);
    if (Perrors) return null;

    // Parsing was successful. Create and return a Context object.
    Context* context = createEmptyContext();
    context->fileNames = parsedFiles;
    context->globals = globals;
    context->procedures = procedures;
    context->functions = functions;
    globals = null;
    procedures = functions = null;
    parsedFiles = null;
    programParsing = false;
    return context;
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
