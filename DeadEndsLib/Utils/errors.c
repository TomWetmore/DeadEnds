//  DeadEnds
//
//  errors.c has code for handling DeadEnds errors.
//
//  Created by Thomas Wetmore on 4 July 2023.
//  Last changed on 10 February 2025.

#include "errors.h"
#include "list.h"

#define NUMKEYS 64
static bool debugging = false;

// getKey returns the comparison key of an error.
static String getKey(void* error) {
	static char buffer[NUMKEYS][128];
	static int dex = 0;
	if (++dex > NUMKEYS - 1) dex = 0;
	String scratch = buffer[dex];
	String fileName = ((Error*) error)->fileName;
	if (!fileName) fileName = "";
	int lineNumber = ((Error*) error)->lineNumber;
	sprintf(scratch, "%s%09d", fileName, lineNumber);
	return scratch; // Static memory!
}

// compare compares two errors for their placement in an error log.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// delete frees an Error from an ErrorLog.
static void delete(void* error) {
	String message = ((Error*) error)->message;
	if (message) stdfree(message);
	stdfree(error);
	return;
}

// createErrorLog creates an error log, a specialized List.
ErrorLog* createErrorLog(void) {
	ErrorLog* errorLog = createList(getKey, compare, delete, false);
	errorLog->sorted = true;
	return errorLog;
}

// createError creates an Error.
Error* createError(ErrorType type, String fileName, int lineNumber, String message) {
	Error* error = (Error*) stdalloc(sizeof(Error));
	error->type = type;
	error->severity = severeError;
	error->fileName = fileName; // Do not free.
	error->lineNumber = lineNumber;
	error->message = strsave(message);
	if (debugging) printf("CREATE ERROR: %s, %d, %s\n", fileName, lineNumber, message);
	return error;
}

// setSeverityError changes the severity of an Error.
void setSeverityError(Error* error, ErrorSeverity severity) {
	error->severity = severity;
}

// deleteError deletes an Error.
void deleteError (Error* error) {
	if (error->message) stdfree(error->message);
	stdfree(error);
}

// addErrorToLog adds an Error to an ErrorLog.
void addErrorToLog (ErrorLog* errorLog, Error* error) {
	if (!error) return;
	appendToList(errorLog, error);
}

// showError shows an Error on standard output.
void showError(Error* error) {
	printf("error");
	if (error->fileName) printf(" in %s", error->fileName);
	if (error->lineNumber) printf(" line %d:", error->lineNumber);
	if (error->message) printf(": %s.\n", error->message);
}

// deleteErrorLog deletes an ErrorLog.
void deleteErrorLog(ErrorLog* errorLog) {
	deleteList(errorLog);
}

// showErrorLog shows the contents of an ErrorLog on standard output.
void showErrorLog(ErrorLog* errorLog) {
	if (!errorLog) {
		printf("Error log does not exist.\n");
		return;
	}
	if (lengthList(errorLog) == 0) {
		printf("Error log is empty.\n");
		return;
	}
	printf("Error log:\n");
	sortList(errorLog);
	FORLIST(errorLog, error)
		showError((Error*) error);
	ENDLIST
}
