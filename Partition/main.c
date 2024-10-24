// Partition
//
// main.c is the main program of the partition command. The program reads a Gedcom file and then
// partitions it into closed sets of persons and families.
//
// Created by Thomas Wetmore on 4 October 2024.
// Last changed on 22 October 2024.

#include <stdio.h>
#include "import.h"
#include "gnodelist.h"
#include "utils.h"
#include "stringtable.h"
#include "generatekey.h"
#include "writenode.h"
#include "file.h"
#include "connect.h"

static void getArguments(int, char**, String*);
static void getEnvironment(String*);
static void usage(void);
static void goAway(ErrorLog*);
static List* getPartitions(GNodeList*, GNodeIndex*, ErrorLog*);
static List* createPartition(GNode*, GNodeList*, GNodeIndex*, StringSet*, ErrorLog*);
static GNodeIndex* createIndexOfGNodes(GNodeList*);
static GNodeList* removeNonPersons(GNodeList*);
static void showConnects(List*, GNodeIndex*);
static bool debugging = false;
static bool timing = true;

// main is the main program of the partition program. It reads a Gedcom file into a GNodeList and
// creates a GNodeIndex to serve as database. It partitions the records into closed partitions of
// persons and families. It computes the numbers of ancestors and descendents of all persons.
int main(int argc, char** argv) {
	String gedcomFile = null;
	String searchPath = null;
	if (timing) printf("%s: Partition: begin.\n", getMillisecondsString());

	// Get the Gedcom file.
	getArguments(argc, argv, &gedcomFile);
	getEnvironment(&searchPath);
	gedcomFile = resolveFile(gedcomFile, searchPath);
	if (debugging) printf("Resolved file: %s\n", gedcomFile);

	// Read the file and get the list of all records.
	File* file = openFile(gedcomFile, "r");
	ErrorLog* log = createErrorLog();
	GNodeList* roots = getGNodeTreesFromFile(file, log); // All GNode roots parsed from file.
	if (timing) printf("%s: Partition: read gedcom file.\n", getMillisecondsString());
	if (lengthList(log) > 0) goAway(log);
	closeFile(file);

	// Validate record keys read from the Gedcom file.
	checkKeysAndReferences(roots, file->name, log);
	if (timing) printf("%s: Partition: validated keys.\n", getMillisecondsString());
	if (lengthList(log)) goAway(log);
	GNodeIndex* index = createIndexOfGNodes(roots); // Index of all GNodes.
	roots = removeNonPersons(roots);

	// Create the partitions.
	List* partitions = getPartitions(roots, index, log);
	if (timing) printf("%s: Partition: created partitions.\n", getMillisecondsString());

	// Get number of ancestors and descendents of all person.
	FORLIST(partitions, el)
		List* partition = (List*) el;
		getConnections(partition, index);
	ENDLIST
	printf("%s: Partition: computed connectedness numbers.\n", getMillisecondsString());

	// DEBUG: SHOW THE CONNECTIONS OF EACH PARTITION
	FORLIST(partitions, el)
		showConnects((List*) el, index);
	ENDLIST

	// Find the most connected person.
	int max = 0;
	GNode* topGun = null;
	FORLIST(roots, el)
		GNode* gnode = ((GNodeListEl*) el)->node;
		GNodeIndexEl* element = searchHashTable(index, gnode->key);
		ConnectData* data = element->data;
		int score = data->numAncestors + data->numDescendents;
		if (score > max) {
			max = score;
			topGun = gnode;
		}
	ENDLIST
	printf("Person: %s %s %d\n", topGun->key, topGun->child->value, max);
	if (timing) printf("%s: Partition: done.\n", getMillisecondsString());
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
static GNodeIndex* createIndexOfGNodes(GNodeList* list) {
	GNodeIndex* index = createGNodeIndex(deleteData);
	FORLIST(list, el)
		GNode* root = ((GNodeListEl*) el)->node;
		if (root->key) {
			ConnectData* data = createConnectData();
			addToGNodeIndex(index, root, data);
		}
	ENDLIST
	return index;
}

// getPartitions partitions a list of Gedcom records into a list of lists of GNodes. Each
// partition is a closed set of person records.
static List* getPartitions(GNodeList* gnodes, GNodeIndex* index, ErrorLog* log) {
	StringSet* visited = createStringSet(); // Visited GNode keys.
	List* partitions = createList(null, null, null, false); // List of partitions to return.
	FORLIST(gnodes, el)
		GNode* root = ((GNodeListEl*) el)->node;
		String key = root->key;
		if (key && !isInSet(visited, key)) { // root starts a new partition.
			appendToList(partitions, createPartition(root, gnodes, index, visited, log));
		}
	ENDLIST
	deleteStringSet(visited, false);
	return partitions;
}

// createPartition creates a partition by finding the closed set of Gedcom persons and families
// that contains the GNode argument. A partition is a list of GNodes.
static List* createPartition(GNode* root, GNodeList* gnodes, GNodeIndex* index,
							 StringSet* visited, ErrorLog* log) {
	List* partition = createList(null, null, null, false);
	List* queue = createList(null, null, null, false); // GNodes to process.
	prependToList(queue, root); // Init queue with first node.

	// Iterate until the queue is empty.
	while (lengthList(queue) > 0) {
		GNode* root = getAndRemoveLastListElement(queue);
		String key = root->key;
		if (isInSet(visited, key)) continue; // Skip if seen.
		addToSet(visited, key);
		if (recordType(root) == GRPerson) appendToList(partition, root); // Add persons only.

		// If root is a person add its FAMS and FAMC nodes.
		if (recordType(root) == GRPerson) {
			for (GNode* child = root->child; child; child = child->sibling) {
				String tag = child->tag;
				if (eqstr(tag, "FAMS") || eqstr(tag, "FAMC")) {
					String value = child->value;
					GNode* node = searchRecordIndex(index, value);
					if (!node) { // Can't happen.
						addErrorToLog(log, createError(linkageError, "file", 0, "Couldn't find a family"));
						continue;
					}
					prependToList(queue, node);
				}
			}
		// If root is a family add its HUSB, WIFE, and CHIL nodes.
		} else if (recordType(root) == GRFamily) {
			for (GNode* child = root->child; child; child = child->sibling) {
				String tag = child->tag;
				if (eqstr(tag, "HUSB") || eqstr(tag, "WIFE") || eqstr(tag, "CHIL")) {
					String value = child->value;
					GNode* node = searchRecordIndex(index, value);
					if (!node) { // Can't happen.
						addErrorToLog(log, createError(linkageError, "", 0, "Couldn't find a person"));
						continue;
					}
					prependToList(queue, node);
				}
			}
		}
	}
	deleteList(queue);
	return partition;
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

// removeNonPersons removes the non-person GNodes from a list of GNodes. The original List is
// deleted and the new one is returned.
static GNodeList* removeNonPersons(GNodeList* list) {
	GNodeList* newlist = createGNodeList();
	FORLIST(list, el)
		GNodeListEl* element = (GNodeListEl*) el;
		if (recordType(element->node) == GRPerson)
			appendToList(newlist, el);
	ENDLIST
	deleteList(list);
	return newlist;
}
