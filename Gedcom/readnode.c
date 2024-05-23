// DeadEnds
//
// readnode.c has the functions that read GNodes (Gedcom nodes) and GNode trees from files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 21 May 2024.

#include "readnode.h"
#include "stringtable.h"
#include "list.h"
#include "errors.h"

extern bool importDebugging;
extern FILE* debugFile;

// Static variables that maintain state between functions.
static String fileName;    // Name of file.
static int fileLine = 0;   // Current line number.
static int level;          // Level of last line read.
static String key;         // Key of last line read.
static String tag;         // Tag of last line read.
static String value;       // Value of last line read.
static bool ateof = false; // File is at end of file.

// extractFields processes a String with a single Gedcom line into static field variables. Called
// by both fileToLine and stringToLine.
static ReadReturn originalExtractFields(String p, Error** error) {
	if (!p || *p == 0) { // String exists?
		*error = createError(syntaxError, fileName, fileLine, "Empty string");
		return ReadError;
	}
	key = value = null;
	striptrail(p);
	if (strlen(p) > MAXLINELEN) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is too long.");
		return ReadError;
	}
	while (iswhite(*p)) p++; // Whitespace before level.
	if (chartype(*p) != DIGIT) { // Level.
		*error = createError(syntaxError, fileName, fileLine, "Line does not begin with a level");
		return ReadError;
	}
	level = *p++ - '0';
	while (chartype(*p) == DIGIT) level = level*10 + *p++ - '0';
	while (iswhite(*p)) p++; // Whitespace before key or tag.
	if (*p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is incomplete.");
		return ReadError;
	}
	// If @ is the next character, this line has a key.
	if (*p != '@') goto gettag;

	// Get the key including @'s.
	key = p++;
	if (*p == '@') {  // @@ is illegal.
		*error = createError(syntaxError, fileName, fileLine, "Illegal key (@@)");
		return ReadError;
	}
	while (*p != '@' && *p != 0) p++;  // Read until the second @-sign.
	//  If at the end of the string it is an error.
	if (*p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is incomplete.");
		return ReadError;
	}
	//  p points to the second @-sign. Put a space into the next character (which will be
	//    between the 2nd @ and the first character of the tag).
	if (*++p != ' ') {
		*error = createError(syntaxError, fileName, fileLine, "There must be space between the key and tag.");
		return ReadError;
	}
	*p++ = 0;

gettag:
	while (iswhite(*p)) p++; // Whitespace before tag.
	if ((int) *p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "The line is incomplete");
		return ReadError;
	}
	tag = p++;
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;
	while (iswhite(*p)) p++; // Value.
	value = p;
	return ReadOkay;
}



// Modified from originalExtractFields by CHATGPT.
static ReadReturn extractFields(String p, Error** error) {
	if (!p || *p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Empty string");
		return ReadError;
	}
	key = value = null;
	striptrail(p);
	if (strlen(p) > MAXLINELEN) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is too long.");
		return ReadError;
	}
	while (iswhite(*p)) p++; // Level.
	if (chartype(*p) != DIGIT) {
		*error = createError(syntaxError, fileName, fileLine, "Line does not begin with a level");
		return ReadError;
	}
	level = *p++ - '0';
	while (chartype(*p) == DIGIT) level = level*10 + *p++ - '0';
	while (iswhite(*p)) p++; // Before key or tag.
	if (*p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is incomplete.");
		return ReadError;
	}
	if (*p == '@') { // Key.
		key = p++;
		if (*p == '@') { // @@ illegal.
			*error = createError(syntaxError, fileName, fileLine, "Illegal key (@@)");
			return ReadError;
		}
		while (*p != '@' && *p != 0) p++; // Read to 2nd @-sign.
		if (*p == 0) {
			*error = createError(syntaxError, fileName, fileLine, "Gedcom line is incomplete.");
			return ReadError;
		}
		if (*++p != ' ') {
			*error = createError(syntaxError, fileName, fileLine, "There must be space between the key and tag.");
			return ReadError;
		}
		*p++ = 0;
	}
	while (iswhite(*p)) p++; // Tag.
	if ((int) *p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "The line is incomplete");
		return ReadError;
	}
	tag = p++;
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;
	while (iswhite(*p)) p++; // Value.
	value = p;
	return ReadOkay;
}

// fileToLine reads the next Gedcom line from a file. Empty lines okay.
static ReadReturn fileToLine(FILE* file, Error** error) {
	static char buffer[MAXLINELEN];
	char *p = buffer;
	*error = null;
	while (true) {
		if (!(p = fgets(buffer, MAXLINELEN, file))) { // Read line.
			ateof = true;
			return ReadEOF;
		}
		fileLine++;
		if (!allwhite(p)) break;
	}
	return extractFields(p, error);
}


