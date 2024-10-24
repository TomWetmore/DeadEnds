//  DeadEnds Project
//
//  gnode.c has many functions for the GNode data type.
//
//  Created by Thomas Wetmore on 12 November 2022.
//  Last changed on 22 October 2024.

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

// tagTable is the StringTable that holds a single copy of all tags used in the GNodes.
static StringTable *tagTable = null;

// numNodeAllocs returns the number of GNodes that have been allocatedp. Debugging.
static int nodeAllocs = 0;
int numNodeAllocs(void) {
	return nodeAllocs;
}

// numNodeFrees returns the number of GNodes that have been freed. Debugging.
static int nodeFrees = 0;
int numNodeFrees(void) {
	return nodeFrees;
}

// getFromTagTable returns the persistent tag value from the tag table.
static int numBucketsInTagTable = 67;
static String getFromTagTable(String tag) {
	if (tagTable) return fixString(tagTable, tag);
	tagTable = createStringTable(numBucketsInTagTable);
	return fixString(tagTable, tag);
}

// freeGNode frees a GNode.
void freeGNode(GNode* node) {
	if (node->key) stdfree(node->key);
	if (node->value) stdfree(node->value);
	nodeFrees++;
	stdfree(node);
}

// createGNode creates a GNode from a key, tag, value, and pointer to parent. When a GNode is
// created the key and value, if there, are allocated in the heap, and the tag pointer is taken
// from the tag table. This is the only time that memory for these fields is handled.
GNode* createGNode(String key, String tag, String value, GNode* parent) {
	nodeAllocs++;
	GNode* node = (GNode*) malloc(sizeof(GNode));;
	node->key = strsave(key);
	node->tag = getFromTagTable(tag);
	node->value = strsave(value);
	node->parent = parent;
	node->child = null;
	node->sibling = null;
	return node;
}

// freeGNodes frees all GNodes in a tree or forest of GNodes.
void freeGNodes(GNode* node) {
	while (node) {
		if (node->child) freeGNodes(node->child);
		GNode* sibling = node->sibling;
		freeGNode(node);
		node = sibling;
	}
}

// gnodeLevel returns the level of a GNode by counting parent links.
int gnodeLevel(GNode* node) {
	int level = 0;
	while (node->parent && level < 100) {
		level++;
		node = node->parent;
	}
	if (level >= 100) return 0; // Infinite loop protection.
	return level;
}

