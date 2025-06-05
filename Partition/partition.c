//
//  DeadEnds Partition
//
//  partition.c contains functions that partitions persons from a GNodeList into a List of
//  RootLists of persons in closed sets based on FAMS, FAMC, HUSB, WIFE & CHIL relationships.
//
//  Created by Thomas Wetmore on 11 December 2024.
//  Last changed on 5 June 2025.
//

#include <stdio.h>
#include "deadends.h"

#define gms getMsecondsStr()
static List* createPartition(GNode*, GNodeIndex*, StringSet*, ErrorLog*);
static bool debugging = false;

// getPartitions partitions a RootList of persons into a List of RootLists of persons. Each
// partition is a closed set of persons. Persons is the RootList of all persons from a
// Gedcom source, and index is the GNodeIndex of the persons and families from the source.
List* getPartitions(RootList* persons, GNodeIndex* index, ErrorLog* log) {
	if (debugging) printf("%s: getPartitions: start: |persons|: %d, |index|: %d.\n", gms,
						  lengthList(persons), sizeHashTable(index));
	StringSet* visited = createStringSet(); // Keys of persons and families already processed.
	List* partitions = createList(null, null, null, false); // List of partitions returned.
	FORLIST(persons, el)
		GNode* person = (GNode*) el;
		if (!isInSet(visited, person->key)) // Get person who starts the next partition.
			appendToList(partitions, createPartition(person, index, visited, log));
	ENDLIST
	deleteStringSet(visited, false);
	return partitions;
}

// createPartition finds the closed set of persons that the argument person belongs to. Root is
// a person, index is the GNodeIndex of all persons and families from a Gedcom file, visited is
// the StringSet of person keys that have been visited. A partition is a RootList of persons.
static List* createPartition(GNode* person, GNodeIndex* index, StringSet* visited, ErrorLog* log) {
	if (debugging) printf("%s: createPartition: start.\n", gms);
	RootList* partition = createRootList(); // The new partition.
	List* queue = createList(null, null, null, false); // Queue of persons and families to add.
	prependToList(queue, person); // Initialize queue with first person.

	// Iterate until the queue is empty.
	while (lengthList(queue) > 0) {
		GNode* curr = getAndRemoveLastListElement(queue); // Could be person or family.
		String key = curr->key;
		if (isInSet(visited, key)) continue; // Skip if already processed.
		addToSet(visited, key);
		if (recordType(curr) == GRPerson) appendToList(partition, curr); // Add persons only.

		// If curr is a person add its FAMS and FAMC families to the queue.
		if (recordType(curr) == GRPerson) {
			for (GNode* child = curr->child; child; child = child->sibling) {
				String tag = child->tag;
				if (eqstr(tag, "FAMS") || eqstr(tag, "FAMC")) {
					String value = child->value;
					GNode* node = searchGNodeIndex(index, value);
					if (!node) { // Can't happen in a validated index.
						addErrorToLog(log, createError(linkageError, "file", 0, "Couldn't find a family"));
						continue;
					}
					prependToList(queue, node);
				}
			}
		// If curr is a family add its HUSB, WIFE, and CHIL persons to the queue.
		} else if (recordType(curr) == GRFamily) {
			for (GNode* child = curr->child; child; child = child->sibling) {
				String tag = child->tag;
				if (eqstr(tag, "HUSB") || eqstr(tag, "WIFE") || eqstr(tag, "CHIL")) {
					String value = child->value;
					GNode* node = searchGNodeIndex(index, value);
					if (!node) { // Can't happen in a validated index.
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
