//
//  DeadEnds Project
//
//  gnode.c -- Functions involving the GNode type.
//
//  Created by Thomas Wetmore on 12 November 2022.
//  Last changed on 2 January 2024..

#include "standard.h"
#include "gnode.h"
#include "nodeutils.h"
#include "lineage.h"
#include "stringtable.h"
#include "name.h"
#include "gedcom.h"
#include "splitjoin.h"
#include "readnode.h"
#include "database.h"

typedef struct HashTable StringTable;

//  tagTable -- Table that ensures there is one persistent copy of each tag used in all GNodes.
//--------------------------------------------------------------------------------------------------
static StringTable *tagTable = null;

//  numNodeAllocs -- Return the number of nodes that have been allocated in the heap. Debugging.
//--------------------------------------------------------------------------------------------------
static int nodeAllocs = 0;
int numNodeAllocs(void)
{
	return nodeAllocs;
}

//  numNodeFrees -- Return the number of nodes that have been freed to the heap. Debugging.
//--------------------------------------------------------------------------------------------------
static int nodeFrees = 0;
int numNodeFrees(void)
{
	return nodeFrees;
}

//  getFromTagTable -- Return the persistent copy of a tag from the tag table.
//--------------------------------------------------------------------------------------------------
static String getFromTagTable(String tag)
//  tag -- Return the copy of this tag from the tag table.
{
	if (tagTable) return fixString(tagTable, tag);
	tagTable = createStringTable();
	return fixString(tagTable, tag);
}

//  freeGNode -- Free a GNode.
//--------------------------------------------------------------------------------------------------
void freeGNode(GNode* node)
// node -- GNode to be freed.
{
	ASSERT(node);
	if (node->key) stdfree(node->key);
	if (node->value) stdfree(node->value);
	nodeFrees++;
	stdfree(node);
}

//  createGNode -- Create, initialize and return a GNode.
//--------------------------------------------------------------------------------------------------
GNode* createGNode(String key, String tag, String value, GNode* parent)
//  key -- Cross reference key; only level 0 nodes have them.
//  tag -- Tag; all nodes have them.
//  value -- Value; optional.
//  parent -- Parent node; root nodes don't have them.
{
	nodeAllocs++;
	GNode* node = (GNode*) stdalloc(sizeof(GNode));;
	node->key = strsave(key);
	node->tag = getFromTagTable(tag);  // Get persistent copy.
	node->value = strsave(value);
	node->parent = parent;
	node->child = null;
	node->sibling = null;
	return node;
}

//  freeGNodes -- Free all GNodes in a tree or forest of GNodes. This function recurses through
//    all the GNodes in the tree or forest and calls freeGNode on each.
//--------------------------------------------------------------------------------------------------
void freeGNodes(GNode* node)
// node -- Root GNode to free.
{
	while (node) {
		if (node->child) freeGNodes(node->child);  // Recurse to children.
		GNode* sibling = node->sibling;  // Tail recurse to siblings.
		freeGNode(node);
		node = sibling;
	}
}

//  gnodeLevel -- Return the level of a node in its tree. Works by counting parent links, so the
//    node and tree must be structurally sound.
//--------------------------------------------------------------------------------------------------
int gnodeLevel(GNode* node)
//  node -- The node to have its level determined.
{
	int level = 0;
	while (node->parent) {
		level++;
		node = node->parent;
	}
	return level;
}

//  personToEvent -- Convert an event tree into a string.
//  MNOTE: The string returned is in static memory. Callers must retain a copy if necessary.
//--------------------------------------------------------------------------------------------------
String personToEvent(GNode* node, String tag, String head, int len, bool shorten)
{
	ASSERT(node);
	static char scratch[200];
	String event;
	size_t n;
	if (!node) return null;
	if (!(node = findTag(node->child, tag))) return null;
	event = eventToString(node, shorten);
	if (!event) return null;
	sprintf(scratch, "%s%s", head, event);
	n = strlen(scratch);
	if (scratch[n-1] != '.') {
		scratch[n] = '.';
		scratch[++n] = 0;
	}
	if (n > len) scratch[len] = 0;
	return scratch;
}

