//
//  DeadEnds Project
//
//  gnode.h -- GNode datatype. GNodes represent lines in a Gedcom file. GNodes are heap objects.
//
//  Created by Thomas Wetmore on 4 November 2022.
//  Last changed on 9 February 2024.
//

#ifndef gnode_h
#define gnode_h

typedef struct HashTable HashTable;  // Forward reference.
typedef struct Database Database;
typedef enum SexType SexType;

#include "standard.h"
#include "gedcom.h"
#include "hashtable.h"
#include "database.h"

//  GNode -- Data object that holds a Gedcom line in its tree node form. Only root nodes have
//    keys. It would be useful to have fields for the nodes level and its line number from its
//    Gedcom file. Less convenient means of finding these values exist.
//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------
GNode* createGNode(String key, String tag, String value, GNode* parent);  // Create a Node.
void freeGNode(GNode*);  // Free a GNode.
void freeGNodes(GNode*);  // Free a GNode forest.
int gnodeLevel(GNode* node);  // Return the level of a GNode in its tree.

String gnodeToString(GNode*, int level);
String gnodesToString(GNode*);
int treeStringLength(int, GNode*);
GNode* personToFamilyAsChild(GNode *person, Database*);

String personToEvent(GNode*, String, String, int, bool);
String eventToString(GNode*, bool);
String eventToDate(GNode*, bool);
String eventToPlace(GNode*, bool);
void showGNode(GNode*);
void showGNodeRecursive(int, GNode*);
void showSingleGNode(int level, GNode*);
int length_nodes(GNode*);
String shorten_date(String);
String shorten_plac(String);
//static bool allDigits(String)
GNode* copy_node(GNode*);
GNode* copy_nodes(GNode*, bool, bool);
void traverseNodes (GNode* node, int level, bool (*func)(GNode*, int));
int num_spouses_of_indi(GNode*);
GNode* find_node(GNode*, String, String, GNode**);
GNode* fatherNodes(Database*, GNode*);
GNode* mother_nodes(Database*, GNode*);
GNode* children_nodes(Database*, GNode*);
GNode* parents_nodes(Database*, GNode*);

int countNodes(GNode* node); // Count GNodes in a tree.
int countNodesBefore(GNode*);  // Count the GNodes before a node in a tree.

bool isKey(String);
GNode* findTag(GNode*, String);
SexType valueToSex(GNode*);
String full_value(GNode*);
String recordKey(GNode* node);  // Return the key of the record that contains the node.

int numNodeAllocs(void);  // Return the numbmer of GNodes that have been allocated.
int numNodeFrees(void);  // Return the number of GNodes that have been freed.

#endif // node_h
