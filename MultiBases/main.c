// DeadEnds
//
// main.c is the main program of the MultiBases command line program in the DeadEnds family.
// The first version reads a list of Gedcom files and builds a list of Databases. Later versions
// will experiment with Database merging.
//
// Created by Thomas Wetmore on 16 November 2024.
// Last changed on 17 November 2024.

#include "standard.h"
#include "utils.h"
#include "list.h"
#include "database.h"
#include "import.h"
#include "errors.h"

static bool debugging = true;
static bool timing = true;
static String getGedcomPath(void);
static List* getFileNames(int, char**);
static List* listFromStrings(String);
static List* resolveFileNames(List*, String);

// main is the main program of the MultiBases test program.
int main(int argc, char *argv[]) {
	printf("%s: MultiBases started.\n", getMillisecondsString());

	// Get the Gedcom file search path.
	String gedcomPath = getGedcomPath();
	if (debugging) printf("gedcomPath is %s\n", gedcomPath);

	// Get the list of Gedcom files to process into Databases.
	List* names = getFileNames(argc, argv); // Get List of Gedcom files to process.
	if (!names) {
		fprintf(stderr, "Impossible; must be a bug to fix\n");
		return 1;
	}
	List* resolvedNames = resolveFileNames(names, gedcomPath); // Resolve the Gedcom files.
	deleteList(names);

	// Get the List of Databases from the List of Gedcom files.
	ErrorLog* errorLog = createErrorLog();
	List* databases = gedcomFilesToDatabases(resolvedNames, errorLog);

	// Show the results.
	printf("The number of databases created was %d\n", lengthList(databases));
	printf("The number of errors logged was %d\n", lengthList(errorLog));
	if (lengthList(errorLog)) showErrorLog(errorLog);
}

// getGedcomPath gets the Gedcom file search path, looking for the value of DE_GEDCOM_PATH.
static String getGedcomPath(void) {
	String path = getenv("DE_GEDCOM_PATH");
	return path ? path : ".";
}

// getFileNames gets the list of Gedcom files to process. These are the files names given on
// the command line. They must be resolved with the Gedcom path variable.
static List* getFileNames(int argc, char** argv) {
	int opt;
	while ((opt = getopt(argc, argv, "m:")) != -1) {
		switch (opt) {
		case 'm':
			return listFromStrings(optarg);
		default:
			fprintf(stderr, "Usage: %s -m value1,value2,...\n", argv[0]);
			return null;
		}
	}
	fprintf(stderr, "Usage: %s -m value1,value2,...\n", argv[0]);
	return null;
}

// getListFromStrings creates a List of Strings from a comma-separated String.
static void delete(void* element) { stdfree(element); }
List* listFromStrings(const String string) {
	char *input = strsave(string); // Duplicate for safe tokenization
	List* strings = createList(null, null, delete, false);
	char *token = strtok(input, ",");
	while (token) {
		if (debugging) printf("name: %s\n", token);
		appendToList(strings, strsave(token));
		token = strtok(NULL, ",");
	}
	free(input);
	return strings;
}

// resolveFileNames resolves a List of file names with a path environment variable.
List* resolveFileNames(List* names, String path) {
	List* resolvedNames = createList(null, null, delete, false);
	FORLIST(names, element)
		String resolved = resolveFile(element, path);
		if (resolved) appendToList(resolvedNames, resolved);
		else fprintf(stderr, "Could not resolve %s\n", (String) element);
	ENDLIST
	return resolvedNames;
}