//  eventToString -- Convert an event to a string.
//  MNOTE: The string returned is in static memory. Callers must retain a copy if necessary.
//--------------------------------------------------------------------------------------------------
String eventToString (GNode* node, bool shorten)
{
	static char scratch[MAXLINELEN+1];
	String date, plac, p;
	date = plac = null;
	if (!node) return null;
	node = node->child;
	while (node) {
		if (eqstr("DATE", node->tag) && !date) date = node->value;
		if (eqstr("PLAC", node->tag) && !plac) plac = node->value;
		node = node->sibling;
	}
	if (!date && !plac) return null;
	if (shorten) {
		date = shorten_date(date);
		plac = shorten_plac(plac);
		if (!date && !plac) return null;
	}
	p = scratch;
	if (date && !plac) {
		strcpy(p, date);
		p += strlen(p);
	}
	if (date && plac) {
		strcpy(p, date);
		p += strlen(p);
		strcpy(p, ", ");
		p += 2;
		strcpy(p, plac);
		p += strlen(p);
	}
	if (!date && plac) {
		strcpy(p, plac);
		p += strlen(p);
	}
	*p = 0;
	return scratch;
}

// eventToDate -- Return the date of an event as a string.
//--------------------------------------------------------------------------------------------------
String eventToDate (GNode* node, bool shorten)
{
	if (!node) return null;
	if (!(node = DATE(node))) return null;
	if (shorten) return shorten_date(node->value);
	return node->value;
}

//  event_to_plac -- Convert event to place
//--------------------------------------------------------------------------------------------------
String event_to_plac (GNode* node, bool shorten)
{
	if (!node) return null;
	node = PLAC(node);
	if (!node) return null;
	if (shorten) return shorten_plac(node->value);
	return node->value;
}

//  showGNode -- Show a tree of GNodes; for debugging.
//--------------------------------------------------------------------------------------------------
 void showGNode(GNode* node)
{
	if (!node) return;
	showGNodeRecursive(0, node);
}

// showGNodeRecursive -- Recursive version of show_node.
//--------------------------------------------------------------------------------------------------
void showGNodeRecursive(int levl, GNode* node)
{
	if (!node) return;
	for (int i = 1;  i < levl;  i++)
		printf("  ");

	printf("%d", levl);
	if (node->key) printf(" %s", node->key);
	printf(" %s", node->tag);
	if (node->value) printf(" %s", node->value);
	printf("\n");
	showGNodeRecursive(levl + 1, node->child);
	showGNodeRecursive(levl    , node->sibling);
}

void showSingleGNode(int level, GNode *node)
{
	printf("%d", level);
	if (node->key) printf(" %s", node->key);
	printf(" %s", node->tag);
	if (node->value) printf(" %s", node->value);
	printf("\n");
}

// length_nodes -- Return the length of the list of Nodes starting with first.
//--------------------------------------------------------------------------------------------------
int length_nodes(GNode* first)
// Node first -- first Node in the list of Nodes to count.
{
	int len = 0;
	while (first) {
		len++;
		first = first->sibling;
	}
	return len;
}

// shorten_date -- Return the short form of a date value.
//--------------------------------------------------------------------------------------------------
String shorten_date(String date)
{
	static char buffer[3][MAXLINELEN+1];
	static int dex = 0;
	String p = date, q;
	int c, len;
	/* Allow 3 or 4 digit years. The previous test for strlen(date) < 4
	 * prevented dates consisting of only 3 digit years from being
	 * returned. - pbm 12 oct 99 */
	if (!date || (int) strlen(date) < 3) return null;
	if (++dex > 2) dex = 0;
	q = buffer[dex];
	while (true) {
		while ((c = *p++) && chartype(c) != DIGIT)
			;
		if (c == 0) return null;
		q = buffer[dex];
		*q++ = c;
		len = 1;
		while ((c = *p++) && chartype(c) == DIGIT) {
			if (len < 6) {
				*q++ = c;
				len++;
			}
		}
		*q = 0;
		if (strlen(buffer[dex]) == 3 || strlen(buffer[dex]) == 4)
			return buffer[dex];
		if (c == 0) return null;
	}
}

