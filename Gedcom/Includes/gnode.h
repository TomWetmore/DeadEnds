// DeadEnds Project
//
// gnode.h -- GNode datatype. GNodes represent lines in a Gedcom file. GNodes are heap objects.
//
// Created by Thomas Wetmore on 4 November 2022.
// Last changed on 30 April 2024.

#ifndef gnode_h
#define gnode_h

typedef struct HashTable HashTable;  // Forward reference.
typedef struct Database Database;
typedef enum SexType SexType;

#include "standard.h"
#include "gedcom.h"
#include "hashtable.h"
#include "database.h"

// GNode is the structure that holds a Gedcom line in its tree node form. Root nodes have keys.
typedef struct GNode GNode;
struct GNode {
	String key;     // Record key; only root nodes use this field.
	String tag;     // Line tag; all nodes use this field.
	String value;   // Line value; values are optional.
	GNode *parent;  // Parent node; all nodes except roots use this field.
	GNode *child;   // First child none of this node, if any.
	GNode *sibling; // Next sibling node of this node, if any.
};

//String fileof = (String) "The file is as positioned at EOF.";
//String reremp = (String) "Line %d: This line is empty; EOF?";
//String rerlng = (String) "Line %d: This line is too long.";
//String rernlv = (String) "Line %d: This line has no level number.";
//String rerinc = (String) "Line %d: This line is incomplete.";
//String rerbln = (String) "Line %d: This line has a bad link.";
//String rernwt = (String) "Line %d: This line needs white space before tag.";
//String rerilv = (String) "Line %d: This line has an illegal level.";
//String rerwlv = (String) "The record begins at wrong level.";

// Application programming interface to this type.
GNode* createGNode(String key, String tag, String value, GNode* parent);
void freeGNode(GNode*);
void freeGNodes(GNode*);
int gnodeLevel(GNode* node);

String gnodeToString(GNode*, int level);
String gnodesToString(GNode*);
int treeStringLength(int, GNode*);
GNode* personToFamilyAsChild(GNode *person, Database*);

String personToEvent(GNode*, String, String, int, bool);
String eventToString(GNode*, bool);
String eventToDate(GNode*, bool);
String eventToPlace(GNode*, bool);
void showGNodeTree(GNode*);
void showGNodes(int, GNode*);
void showGNode(int level, GNode*);
int gNodeListLength(GNode*);
String shortenDate(String);
String shortenPlace(String);
//static bool allDigits(String)
GNode* copyNode(GNode*);
GNode* copyNodes(GNode*, bool, bool);
void traverseNodes (GNode* node, int level, bool (*func)(GNode*, int));
int num_spouses_of_indi(GNode*);
GNode* findNode(GNode*, String, String, GNode**);

int countNodes(GNode* node);
int countNodesBefore(GNode*);

bool isKey(String);
GNode* findTag(GNode*, String);
SexType valueToSex(GNode*);
String full_value(GNode*);
String recordKey(GNode* node);

int numNodeAllocs(void);
int numNodeFrees(void);

#endif // node_h
