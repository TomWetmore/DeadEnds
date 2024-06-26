// DeadEnds
//
// generatekey.c has the functions to generate random keys.
//
// Created by Thomas Wetmore on 1 June 2024.
// Last changed on 15 June 2024.

#include "stdlib.h"
#include "time.h"
#include "gedcom.h"
#include "stringtable.h"
#include "stringset.h"

static void initKeyGen(void);
static String generate(char);

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

// generateKey generates a new random Gedcom key ('cross-reference identifier').
static bool first = true;
String generateKey(RecordType recType) {
	if (first) initKeyGen();
	return generate(recordChar(recType));
}

// initKeyGen initializes random key generation.
static char keyBuffer[10];
static void initKeyGen(void) {
	srand((unsigned)time(0));
	first = false;
	keyBuffer[0] = keyBuffer[8] = '@';
	keyBuffer[9] = 0;
}

// inUse returns true if the key is in use.
static bool inUse(String key) {
	return false;
}

// generate generates a new random key.
static String generate(char type) {
	int infiniteLoopProtection = 50;
	int counter = 0;
	keyBuffer[1] = type;
	while (counter++ < infiniteLoopProtection) {
		for (int i = 2; i <= 7; i++) {
			keyBuffer[i] = keyCharacters[rand() % 36];
		}
		if (inUse(keyBuffer)) continue;
		return strsave(keyBuffer);
	}
	exit(2); // Could not generate a key.
	return null;
}

// Functions when reading a Gedcom file.
// Variables:
// Table of all keys encountered or created so far.
// Table of all keys found in previously read Gedcom files.
// Table of all keys found in reading the current Gedcom file.
// What to do when findint a root key.
// Has it been seen before in previous Gedcom files

//if (newRoot) {
//  newKey = gnode->key;
//	if (hasBeenSeenInPreviousFiles(key)) {
//		newKey = generateNewKey();
//		addToHashTable(newKeysInCurrentFile, key, newKey, true, false);
//	} else if (hasBeenSeenInCurrentFile(key)) {
//      error(duplicate)
//  } else {
//		addToHashTable(keysInCurrentFile, key, key, true, false);
//  }
//} else if (hasBeenSeenInPreviousFiles(key)) {
//		if (hasBeenSeenIn(newKeysInCurrentFile)

Set* inPreviousFiles;
Set* inCurrentFile;
StringTable* newKeys;

bool keySeenBefore(String key) { return true; }
bool keyInPreviousFiles(String key) {
	return isInSet(inPreviousFiles, key);
}
bool keyInCurrentFile(String key) {
	return isInSet(inCurrentFile, key);
}

void processKey(String key, bool defining) {
	if (defining) {

	} else {

	}

}