// shorten_plac -- Return the short form of a place value.
//--------------------------------------------------------------------------------------------------
String shorten_plac(String plac)
{
	String plac0 = plac, comma /* , val */;
	if (!plac) return null;
	comma = (String) strrchr(plac, ',');
	if (comma) plac = comma + 1;
	while (*plac++ == ' ')
		;
	plac--;
	if (*plac == 0) return plac0;
	//if ((val = (String) valueof(placabbvs, plac))) return val;
	return plac;
}

// all_digits -- Check if a String is all digits.
//--------------------------------------------------------------------------------------------------
//static bool all_digits(String s)
//{
//	int c;
//	while ((c = *s++)) {
//		if (c < '0' || c > '9') return false;
//	}
//	return true;
//}

// copy_node -- Copy a Node.
//--------------------------------------------------------------------------------------------------
GNode* copy_node(GNode* node)
{
	return createGNode(node->key, node->tag, node->value, null);
}

// copy_nodes -- Copy a tree of Nodes.
//--------------------------------------------------------------------------------------------------
GNode* copy_nodes (GNode* node, bool kids, bool sibs)
//  node -- Node tree to copy.
//  kids -- If true also copy children.
//  sibs -- if true also cope siblings.
{
	GNode* new;
	GNode* kin;
	if (!node) return null;
	new = copy_node(node);
	if (kids && node->child) {
		kin = copy_nodes(node->child, true, true);
		ASSERT(kin);
		new->child = kin;
		while (kin) {
			kin->parent = new;
			kin = kin->sibling;
		}
	}
	if (sibs && node->sibling) {
		kin = copy_nodes(node->sibling, kids, true);
		ASSERT(kin);
		new->sibling = kin;
	}
	return new;
}

// traverseNodes -- Traverse the nodes of a Gedcom record tree calling a function on each.
//--------------------------------------------------------------------------------------------------
void traverseNodes (GNode* node, int level, bool (*func)(GNode*, int))
//  node -- Current node in tree.
//  level -- Level of the node.
//  func -- Function to call at node. If it returns false don't go deeper.
{
	if (func == null) return; // Nothing to do if there is no function.
	for (; node; node = node->sibling) {
		if (!(*func)(node, level)) continue;
		if (node->child) traverseNodes(node->child, level+1, func);
	}
}

//  countNodes -- Return the number of gedcom nodes in a record tree.
//--------------------------------------------------------------------------------------------------
int countNodes (GNode* node)
{
	if (node == null) return 0;
	int count = 1 + countNodes(node->child) + countNodes(node->sibling);
	if (count > 100000) {
		printf("Recursing forever?????\n");
		exit(3);
	}
	return count;
}

//  find_node -- Find child node with specific tag and value. Either or both of tag and value
//    must be non-null.
//--------------------------------------------------------------------------------------------------
GNode* find_node (GNode* parent, String tag, String value, GNode **plast)
//  parent -- Reqested node must be a child of this node.
//  tag -- Tag of requested node.
//  value -- Value of requested node; if null no value check is done.
//  plast -- (out) Previous node to the requested node if any; may be null.
{
	ASSERT(parent && (tag || value));
	if (plast) *plast = null;
	for (GNode *last = null, *node = parent->child; node; last = node, node = node->sibling) {
		if (tag && nestr(tag, node->tag)) continue;
		if (value && nestr(value, node->value)) continue;
		if (plast) *plast = last;
		return node;
	}
	return null;
}

//  recordKey -- Return the key of the record the node is from.
//--------------------------------------------------------------------------------------------------
String recordKey (GNode* node)
{
	if (!node) return null;
	int count = 0;
	while (node->parent) {
		if (++count > 100) {
			printf("Infinite loop in tree parent chain?");
			ASSERT(false);
		}
		node = node->parent;
	}
	return node->key;
}

