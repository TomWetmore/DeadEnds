// ask.h
// CloneOne
//
// Created by Thomas Wetmore on 9 August 2924.
// Last changed on 16 August 2024.

#include "standard.h"
#include "list.h"
#include "database.h"

#ifndef ask_h
#define ask_h

// AskRetun is the type of the return codes from the ask user functions.
typedef enum AskReturn {
	askOkay = 1,
	askFail = 2,
	askQuit = 4,
} AskReturn;

// Interface to the ask functions.
AskReturn askForInteger(String, int, int*);
AskReturn askForStringInSet(String, List*, int, String*);
AskReturn askForString(String, int, String*);
AskReturn askForPattern(String, String pattern, int, String*);

AskReturn askForPerson(Database*, int, GNode**);

// NOTE: Think about moving this into the DataType sub-library.
List* createStringList(String, ...);

#endif // ask_h
