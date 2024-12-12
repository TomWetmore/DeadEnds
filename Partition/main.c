// Partition
//
// main.c is the main program of the partition program. It reads a Gedcom file and partitions it
// into closed sets of persons and families.
//
// Created by Thomas Wetmore on 4 October 2024.
// Last changed on 11 December 2024.

#include <stdio.h>
#include "import.h"
#include "gnodelist.h"
#include "utils.h"
#include "stringtable.h"
#include "generatekey.h"
#include "writenode.h"
#include "file.h"
#include "connect.h"
#include "partition.h"

#define gms getMsecondsStr()

static void getArguments(int, char**, String*);
static void getEnvironment(String*);
static void usage(void);
static void goAway(ErrorLog*);
static GNodeIndex* createIndexOfGNodes(GNodeList*);
static GNodeList* removeNonPersons(GNodeList*);
static void showConnects(List*, GNodeIndex*);
static bool debugging = false;
static bool timing = true;
static bool brownnose = false;

// main is the main program of the partition program. It reads a Gedcom file into a GNodeList and
// creates a GNodeIndex to serve as database. It partitions the records into closed partitions of
// persons. It computes the numbers of ancestors and descendents of all persons.
int main(int argc, char** argv) {
	String gedcomFile = null;
	String searchPath = null;
	if (timing) printf("%s: Partition: begin.\n", getMsecondsStr());

	// Get the Gedcom file.
	getArguments(argc, argv, &gedcomFile);
	getEnvironment(&searchPath);
	String resolvedFile = resolveFile(gedcomFile, searchPath);
	if (!resolvedFile) {
		fprintf(stderr, "Could not open file %s.\n", gedcomFile);
		exit(1);
	}
	if (debugging) printf("%s: partition: resolved file: %s\n", gms, resolvedFile);

	// Read the Gedcom file and get the list of its records.
	File* file = openFile(resolvedFile, "r");
	ErrorLog* log = createErrorLog();
	IntegerTable* keymap = createIntegerTable(4097);
	RootList* roots = getRootListFromFile(file, keymap, log); // All roots parsed from file.
	if (brownnose) showRootList(roots);
	if (timing) printf("%s: Partition: read gedcom file.\n", gms);
	if (debugging) printf("%s: Partition: |roots| = %d.\n", gms, lengthList(roots));
	if (lengthList(log) > 0) goAway(log);
	closeFile(file);

	// Validate record keys read from the Gedcom file.
	checkKeysAndReferences(roots, file->name, keymap, log);
	if (timing) printf("%s: Partition: validated keys.\n", gms);
	if (lengthList(log)) goAway(log);
	GNodeIndex* index = createIndexOfGNodes(roots); // Index of all GNodes.
	if (timing) printf("%s: Partition: createdIndexOfGNodes.\n", gms);
	if (debugging) printf("%s: Partition: |index| = %d.\n", gms, sizeHashTable(index));
	RootList* persons = removeNonPersons(roots);
	if (timing) printf("%s: Partition: non-persons removed from roots.\n", gms);
	if (debugging) printf("%s: Partition: |persons| = %d.\n", gms, lengthList(persons));

	// Create the partitions.
	List* partitions = getPartitions(persons, index, log);
	if (timing) printf("%s: Partition: created partitions.\n", gms);

	// Get number of ancestors and descendents of all persons.
	FORLIST(partitions, el)
		List* partition = (List*) el;
		getConnections(partition, index);
	ENDLIST
	if (timing) printf("%s: Partition: computed connectedness numbers.\n", gms);

	// DEBUG: SHOW THE CONNECTIONS OF EACH PARTITION
	FORLIST(partitions, el)
		showConnects((List*) el, index);
	ENDLIST

	// Find the most connected person.
	int max = 0;
	GNode* topGun = null;
	FORLIST(persons, el)
		GNode* person = (GNode*) el;
		GNodeIndexEl* element = searchHashTable(index, person->key);
		ConnectData* data = element->data;
		int score = data->numAncestors + data->numDescendents;
		if (score > max) {
			max = score;
			topGun = person;
		}
	ENDLIST
	printf("Person: %s %s %d\n", topGun->key, topGun->child->value, max);
	if (timing) printf("%s: Partition: done.\n", gms);
}

// showConnects is a debug function that shows the connect data of each person in a list.
static void showConnects(List* list, GNodeIndex* index) {
	printf("\nPartition:\n");
	FORLIST(list, el)
		GNode* root = el;
		GNodeIndexEl* element = searchHashTable(index, root->key);
		ConnectData* data = element->data;
		printf("%s: %s: %d: %d\n", root->key, root->child->value, data->numAncestors,
			   data->numDescendents);
	ENDLIST
}

// deleteData is the delete function use by GNodeIndex to delete the connect data areas.
static void deleteData(void* data) {
	stdfree(data);
}

// createIndexOfRecords creates a GNodeIndex from a GNodeList. Its elements have the ConnectData.
static GNodeIndex* createIndexOfGNodes(RootList* list) {
	GNodeIndex* index = createGNodeIndex(deleteData);
	FORLIST(list, el)
		GNode* root = (GNode*) el;
		if (root->key) {
			ConnectData* data = createConnectData();
			addToGNodeIndex(index, root, data);
		}
	ENDLIST
	return index;
}

// getArguments gets the Gedcom file name from the command line.
static void getArguments(int argc, char* argv[], String* gedcomFile) {
	int ch;
	while ((ch = getopt(argc, argv, "g:")) != -1) {
		switch(ch) {
		case 'g':
			*gedcomFile = strsave(optarg);
			break;
		case '?':
		default:
			usage();
			exit(1);
		}
	}
	if (!*gedcomFile) {
		usage();
		exit(1);
	}
}

// getEnvironment checks for the DE_GEDCOM_PATH environment variable.
static void getEnvironment(String* gedcomPath) {
	*gedcomPath = getenv("DE_GEDCOM_PATH");
	if (!*gedcomPath) *gedcomPath = ".";
}

// usage prints the Partition usage message.
static void usage(void) {
	fprintf(stderr, "usage: partition -g gedcomfile\n");
}

// goAway prints the error log and quits.
static void goAway(ErrorLog* log) {
	printf("Partition: cancelled due to errors\n");
	showErrorLog(log);;
	exit(1);
}

// removeNonPersons removes the non-person records from a RootList of records. The original
// RootList is deleted and the new one is returned.
static RootList* removeNonPersons(RootList* list) {
	RootList* newlist = createRootList();
	FORLIST(list, el)
		if (recordType((GNode*) el) == GRPerson)
			appendToList(newlist, el);
	ENDLIST
	deleteList(list);
	return newlist;
}


static void showqueue(List* queue) {
	FORLIST(queue, element)
		printf("%s ",  ((GNode*) element)->key);
	ENDLIST
	printf("\n");
}
