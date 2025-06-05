//
//  DeadEnds (Partition)
//  connect.h is a header file used by the Partition program. It defines the ConnectData type used as data
//  in GNodeIndexEls.
//
//  Created by Thomas Wetmore on 5 October 2024.
//  Last changed on 5 June 2025.
//

#ifndef connect_h
#define connect_h

#include "standard.h"

typedef struct GNode GNode;
typedef struct HashTable HashTable;
typedef struct List List;
typedef HashTable GNodeIndex;

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
