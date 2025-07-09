//
//  DeadEnds Library
//
//  gnode.h defines the GNode data type. GNodes represent lines in a Gedcom file.
//
//  Created by Thomas Wetmore on 4 November 2022.
//  Last changed on 4 June 2025.
//

#ifndef gnode_h
#define gnode_h

#include "standard.h"

typedef struct Database Database;
typedef struct HashTable HashTable;
typedef enum SexType SexType;
typedef HashTable RecordIndex;

// GNode is the structure that holds a Gedcom line in its 'internal' form.
typedef struct GNode GNode;
struct GNode {
	String key;     // Record key; only root nodes use this field.
	String tag;     // Line tag; all nodes use this field.
	String value;   // Line value; values are optional.
	GNode *parent;  // Parent node; all nodes except roots use this field.
	GNode *child;   // First child none of this node, if any.
	GNode *sibling; // Next sibling node of this node, if any.
};

// Application programming interface to this type.
GNode* createGNode(String key, String tag, String value, GNode* parent);
void freeGNode(GNode*);
void freeGNodes(GNode*);
int gnodeLevel(GNode* node);

String gnodeToString(GNode*, int level);
String gnodesToString(GNode*);
int treeStringLength(int, GNode*);
GNode* personToFamilyAsChild(GNode *person, RecordIndex*);

String personToEvent(GNode*, String, String, int, bool);
String eventToString(GNode*, bool);
String eventToDate(GNode*, bool);
String eventToPlace(GNode*, bool);
void showGNodeTree(GNode*);
void showGNodes(int, GNode*);
void showGNode(int level, GNode*);
int lengthGNodes(GNode*);
String shortenDate(String);
String shortenPlace(String);
//static bool allDigits(String)
GNode* copyGNode(GNode*);
GNode* copyGNodes(GNode*, bool, bool);
void traverseNodes (GNode* node, int level, bool (*func)(GNode*, int));
int num_spouses_of_indi(GNode*);
GNode* findNode(GNode*, String, String, GNode**);

int countGNodes(GNode* node);
int countNodesBefore(GNode*);

bool isKey(String);
GNode* findTag(GNode*, String);
SexType valueToSex(GNode*);
String full_value(GNode*);
String recordKey(GNode* node);

int numGNodeAllocs(void);
int numGNodeFrees(void);

#endif // gnode_h