// fatherNodes -- Given a list of FAMS or FAMC nodes, return the list of HUSB nodes they contain.
//--------------------------------------------------------------------------------------------------
GNode* fatherNodes (Database* database, GNode* faml)
// Node faml -- List of FAMC and/or FAMS Nodes.
{
	GNode *refn, *husb, *wife, *chil, *rest;
	GNode *old = null, *new = null;
	while (faml) {
		ASSERT(eqstr("FAMC", faml->tag) || eqstr("FAMS", faml->tag));
		GNode *fam = keyToFamily(faml->tag, database);
		ASSERT(fam);
		splitFamily(fam, &refn, &husb, &wife, &chil, &rest);
		new = union_nodes(old, husb, false, true);
		freeGNodes(old);
		old = new;
		joinFamily(fam, refn, husb, wife, chil, rest);
		faml = faml->sibling;
	}
	return new;
}

// mother_nodes -- Given a list of FAMS and FAMC nodes, return the list of WIFE Nodes they contain.
//--------------------------------------------------------------------------------------------------
GNode* mother_nodes (Database *database, GNode *faml)
// fam -- List of FAMC and/or FAMS nodes.
{
	GNode *refn, *husb, *wife, *chil, *rest;
	GNode *old = null, *new = null;
	while (faml) {
		ASSERT(eqstr("FAMC", faml->tag) || eqstr("FAMS", faml->tag));
		GNode *fam = keyToFamily(faml->tag, database);
		ASSERT(fam);
		splitFamily(fam, &refn, &husb, &wife, &chil, &rest);
		new = union_nodes(old, wife, false, true);
		freeGNodes(old);
		old = new;
		joinFamily(fam, refn, husb, wife, chil, rest);
		faml = faml->sibling;
	}
	return new;
}

//  children_nodes -- Given a list of FAMS or FAMC nodes, return the list of CHIL Nodes their
//    families contain.
//--------------------------------------------------------------------------------------------------
GNode* children_nodes(Database *database, GNode *faml)
//  database -- Database of records.
//  faml -- List of FAMC or FAMS node.
{
	GNode *refn, *husb, *wife, *chil, *rest;
	GNode *old = null, *new = null;
	while (faml) {
		ASSERT(eqstr("FAMC", faml->tag) || eqstr("FAMS", faml->tag));
		GNode *fam = keyToFamily(faml->tag, database);
		ASSERT(fam);
		splitFamily(fam, &refn, &husb, &wife, &chil, &rest);
		new = union_nodes(old, chil, false, true);
		freeGNodes(old);
		old = new;
		joinFamily(fam, refn, husb, wife, chil, rest);
		faml = faml->sibling;
	}
	return new;
}

//  parents_nodes -- Given list of FAMS or FAMC nodes, returns list of HUSB
//    and WIFE lines they contain
//--------------------------------------------------------------------------------------------------
GNode* parents_nodes(Database *database, GNode* family)
// Node family -- List of FAMC and/or FAMS Nodes.
{
	GNode *refn, *husb, *wife, *chil, *rest;
	GNode *old = null, *new = null;
	while (family) {
		ASSERT(eqstr("FAMC", family->tag) || eqstr("FAMS", family->tag));
		GNode *fam = keyToFamily(family->value, database);
		ASSERT(fam);
		splitFamily(fam, &refn, &husb, &wife, &chil, &rest);
		new = union_nodes(old, husb, false, true);
		freeGNodes(old);
		old = new;
		new = union_nodes(old, wife, false, true);
		freeGNodes(old);
		old = new;
		joinFamily(fam, refn, husb, wife, chil, rest);
		family = family->sibling;
	}
	return new;
}

//  addat -- Add @'s to both ends of String.
//    MNOTE: This function uses a cycle of three static buffers to do its work. Callers that
//    need the string to persist must retain a copy.
//--------------------------------------------------------------------------------------------------
String addat (String string)
//  string -- String to have @-signs attached to.
{
	String scratch;
	static char buffer[3][20];
	static int dex = 0;
	if (++dex > 2) dex = 0;
	scratch = buffer[dex];
	sprintf(scratch, "@%s@", string);
	return scratch;
}

