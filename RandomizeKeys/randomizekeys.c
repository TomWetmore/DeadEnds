// DeadEnds Tool
//
// randomizekeys.c is the DeadEnds tool that randomizes the keys in a Gedcom file.
//
// Created by Thomas Wetmore on 14 July 2024.
// Last changed on 21 July 2024.

#include "randomizekeys.h"
#include "stringtable.h"
#include "writenode.h"
#include "file.h"

FILE* debugFile = null; // Need to get rid of this.
static void goAway(ErrorLog*);

// main is the main program of the randomize keys batch program.
int main(void) {
	printf("randomize keys: %s: start.\n", getMillisecondsString());
	// Get the Gedcom records from a file.
	String fileName = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/rekeyed.ged";
	File* file = openFile(fileName, "r");
	ErrorLog* log = createErrorLog();

	// Parse the Gedcom file and build a GNodeList of all of its records.
	GNodeList* roots = getGNodeTreesFromFile(file, log);
	printf("ramdomize keys: %s: read gedcom file.\n", getMillisecondsString());
	if (lengthList(log) > 0) goAway(log);
	closeFile(file);

	// Validate the keys.
	checkKeysAndReferences(roots, file->name, log);
	printf("ramdomize keys: %s: validated keys.\n", getMillisecondsString());
	if (lengthList(log)) {
		deleteGNodeList(roots, true);
		goAway(log);
	}

	// Create a table that maps existing keys to random keys.
	StringTable* keyTable = createStringTable(1025);
	initRecordKeyGenerator();
	FORLIST(roots, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		String key = root->key;
		if (!key) continue;
		RecordType r = recordType(root);
		String newKey = generateRecordKey(r); // Get a random key.
		addToStringTable(keyTable, key, newKey);
	ENDLIST
	printf("ramdomize keys: %s: created remap table.\n", getMillisecondsString());

	// Change the keys throughout the roots list.
	FORLIST(roots, element)
		// Change the key on the root.
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		String key = root->key;
		if (!key) continue;
		String new = searchStringTable(keyTable, key);
		root->key = new;
		// Change all values that are keys.
		FORTRAVERSE(root, node)
			if (isKey(node->value)) {
				new = searchStringTable(keyTable, node->value);
				node->value = strsave(new);
			}
		ENDTRAVERSE
	ENDLIST
	printf("ramdomize keys: %s: rekeyed records.\n", getMillisecondsString());

	// Write the modified GNodeList as a new Gedcom file.
	fileName = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/rekeyed.ged";
	file = openFile(fileName, "w");
	FORLIST(roots, element)
		GNodeListEl* el = (GNodeListEl*) element;
		writeGNodeRecord(file->fp, el->node, false);
	ENDLIST
	printf("ramdomize keys: %s: wrote gedcom file.\n", getMillisecondsString());
	closeFile(file);
	return 0;
}

static void goAway(ErrorLog* log) {
	printf("randomizekeys: cancelled due to errors\n");
	showErrorLog(log);;
	exit(1);
}
