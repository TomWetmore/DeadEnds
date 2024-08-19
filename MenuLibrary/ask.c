//
//  ask.c
//  MenuLibrary
//
// Created by Thomas Wetmore on 8 August 2024.
// Last changed on 9 September 2024

#include "standard.h"
#include "ask.h"
#include "list.h"
#include "stdarg.h"
#include "regex.h"

static bool checkForInteger(String, int*);
static bool isInStringList(String, List*);

static bool debugging = true;

// askForInteger asks a user for an integer value.
AskReturn askForInteger(String msg, int returnSet, int* result) {
	static char response[256];
	printf("%s: ", msg); // Try first time outside retry loop.
	fgets(response, sizeof(response), stdin);
	response[strcspn(response, "\n")] = '\0';
	if (checkForInteger(response, result)) return askOkay;
	while (true) {
		if (returnSet & askQuit) { // User allowed to quit.
			printf("%s is not an integer; please reenter or hit q to quit: ", response);
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (checkForInteger(response, result)) return askOkay;
			if (eqstr(response, "q")) return askQuit;
		} else { // User not allowed to quit.
			printf("%s is not an integer; please reenter: ", response);
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = '\0';
			if (checkForInteger(response, result)) return askOkay;
		}
	}
	return askOkay; // Will never reach here.
}

// askForStringInSet asks a user to choose from a set of Strings.
AskReturn askForStringInSet(String msg, List* strings, int returnSet, String* result) {
	static char response[256];
	printf("%s: ", msg); // Try first time outside retry loop.
	fgets(response, sizeof(response), stdin);
	response[strcspn(response, "\n")] = 0;
	if (isInStringList(response, strings)) {
		*result = response;
		return askOkay;
	}
	while (true) { // Retry loop.
		if (returnSet & askQuit) {
			printf("%s is not a proper choice; please enter one of", response);
			FORLIST(strings, element)
			printf(" %s", (String) element);
			ENDLIST
			printf(" or hit q to quit: ");
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (isInStringList(response, strings)) {
				*result = response;
				return askOkay;
			} else if (strcmp(response, "q")) {
				*result = null;
				return askQuit;
			}
		} else {
			printf("%s is not a proper choice; please enter one of", response);
			FORLIST(strings, element)
			printf(" %s", (String) element);
			ENDLIST
			printf(": ");
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (isInStringList(response, strings)) {
				*result = response;
				return askOkay;
			}
		}
	}
	return askOkay; // Will never get here.
}

AskReturn askForString(String msg, int returnSet, String* result) {
	static char response[256];
	printf("%s: ", msg); // Try first time outside retry loop.
	fgets(response, sizeof(response), stdin);
	response[strcspn(response, "\n")] = 0;
	if (strlen(response)) {
		*result = response;
		return askOkay;
	}
	while (true) { // Retry loop.
		if (returnSet & askQuit) {
			printf("Please enter a string or q to quit :");
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (strcmp(response, "q")) {
				*result = null;
				return askQuit;
			}
			if (strlen(response)) {
				*result = response;
				return askOkay;
			}
		} else {
			printf("Please enter a string: ");
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (strlen(response)) {
				*result = response;
				return askOkay;
			}
		}
	}
	return askOkay; // Will never get here.
}



// checkForInteger checks that a String is numeric; if so returns the number through a parameter.
bool checkForInteger(String string, int* result) {
	int accum = 0;
	*result = 0; // Return 0 on failure.
	for (int i = 0; string[i]; i++) { // TODO: Add large iteration protection.
		if (!isdigit(string[i])) return false;
		accum = accum*10 + string[i] - '0';
	}
	*result = accum;
	return true;
}

// isInStringList checks for a String in a List of Strings.
bool isInStringList(String string, List* strings) {
	FORLIST(strings, element)
	if (eqstr(string, (String) element)) return true;
	ENDLIST
	return false;
}

// createStringList creates a List of Strings of any non-zero length. Caller must pass null as
// the last argument to end the list.
// MNOTE: User is responible for freeing the list; when freed the String elements are also freed.
static void del(void* element) { stdfree(element); }
List* createStringList(String first, ...) {
	va_list args;
	List* list = createList(null, null, del, false);
	va_start(args, first);
	for (String arg = first; arg; arg = va_arg(args, String)) {
		appendToList(list, strsave(arg));
	}
	va_end(args);
	return list; // MNOTE: Caller responsible for freeing.
}

// askForPattern asks the user to enter a String that matches a regular expression.
AskReturn askForPattern(String msg, String pattern, int returnSet, String* result) {
	printf("%s: ", msg);
	regex_t regex;
	int reti = regcomp(&regex, pattern, REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "Could not compile regular expression: %s\n", pattern);
		return askFail;
	}
	// Try once outside the retry loop.
	static char response[256];
	fgets(response, sizeof(response), stdin);
	response[strcspn(response, "\n")] = 0;
	reti = regexec(&regex, response, 0, null, 0);
	if (reti == 0) {
		*result = response;
		regfree(&regex);
		return askOkay;
	}
	while (true) {
		if (returnSet & askQuit) {
			printf("%s is not a proper choice: please reenter or hit q: ", response);
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			if (eqstr(response, "q")) {
				regfree(&regex);
				*result = null;
				return askQuit;
			} else {
				reti = regexec(&regex, response, 0, null, 0);
				if (reti == 0) {
					regfree(&regex);
					*result = response;
					return askOkay;
				}
			}
		} else {
			printf("%s is not a proper choice: please renter: ", response);
			fgets(response, sizeof(response), stdin);
			response[strcspn(response, "\n")] = 0;
			reti = regexec(&regex, response, 0, null, 0);
			if (reti == 0) {
				regfree(&regex);
				*result = response;
				return askOkay;
			}
		}
	}
	return askOkay; // Cannot get here.
}

AskReturn askForPerson(Database* database, int codes, GNode** proot) {
	String name = null;
	// Ask first time outside of loop.
	AskReturn rcode = askForString("Enter the name of a person: ", askQuit, &name);
	if (rcode == askQuit) return rcode; // User backed out.

	return askOkay;
}

AskReturn getPersonFromName(Database* database, String name, GNode** root) {
	// Get the list of persons who match the name.
	// If the list is of length one, return that one person.
	// Get user to choose person from a List.
	// If user doesn't choose return askQuit.
	// If user does choose return askOkay with the GNode* of the person's root.
	return askOkay;
}