// personToEvent converta an event tree to a string; returns static memory.
String personToEvent(GNode* person, String tag, String head, int len, bool shorten) {
	static char scratch[200];
	String event;
	size_t n;
	if (!person) return null;
	if (!(person = findTag(person->child, tag))) return null;
	event = eventToString(person, shorten);
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

// eventToString converts an event to a string; returns static memory.
String eventToString(GNode* node, bool shorten) {
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
		date = shortenDate(date);
		plac = shortenPlace(plac);
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

// eventToDate returns the date of an event as a string.
String eventToDate(GNode* node, bool shorten) {
	if (!node) return null;
	if (!(node = DATE(node))) return null;
	if (shorten) return shortenDate(node->value);
	return node->value;
}

// eventToPlace returns the place of an event as a string.
String eventToPlace (GNode* node, bool shorten) {
	if (!node) return null;
	node = PLAC(node);
	if (!node) return null;
	if (shorten) return shortenPlace(node->value);
	return node->value;
}

// showGNodeTree shows a tree of GNodes; for debugging.
void showGNodeTree(GNode* node) {
	if (!node) return;
	showGNodes(0, node);
}

// showGNodes shows a GNode tree recursively.
void showGNodes(int levl, GNode* node) {
	if (!node) return;
	for (int i = 1;  i < levl;  i++) printf("  ");
	printf("%d", levl);
	if (node->key) printf(" %s", node->key);
	printf(" %s", node->tag);
	if (node->value) printf(" %s", node->value);
	printf("\n");
	showGNodes(levl + 1, node->child);
	showGNodes(levl    , node->sibling);
}

// showGNode returns the string (Gedcom line) form of a GNode.
void showGNode(int level, GNode* node) {
	printf("%d", level);
	if (node->key) printf(" %s", node->key);
	printf(" %s", node->tag);
	if (node->value) printf(" %s", node->value);
	printf("\n");
}

// gNodesLength returns the length of a list of GNodes.
int gNodesLength(GNode* node) {
	int len = 0;
	while (node) {
		len++;
		node = node->sibling;
	}
	return len;
}

// shortenDate returns the short form of a date value.
String shortenDate(String date) {
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

// shortenPlace returns the short form of a place value.
String shortenPlace(String place) {
	String plac0 = place, comma;
	if (!place) return null;
	comma = (String) strrchr(place, ',');
	if (comma) place = comma + 1;
	while (*place++ == ' ')
		;
	place--;
	if (*place == 0) return plac0;
	//if ((val = (String) valueof(placabbvs, plac))) return val;
	return place;
}

// copyNode copies a GNode.
GNode* copyNode(GNode* node) {
	return createGNode(node->key, node->tag, node->value, null);
}

// copyNodes copies a GNode tree. If kids or sibs copy children or siblings respectively.
GNode* copyNodes(GNode* node, bool kids, bool sibs) {
	if (!node) return null;
	GNode* kin;
	GNode* new = copyNode(node);
	if (kids && node->child) {
		kin = copyNodes(node->child, true, true);
		new->child = kin;
		while (kin) {
			kin->parent = new;
			kin = kin->sibling;
		}
	}
	if (sibs && node->sibling) {
		kin = copyNodes(node->sibling, kids, true);
		new->sibling = kin;
	}
	return new;
}

// traverseNodes traverse the GNodes of a Gedcom record calling a function on each.
void traverseNodes(GNode* node, int level, bool (*func)(GNode*, int)) {
	if (func == null) return;
	for (; node; node = node->sibling) {
		if (!(*func)(node, level)) continue;
		if (node->child) traverseNodes(node->child, level+1, func);
	}
}

// countNodes return the number of GNodes in a GNode tree or forest.
int countNodes(GNode* node) {
	if (node == null) return 0;
	int count = 1 + countNodes(node->child) + countNodes(node->sibling);
	if (count > 40000) {
		printf("Recursing forever?????\n");
		exit(3);
	}
	return count;
}

// findNode finds the first child node with specific tag and/or value. One or both of tag and
// value must be non-null. plast can be used to point to GNode before returned GNode.
GNode* findNode(GNode* parent, String tag, String value, GNode** plast) {
	if (!parent || (!tag && !value)) return null;
	if (plast) *plast = null;
	for (GNode *last = null, *node = parent->child; node; last = node, node = node->sibling) {
		if (tag && nestr(tag, node->tag)) continue;
		if (value && nestr(value, node->value)) continue;
		if (plast) *plast = last;
		return node;
	}
	return null;
}

// recordKey returns the key of the record the node is from.
String recordKey(GNode* node) {
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

// isKey returns true if a string has a Gedcom key format.
bool isKey(String str) {
	return str && str[0] == '@' && str[strlen(str) - 1] == '@' && strlen(str) >= 3;
}

// findTag searches a list of nodes and returns the first with a specific tag.
GNode* findTag(GNode* node, String tag) {
	while (node) {
		if (eqstr(tag, node->tag)) return node;
		node = node->sibling;
	}
	return null;
}

// valueToSex converts a 1 SEX GNode value to enumerated form.
SexType valueToSex (GNode* node) {
	if (!node || !node->value) return sexUnknown;
	if (eqstr("M", node->value)) return sexMale;
	if (eqstr("F", node->value)) return sexFemale;
	return sexUnknown;
}

// full_value -- Return value of node, with CONT lines. This has never worked according to Gedcom
// specifications.
String full_value(GNode* node) {
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

// countNodesInTree counts the GNodes in a single GNode tree. Siblings of node are not counted.
static int countNodesInTree(GNode *node) {
	if (!node) return 0;
	return 1 + countNodes(node->child);
}

// countNodesBefore returns the number of GNodes that occur before this one in depth first,
// left-to-rignt order; used in generating error messages. Sweet algorithm.
int countNodesBefore(GNode* node) {
	if (!node) return 0;
	int count = 0;
	while (node->parent) { // Interate node and ancestors.
		GNode *parent = node->parent;
		GNode *child = parent->child;
		while (child && child != node) { // Count trees of previous sibs.
			count += countNodesInTree(child);
			if (count > 100000) {
				printf("Recursing forever?????\n");
				ASSERT(false);
			}
			child = child->sibling;
		}
		node = parent; // Move up and repeat.
		count++;
	}
	return count;
}
