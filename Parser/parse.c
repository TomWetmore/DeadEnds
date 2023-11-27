//
//  DeadEnds
//
//  parse.c -- Contains two functions, parseProgram and parseFile, to parse DeadEnds programs.
//
//  Created by Thomas Wetmore on 4 January 2023.
//  Last changed on 7 July 2023.
//

#include "parse.h"
#include <stdarg.h>
#include "functiontable.h"
#include "integertable.h"
#include "gedcom.h"
#include "interp.h"
#include "list.h"
#include "path.h"
#include "pnode.h"
#include <unistd.h>  // sleep.

static bool debugging = true;  //  Turn on local debugging in this file.
extern bool programParsing;     //  True when program files are being parsed.

//  Global variables defined and created here. Not mentioned in header files.
//--------------------------------------------------------------------------------------------------
SymbolTable *globalTable;             //  Table of global variables.
FunctionTable *functionTable;         //  Table of user-defined functions.
FunctionTable *procedureTable;        //  Table of user-defined procedures.
List *pendingFileList;                //  List of names of the files to be parsed.
String currentProgramFileName = null; //  Name of file being parsed
FILE *currentProgramFile = null;      //  File structure of the file being parsed.
int currentProgramLineNumber = 1;     //  Current line number in file being parsed.

//  parseFile -- Parse a single file. Private to this file.
static void parseFile(String fileName, String searchPath);

//  parseProgram -- Parse a DeadEnds program and prepare it for interpreting. The name of the
//    file with the main procedure is passed in with the search path to find it. This is the
//    first file parsed. It may include other files which may include others and so on. This
//    function also parses the included files and makes sure not to reparse any. This function
//    calls parseFile on each file, which in turn uses the yacc-generated yyparse function to
//    do the actual parsing. (The main procedure does not have to be in the first file.)
//--------------------------------------------------------------------------------------------------
void parseProgram(String fileName, String searchPath)
//  fileName -- Name of the first file to parse; normally has the main procedure.
//  searchPath -- Search path for finding the files.
{
    //  Create a list of files to be parsed.
    pendingFileList = createList(null, null, null);
    //set_list_type(pendingFileList, LISTDOFREE);  // TODO: HOPE THIS CAN GO.

    //  Add the first file to the list.
    prependListElement(pendingFileList, strsave(fileName));

    //  Set the state variable that is true during parsing.
    programParsing = true;

    //  Use an IntegerTable to simulate a Set.
    //    TODO: Why not use a real set type?
    IntegerTable *includedFileSet = createIntegerTable();

    //  Create the global tables needed for interpreting; they are populated by yyparse.
    globalTable = createSymbolTable();      // Global variables.
    //if (debugging) printf("parseProgram: Create symbol table %p for global variables\n", globalTable);
    procedureTable = createFunctionTable(); // User-defined procedures.
    functionTable = createFunctionTable();  // User-defined functions.

    // Loop that parses the files making up the program.
    while (!isEmptyList(pendingFileList)) {

        // Remove the next pending filename from the list.
        String pendingFile = (String) removeLastListElement(pendingFileList);
        
        //  If the file hasn't been seen, parse it and add it to the included set. Do nothing
        //    if it has been seen.
        if (!isInHashTable(includedFileSet, pendingFile)) {
            insertInIntegerTable(includedFileSet, pendingFile, 0);

            // Parse the file. This may add elements to the tables and the pending file set.
            parseFile(pendingFile, searchPath);
        }
        // Free the saved copy of the file name.
        stdfree(pendingFile);
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
