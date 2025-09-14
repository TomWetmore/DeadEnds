//
// DeadEnds Runscript
//
//  main.c is the DeadEnds RunScript program. It has three steps.
//   1. Create a Database from a Gedcom file.
//   2. Parse a DeadEnds script file into its internal form.
//   3. Run the script on the Database and write its output to a file.
//
//  usage: runscript -g gedcomfile -s scriptfile
//
//  If DE_GEDCOM_PATH and/or DE_SCRIPTS_PATH are defined, they may be used as search paths.
//
//  Created by Thomas Wetmore on 21 July 2024
//  Last changed on 13 September 2025.
//

#include "deadends.h"

// Local functions.
static void usage(void);
static void getArguments(int, char**, String*, String*);
static void getEnvironment(String*, String*);

// Main program of the RunScript program.
int main(int argc, char* argv[]) {
    // Get the files.
    fprintf(stderr, "%s: RunScript started.\n", getMsecondsStr());
    String gedcomFile = null;
    String scriptFile = null;
    String gedcomPath = null;
    String scriptPath = null;
    getArguments(argc, argv, &gedcomFile, &scriptFile);
    getEnvironment(&gedcomPath, &scriptPath);

    // Build the Database from the Gedcom file.
    gedcomFile = resolveFile(gedcomFile, gedcomPath, "ged");
    ErrorLog* errorLog = createErrorLog();
    Database* database = getDatabaseFromFile(gedcomFile, errorLog);
    if (lengthList(errorLog)) {
        showErrorLog(errorLog);
        exit(1);
    }
    fprintf(stderr, "%s: Database created.\n", getMsecondsStr());

    // Parse the program script.
    fprintf(stderr, "%s: Script parsed.\n", getMsecondsStr());
    Program* program = parseProgram(scriptFile, scriptPath);
    if (!program) {
        fprintf(stderr, "Error parsing program\n");
        exit(1);
    }

    // Run the program.
    runProgram(program, database, stdOutputFile());
    fprintf(stderr, "%s: RunScript done.\n", getMsecondsStr());
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

// usage prints the RunScript usage message.
static void usage(void) {
    fprintf(stderr, "usage: runscript -g gedcomfile -s scriptfile\n");
}
