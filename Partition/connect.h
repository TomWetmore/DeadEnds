// connect.h
// Partition
//
// Created by Thomas Wetmore on 5 October 2024.
// Last changed on 11 October 2024.

#ifndef connect_h
#define connect_h

#include <stdio.h>
#include "standard.h"
#include "gnode.h"
#include "gnodeindex.h"

// Connect data is the data field used in the GNodeIndexEls by the partition program. It holds
// the numbers of ancestors and descendents of the persons in the index.
typedef struct ConnectData {
	bool ancestorsDone;
	int numAncestors;
	bool descendentsDone;
	int numDescendents;
} ConnectData;

ConnectData* createConnectData(void);
void getConnections(List*, GNodeIndex*);
void debugGNodeIndex(GNodeIndex*);

#endif // connect_h
