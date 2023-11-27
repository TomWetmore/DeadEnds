//
//  DeadEnds Project
//
//  gnode.h -- GNode datatype. GNodes represent lines in a Gedcom file. GNodes are heap objects.
//
//  Created by Thomas Wetmore on 4 November 2022.
//  Last changed on 25 November 2023.
//

#ifndef gnode_h
#define gnode_h

typedef struct HashTable HashTable;  // Forward reference needed.
typedef struct Database Database;
typedef enum SexType SexType;

#include "standard.h"
#include "gedcom.h"
#include "hashtable.h"
#include "database.h"

//  GNode -- Data object that holds a Gedcom line in its tree node form. Only root nodes have
//    key fields, and the @-signs have been removed. Remember this throughout.
//--------------------------------------------------------------------------------------------------
typedef struct GNode GNode;
struct GNode {
	String key;     // Record key; only root nodes use this field; @-signs are removed.
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

// Prototypes of functions defined in node.c
//static String fixup(String str);
//static String fix_tag(String str);
//static void free_node(Node);

// Application programming interface to this type.
//--------------------------------------------------------------------------------------------------
GNode* createGNode(String key, String tag, String value, GNode* parent);  // Create a Node.
void freeGNodes(GNode* node);  // Free a node tree.
int gnodeLevel(GNode* node);  // Return the level of a GNode in its tree.

String gnodeToString(GNode*, int level);
String gnodesToString(GNode*);
int treeStringLength(int, GNode*);
GNode* personToFamilyAsChild(GNode *person, Database*);

String personToEvent(GNode*, String, String, int, bool);
String eventToString(GNode*, bool);
String eventToDate(GNode*, bool);
String event_to_plac(GNode*, bool);
void show_node(GNode*);
void show_node_rec(int, GNode*);
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

//String addat(String); //  Deprecated.
//String rmvat(String); //  Deprecated.
bool isKey(String);
GNode* findTag(GNode*, String);
SexType val_to_sex(GNode*);
String full_value(GNode*);

int numNodeAllocs(void);
int numNodeFrees(void);

//Going to have to face the music on these soon.
//GNode* keyToPerson(String);
//GNode* keyToFamily(String);

#endif /* node_h */
