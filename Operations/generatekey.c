// DeadEnds
//
// generatekey.c has the functions to generate random keys.
//
// Created by Thomas Wetmore on 1 June 2024.
// Last changed on 7 June 2024.

#include <stdio.h>

#include "stdlib.h"
#include "time.h"
#include "gedcom.h"
#include "stringtable.h"
#include "stringset.h"

static void initKeyGen(void);
static String generate(char);

static bool first = true;
static StringTable* keyTable; // Key to GNode* root map??
static StringSet* keySet; // Or just a Set of keys?

// keyCharacters are the 36 characters that make up key values.
static char keyCharacters[36] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
};

// recordChar returns a character for each record type.
static char recordChar(RecordType recType) {
	if (recType == GRPerson) return 'I';
	if (recType == GRFamily) return 'F';
	if (recType == GRSource) return 'S';
	if (recType == GREvent) return 'E';
	return 'X';
}

static char keyBuffer[10];

// generateKey generates a new random Gedcom key ('cross-reference identifier').
String generateKey(RecordType recType) {
	if (first) initKeyGen();
	return generate(recordChar(recType));
}

// initKeyGen initializes the key generation.
static void initKeyGen(void) {
	srand((unsigned)time(0));
	first = false;
	keyBuffer[0] = keyBuffer[8] = '@';
	keyBuffer[9] = 0;
}

// seenBefore returns true if the key is already in use.
static bool seenBefore(String key) {
	return false;
}

// generate does the work of generating a new random key.
static String generate(char type) {
	int infiniteLoopProtection = 50;
	int counter = 0;
	keyBuffer[1] = type;
	while (counter++ < infiniteLoopProtection) {
		for (int i = 2; i <= 7; i++) {
			keyBuffer[i] = keyCharacters[rand() % 36];
		}
		if (seenBefore(keyBuffer)) continue;
		return strsave(keyBuffer);
	}
	exit(2); // Could not make a new key.
	return null;
}