//  stringToLine -- Get the next Gedcom line as fields from a string holding one or more Gedcom
//    lines. This function reads to the next newline, if any, and processes that part of the
//    string. If there are remaining characters the address of the next character is returned
//    in ps
//--------------------------------------------------------------------------------------------------
/*static ReadReturn stringToLine(String *ps, int *plevel, String *pkey, String *ptag, String *pvalue,
						 Error **error)
//  ps -- (in/out) Pointer to string.
//  plevel -- (out) Pointer to level.
//  pxref -- (out) Pointer to cross reference string of this line.
//  ptag -- (out) Pointer to tag of this line.
//  pval -- (out) Pointer to value of this line.
//  pmsg -- (out) Pointer to error message if anything goes wrong.
{
	String s0, s;
	*error = null;
	s0 = s = *ps;
	if (!s || *s == 0) return false;
	while (*s && *s != '\n') s++;
	if (*s == 0)
		*ps = s;
	else {
		*s = 0;
		*ps = s + 1;
	}
	return extractFields(s0, plevel, pkey, ptag, pvalue, error) == ReadOkay;
}*/

//  stringToNodeTree -- Convert a string with a single Gedcom record into a node tree. Was used
//    by LifeLines when reading records from its database to convert them to node tree format.
//    So far not needed by DeadEnds because the records are never in a string format.
//--------------------------------------------------------------------------------------------------
/* GNode* stringToNodeTree(String str, ErrorLog *errorLog)
{
	int lev;
	int lev0;
	String xref;
	String tag;
	String val;

	int curlev;
	GNode *root, *node, *curnode;
	String msg;
	fileLine = 0;
	if (!stringToLine(&str, &lev, &xref, &tag, &val, &msg)) return null;
	lev0 = curlev = lev;
	root = curnode = createGNode(xref, tag, val, null);
	while (stringToLine(&str, &lev, &xref, &tag, &val, &msg)) {
		if (lev == curlev) {
			node = createGNode(xref, tag, val, curnode->parent);
			curnode->sibling = node;
			curnode = node;
		} else if (lev == curlev + 1) {
			node = createGNode(xref, tag, val, curnode);
			curnode->child = node;
			curnode = node;
			curlev = lev;
		} else if (lev < curlev) {
			if (lev < lev0) {
				printf("Error: line %d: illegal level", fileLine);
				return null;
			}
			while (lev < curlev) {
				curnode = curnode->parent;
				curlev--;
			}
			node = createGNode(xref, tag, val, curnode->parent);
			curnode->sibling = node;
			curnode = node;
		} else {
			printf("Error: line %d: illegal level", fileLine);
			return null;
		}
	}
	if (!msg) return root;
	freeGNodes(root);
	return null;
} */

// createNodeListElement creates an element for a NodeList. Node or error must be null.
NodeListElement* createNodeListElement(GNode* node, int level, int lineNo, Error* error) {
	ASSERT(node || error && (!node || !error));
	NodeListElement *element = (NodeListElement*) malloc(sizeof(NodeListElement));
	element->node = node;
	element->level = level;
	element->lineNo = lineNo;
	element->error = error;
	return element;
}

// getKey is the getKey function for NodeLists.
static String getKey(void* element) {
	return ((NodeListElement*) element)->node->tag;
}

