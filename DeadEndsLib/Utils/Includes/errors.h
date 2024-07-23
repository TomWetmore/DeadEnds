// DeadEnds
//
// errors.h is the header file for DeadEnds Errors.
//
// Created by Thomas Wetmore on 4 July 2023.
// Last changed on 21 May 2024.

#ifndef errors_h
#define errors_h

#include "standard.h"
#include "list.h"

//  ErrorType is the type of a DeadEnds Error.
typedef enum ErrorType {
	systemError,
	syntaxError,
	gedcomError,
	linkageError
} ErrorType;

// ErrorSeverity is the severity of a DeadEnds Error.
typedef enum ErrorSeverity {
	fatalError,   // Quit loading database
	severeError,  // Continue with file but don't keep database
	warningError, // Continue with file and load database
	commentError  // Message for user
} ErrorSeverity;

// Error is the structure for holding a DeadEnds Error.
typedef struct Error {
	ErrorType type;
	ErrorSeverity severity;
	String fileName;
	int lineNumber;
	String message;
} Error;

// User interface.
#define ErrorLog List
ErrorLog *createErrorLog(void);
void deleteErrorLog(ErrorLog*);
Error *createError(ErrorType type, String fileName, int lineNumber, String message);
void deleteError(Error*);
void addErrorToLog(ErrorLog*, Error*);
void showErrorLog(ErrorLog*);
void showError(Error*);

#endif // errors_h
