// DeadEnds
//
// parse.c contains two functions, parseProgram and parseFile, which parse DeadEnds scripts.
//
//  Created by Thomas Wetmore on 4 January 2023.
//  Last changed on 20 April 2024.
//

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

static bool debugging = true;  //  Turn on local debugging in this file.
extern bool programParsing;     //  True when program files are being parsed.

//  Global variables defined and created here. Not mentioned in header files.
//--------------------------------------------------------------------------------------------------
SymbolTable *globalTable;             // Table of global variables.
FunctionTable *functionTable;         // Table of user-defined functions.
FunctionTable *procedureTable;        // Table of user-defined procedures.
List *pendingFileList;                // List of names of the files to be parsed.
String currentProgramFileName = null; // Name of file being parsed
FILE *currentProgramFile = null;      // File structure of the file being parsed.
int currentProgramLineNumber = 1;     // Current line number in file being parsed.

static void parseFile(String fileName, String searchPath);

// parseProgram parses a DeadEnds script and prepares for interpreting. A file name and search
// path are passed in. The file may include other files. This function is called on each.
void parseProgram(String fileName, String searchPath) {
    pendingFileList = createList(null, null, null, false); // Queue of files to parse.
    prependToList(pendingFileList, fileName); // Add first file to queue.
    programParsing = true;

    //  Use an IntegerTable to simulate a Set.
    Set* includedFileSet = createStringSet();

    globalTable = createSymbolTable(); // Global symbol table.
    procedureTable = createFunctionTable(); // User-defined procedures.
    functionTable = createFunctionTable();  // User-defined functions.

    while (!isEmptyList(pendingFileList)) {
        String pendingFile = (String) getLastListElement(pendingFileList); // Dequeue file.
        if (!isInSet(includedFileSet, pendingFile)) {
            addToSet(includedFileSet, pendingFile);

            // Parse the file. This may add elements to the tables and the pending file set.
            parseFile(pendingFile, searchPath);
        }
        // Free the saved copy of the file name.
        removeLastListElement(pendingFileList);
    }

    // Done with the pending file list so delete it.
    deleteList(pendingFileList);
    pendingFileList = null;

    // Done parsing.
    programParsing = false;

    // If there were errors in the program say something about it.
    if (Perrors) { printf("The program contains errors.\n"); }
}



//  parseFile - Parse a program file. The function calls the yacc-generated yyparse function.
//--------------------------------------------------------------------------------------------------
static void parseFile(String fileName, String searchPath)
//  fileName -- Name of the file to parse.
//  searchPath -- Searchpath used to locate the file.
{
    // Make sure the file name makes sense.
    if (!fileName || *fileName == 0) return;

    // Set the global file name.
    currentProgramFileName = fileName;
    // Try to open the file for reading and handle errors.
    currentProgramFile = fopenPath(fileName, "r", searchPath);
    if (!currentProgramFile) {
        printf("Error: file \"%s\" not found.\n", fileName);
        currentProgramFileName = null;
        Perrors++;
        return;
    }
    if (debugging) printf("File %s found and about to be processed\n", fileName);
    // The program file is open. Call yyparse to process it.
    currentProgramLineNumber = 1;
    yyparse();  // Use the yacc-generated parser to parse the file.
    fclose(currentProgramFile);
}