// createNodeList creates a NodeList; one type holds all GNodes from a file, the other holds all
// roots.
NodeList* createNodeList(void) {
	NodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// getNodeListFromFile uses fileToLine and extractFields to create a GNode for each line in a
// Gedcom file. Lines with errors store Errors in the list rather than GNodes.
NodeList *getNodeListFromFile(FILE* fp, int* numErrors) {
	NodeList* nodeList = createNodeList();
	Error* error;

	*numErrors = 0;
	ReadReturn rc = fileToLine(fp, &error);
	while (rc != ReadEOF) {
		if (rc == ReadOkay) {
			appendToList(nodeList, createNodeListElement(
							createGNode(key, tag, value, null), level, fileLine, null));
		} else {
			appendToList(nodeList, createNodeListElement(null, level, fileLine, error));
			(*numErrors)++;
		}
		rc = fileToLine(fp, &error);
	}
	if (importDebugging) {
		fprintf(debugFile, "Length of the node and error list is %d\n", lengthList(nodeList));
		fprintf(debugFile, "Number of errors is %d\n", *numErrors);
	}
	if (lengthList(nodeList) > 0) return nodeList;
	deleteList(nodeList);
	return null;
}

// showNodeList shows the contents of a NodeList. Debugging.
void showNodeList(NodeList* nodeList) {
	FORLIST(nodeList, element)
		NodeListElement *nodeListElement = (NodeListElement*) element;
		printf("%d ", nodeListElement->lineNo);
		if (nodeListElement->error) {
			printf("Error: "); showError(nodeListElement->error);
		} else if (nodeListElement->node) {
			printf("Node: "); showGNode(nodeListElement->level, nodeListElement->node);
		}
	ENDLIST
}

#define isZeroLevelNode(element) ((element)->node && (element)->level == 0)
#define isNonZeroLevelNode(element) ((element)->node && (element)->level != 0)
#define isNodeElement(element) ((element)->node)
#define isErrorElement(element) ((element)->error)
#define elementFields(element) element->node->key, element->node->tag, element->node->value, null

// getNodeTreesFromNodeList scans a NodeList of GNodes and creates a NodeList of GNode trees;
// uses a three state state machine to track levels and errors.
NodeList *getNodeTreesFromNodeList(NodeList *lowerList, ErrorLog *errorLog) {
	enum ExState { InitialState, MainState, ErrorState };
	enum ExState state = InitialState;
	int prevLevel = 0;
	int curLevel = 0;
	int line0Number = 0;  // Line number of current root node.
	GNode *curRoot = null;
	GNode *curNode = null;
	NodeList *rootNodeList = createNodeList();

	for (int index = 0; index < lengthList(lowerList); index++) {
		NodeListElement *element = getListElement(lowerList, index);
		if (isNodeElement(element)) {
			prevLevel = curLevel;
			curLevel = element->level;
		}
		switch (state) {
		case InitialState:
			if (isZeroLevelNode(element)) {
				curRoot = curNode = createGNode(elementFields(element));
				line0Number = element->lineNo;
				state = MainState;
				continue;
			}
			if (isNonZeroLevelNode(element)) {
				addErrorToLog(errorLog, createError(syntaxError, fileName, element->lineNo, "Illegal line level."));
				state = ErrorState;
				continue;
			}
			if (isErrorElement(element)) {
				addErrorToLog(errorLog, element->error);
				state = ErrorState;
				continue;
			}
			break;
		case MainState:
			// On error enter ErrorState but return the possibly partial current node tree.
			if (isErrorElement(element)) {
				addErrorToLog(errorLog, element->error);
				appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
				state = ErrorState;
				continue;
			}
			// A 0-level node is the first node of the next record. Return current node tree.
			if (isZeroLevelNode(element)) {
				appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
				curRoot = curNode = createGNode(elementFields(element));
				prevLevel = curLevel = 0;
				line0Number = element->lineNo;
				continue;
			}
			// Found sibling node. Add it to the tree and make it current.
			if (curLevel == prevLevel) {
				GNode *newNode = createGNode(elementFields(element));
				newNode->parent = curNode->parent;
				curNode->sibling = newNode;
				curNode = newNode;
				continue;
			}
			// Found child node. Add it to the tree and make it current.
			if (curLevel == prevLevel + 1) {
				GNode *newNode = createGNode(elementFields(element));
				newNode->parent = curNode;
				curNode->child = newNode;
				curNode = newNode;
				continue;
			}
			// Found higher level node. Make it a sibling to higher node and make it current.
			if (curLevel < prevLevel) {
				while (curLevel < prevLevel) {
					curNode = curNode->parent;
					prevLevel--;
				}
				GNode *newNode = createGNode(elementFields(element));
				curNode->sibling = newNode;
				newNode->parent = curNode->parent;
				curNode = newNode;
				continue;
			}
			// Anything else is an error.
			addErrorToLog(errorLog, createError(syntaxError, fileName, element->lineNo, "Illegal level number."));
			appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
			state = ErrorState;
			continue;
		case ErrorState:
			if (isErrorElement(element)) continue;
			if (isNonZeroLevelNode(element)) continue;
			line0Number = element->lineNo;
			curRoot = curNode = createGNode(elementFields(element));
			prevLevel = curLevel = 0;
			state = MainState;
			continue;
		default:
			ASSERT(false);
		}
	}
	//  If in MainState at the end there is a a tree to add to the list.
	if (state == MainState) {
		appendToList(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
	}
	return rootNodeList;
}

// numberNodesInNodeList returns the number of GNodes or GNode trees in a NodeList.
int numberNodesInNodeList(NodeList* list) {
	int numNodes = 0;
	FORLIST(list, element)
		NodeListElement *e = (NodeListElement*) element;
		if (e->node) numNodes++;
	ENDLIST
	return numNodes;
}

// numberErrorsInNodeList returns the number of Errors in a NodeList.
int numberErrorsInNodeList(NodeList* list) {
	int numErrors = 0;
	FORLIST(list, element)
		NodeListElement *e = (NodeListElement*) element;
		if (e->error) numErrors++;
	ENDLIST
	return numErrors;
}