//  rmvat -- Remove the @-signs from both ends of a Gedcom key.
//    MNOTE: Uses a cycle of NUMRMVAT static buffers to return the keys. Callers that
//    need the key to persist must make a copy.
//--------------------------------------------------------------------------------------------------
#define NUMRMVAT 50
String rmvat (String string)
//  string -- String that should start and end with an @-sign. This is not checked.
{
	String scratch;
	static char buffer[NUMRMVAT][20];
	static int dex = 0;
	if (++dex > NUMRMVAT - 1) dex = 0;
	scratch = buffer[dex];
	strcpy(scratch, &string[1]);  // Remove the left @-sign.
	scratch[strlen(scratch)-1] = 0;  // Remove the right @-sign.
	return scratch;
}

//  isKey -- Return true is a string has a Gedcom key format (surrounded by @signs).
//--------------------------------------------------------------------------------------------------
bool isKey(String str)
{
	return str && str[0] == '@' && str[strlen(str) - 1] == '@' && strlen(str) >= 3;
}

//  findTag -- Search a list of nodes for the first node with a specific tag. Return that node.
//    If a node with the tag is not found, return null.
//--------------------------------------------------------------------------------------------------
GNode* findTag(GNode* node, String tag)
//  node -- First node in the list of nodes to search.
//  tag -- Tag being searched for.
{
	while (node) {
		if (eqstr(tag, node->tag)) return node;
		node = node->sibling;
	}
	return null;
}

// valueToSex -- Convert SEX value to internal form.
//--------------------------------------------------------------------------------------------------
SexType valueToSex (GNode* node)
//  node -- Node whose tag should be SEX and whose value should be M, F, or U.
{
	if (!node || !node->value) return sexUnknown;
	if (!strcmp("M", node->value)) return sexMale;
	if (!strcmp("F", node->value)) return sexFemale;
	return sexUnknown;
}

// full_value -- Return value of node, with CONT lines. This has never worked according to Gedcom
// specifications.
//--------------------------------------------------------------------------------------------------
String full_value(GNode* node)
//Node node;
{
	GNode* cont;
	int len = 0;
	String p, q, str;
	if (!node) return null;
	if ((p = node->value)) len += strlen(p) + 1;
	cont = node->child;
	while (cont && !strcmp("CONT", cont->tag)) {
		if ((p = cont->value))
			len += strlen(p) + 1;
		else
			len++;
		cont = cont->sibling;
	}
	if (len == 0) return null;
	str = p = stdalloc(len + 1);
	if ((q = node->value)) {
		sprintf(p, "%s\n", q);
		p += strlen(p);
	}
	cont = node->child;
	while (cont && !strcmp("CONT", cont->tag)) {
		if ((q = cont->value))
			sprintf(p, "%s\n", q);
		else
			sprintf(p, "\n");
		p += strlen(p);
		cont = cont->sibling;
	}
	*(p - 1) = 0;
	return str;
}

//  countNodesInTree -- Count the nodes in the tree rooted at node. Siblings of the node are
//    not in the tree so and not counted.
//--------------------------------------------------------------------------------------------------
static int countNodesInTree (GNode *node)
{
	if (!node) return 0;
	return 1 + countNodes(node->child);
}

//  countNodesBefore -- Return the number of nodes that occur before this node in depth first,
//    left-to-rignt order. This is used in generating error messages.
//--------------------------------------------------------------------------------------------------
int countNodesBefore (GNode *node)
//  node -- Node in a node tree.
{
	if (!node) return 0;  // Should not happen.
	int count = 0;

	// Loop over original node and its ancestors...
	while (node->parent) {
		// Get the node's parent and parent's first child.
		GNode *parent = node->parent;
		GNode *child = parent->child;
		// Count nodes in the subtrees of previous siblings.
		while (child && child != node) {
			count += countNodesInTree(child);
			if (count > 100000) {
				printf("Recursing forever?????\n");
				ASSERT(false);
			}
			child = child->sibling;
		}
		// Move up an ancestor and repeat.
		node = parent;
		count++;
	}
	return count;
}
