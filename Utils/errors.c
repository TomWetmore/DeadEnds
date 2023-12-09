//
//  DeadEnds
//
//  errors.c -- Code for handling DeadEnds errors.
//
//  Created by Thomas Wetmore on 4 July 2023.
//  Last changed on 3 December 2023.
//

#include "errors.h"
#include "list.h"

static bool debugging = true;

//  getErrKey -- Get the comparison key of an error. Required list function.
//
//    This needs a little thought. We need to sort the errors by a key made up of the file name
//    and the line number. But The sorting has to be done numerically with the line numbers and
//    not lexicographably.
//--------------------------------------------------------------------------------------------------
#define NUMKEYS 64
static String getErrKey(Word error)
{
	static char buffer[NUMKEYS][128];
	static int dex = 0;
	if (++dex > NUMKEYS - 1) dex = 0;
	String scratch = buffer[dex];
	String fileName = ((Error*) error)->fileName;
	if (!fileName) fileName = "";
	int lineNumber = ((Error*) error)->lineNumber;
	//  MNOTE: The key is returned out of static memory. The caller must make a copy if it needs
	//  to persist beyond NUMKEYs calls of this function.
	sprintf(scratch, "%s%09d", fileName, lineNumber);
	return scratch;
}

//  cmpError -- Compare two errors for their placement in an error log. Required list function.
//--------------------------------------------------------------------------------------------------
static int cmpError(Word errorOne, Word errorTwo)
{
	String key1 = getErrKey(errorOne);
	String key2 = getErrKey(errorTwo);
	return strcmp(key1, key2);
}

//  delError -- Free up the memory for an error when an error log is freed. Required list function.
//--------------------------------------------------------------------------------------------------
static void delError(Word error)
{
	String message = ((Error*) error)->message;
	if (message) stdfree(message);
	stdfree(error);
	return;
}

//  createErrorLog -- Create an error log. An ErrorLog is a specialized List.
//--------------------------------------------------------------------------------------------------
ErrorLog *createErrorLog(void)
{
	ErrorLog *errorLog = createList(cmpError, delError,  getErrKey);
	errorLog->keepSorted = true;
	return errorLog;
}

//  createError -- Create an Error.
//--------------------------------------------------------------------------------------------------
Error *createError(ErrorType type, String fileName, int lineNumber, String message)
{
	Error *error = (Error*) stdalloc(sizeof(Error));
	error->type = type;
	error->severity = severeError;  //  Override with setSeverityError.
	error->fileName = fileName;  // MNOTE: Not saved; do not free.
	error->lineNumber = lineNumber;
	error->message = strsave(message);
	if (debugging) printf("            CREATE ERROR: %s, %d, %s\n", fileName, lineNumber, message);
	return error;
}

//  setSeverityError -- Set the severity of an Error. By default severity is set to severeError.
//    Use this function to alter this value.
//-------------------------------------------------------------------------------------------------
void setSeverityError(Error *error, ErrorSeverity severity)
{
	error->severity = severity;
}

//  deleteError -- Delete an Error. MNOTE: Not freeing fileName.
//-------------------------------------------------------------------------------------------------
void deleteError (Error *error)
{
	if (error->message) stdfree(error->message);
	stdfree(error);
}

//  addErrorToLog -- Add an Error to an ErrorLog.
//-------------------------------------------------------------------------------------------------
void addErrorToLog (ErrorLog *errorLog, Error *error)
{
	if (!error) return;
	appendListElement(errorLog, error);
}

//  addErrorToLog -- Add an Error to an ErrorLog.
//--------------------------------------------------------------------------------------------------
void oldAddErrorToLog(ErrorLog *errorLog, ErrorType errorType, String fileName, int lineNumber,
	String message)
{
	appendListElement(errorLog, createError(errorType, fileName, lineNumber, message));
}

//  showError -- Show an Error on standard output.
//-------------------------------------------------------------------------------------------------
void showError (Error *error)
{
	/*switch (error->severity) {
		case fatalError: printf("fatal "); break;
		case severeError: printf("severe "); break;
		case warningError: printf("warning: "); break;
		case commentError: printf("comment: "); break;
		default: printf("unknown (can't happen):"); break;
	}*/
	switch (error->type) {
		case systemError: printf("system error "); break;
		case syntaxError: printf("syntax error "); break;
		case gedcomError: printf("semantic error "); break;
		case linkageError: printf("linkage error "); break;
		default: printf("unknown error (can't happen) "); break;
	}
	printf("in %s ", error->fileName ? error->fileName : "no filename");
	printf("at line %d: ", error->lineNumber);
	printf("%s\n", error->message ? error->message : "no message");
}

//  showErrorLog -- Show the contents of an ErrorLog on standard output.
//-------------------------------------------------------------------------------------------------
void showErrorLog (ErrorLog *errorLog)
{
	//sortList(errorLog, true);
	FORLIST(errorLog, error)
		showError((Error*) error);
	ENDLIST
}
