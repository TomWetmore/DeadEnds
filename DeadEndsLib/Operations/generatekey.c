// DeadEnds
//
// generatekey.c has the functions to generate random keys.
//
// Created by Thomas Wetmore on 1 June 2024.
// Last changed on 14 March 2025.

#include "stdlib.h"
#include "time.h"
#include "gedcom.h"
#include "stringtable.h"
#include "stringset.h"

// Locals.
static StringSet* keysInUse = null; // Set of generated keys.
static String generate(char type);
static void initRecordKeyGenerator(void);

// keyCharacters are the 36 characters that make up key values.
static char keyCharacters[36] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
};

// recordChar returns the character that starts the key for each record type.
static char recordChar(RecordType recType) {
	if (recType == GRPerson) return 'I';
	if (recType == GRFamily) return 'F';
	if (recType == GRSource) return 'S';
	if (recType == GREvent) return 'E';
	return 'X';
}

// generateRecordKey generates a new random Gedcom key ('cross-reference identifier').
// This is the public interface.
String generateRecordKey(RecordType recType) {
	static bool first = true;
	if (first) {
		initRecordKeyGenerator();
		first = false;
	}
	return generate(recordChar(recType));
}

// initRecordKeyGenerator initializes random key generation.
static char keyBuffer[10];
void initRecordKeyGenerator(void) {
	srand((unsigned)time(0));
	keyBuffer[0] = keyBuffer[8] = '@';
	keyBuffer[9] = 0;
	if (keysInUse) deleteStringSet(keysInUse, true);
	keysInUse = createStringSet();
}

// inUse returns true if the key is in use.
static bool inUse(String key) {
	return isInSet(keysInUse, key);
}

// generateRecordKey generates a random record key.
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
	fprintf(stderr, "Fatal error: could not generate a record key.\n");
	exit(2);
	return null;
}

