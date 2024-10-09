// connect.h
// Partition
//
// Created by Thomas Wetmore on 5 October 2024.
// Last changed on 9 October 2024.

#ifndef connect_h
#define connect_h

#include <stdio.h>
#include "standard.h"
#include "gnode.h"
#include "gnodeindex.h"

// Connect data is the data field to be used in the GNodeIndexEls used by the partition program.
// It keeps track of the numbers of ancestors and descendents of persons in a partition.
typedef struct ConnectData {
	bool ancestorsDone;
	int numAncestors;
	bool descendentsDone;
	int numDescendents;
	int numVisits;
} ConnectData;

ConnectData* createConnectData(void);
void getConnections(List*, GNodeIndex*);
void debugGNodeIndex(GNodeIndex*);

#endif // connect_h
