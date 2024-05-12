//
// DeadEnds
//
// readnode.c has the functions that read GNodes (Gedcom nodes) and GNode trees from files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 25 April 2024.
//

#include "readnode.h"
#include "stringtable.h"
#include "list.h"
#include "errors.h"

//static bool debugging = true;
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

//  extractFields -- Process a String holding a single Gedcom line by extracting the level, key
//    (if any), tag, and value (if any). The line may have a newline at the end. This function is
//    called by both fileToLine and stringToLine. The key, tag and value fields are stored in
//    the static state variables key, tag and value.
//
//    MNOTE: key, tag, and value are pointers into the input string. Callers must use or copy
//    them before the next line is extracted.
//--------------------------------------------------------------------------------------------------
static ReadReturn extractFields (String p, Error **error)
//  p -- Gedcom line before processing. Within the function p is used as a cursor.
//  error -- (out) Error, if any.
{
	// Be sure a string was passed in.
	if (!p || *p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Empty string");
		return ReadError;
	}
	//if (debugging) printf("          EXTRACT FIELDS: %s", p);  // No \n because the buffer has it.
	// Initialize the output parameters.
	key = value = null;  // Shifting over to using the static variables.
	// Strip trailing white space from the String. TODO: THIS SEEMS WASTEFUL.
	striptrail(p);

	// See if the input string is too long.
	if (strlen(p) > MAXLINELEN) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is too long.");
		return ReadError;
	}

	// Get the level number. Pass any whitespace that precedes the level.
	while (iswhite(*p)) p++;

	// The first non-white character must be a digit for the Gedcom's line level.
	if (chartype(*p) != DIGIT) {
		*error = createError(syntaxError, fileName, fileLine, "Line does not begin with a level");
		return ReadError;
	}

	// Use ascii arithmetic to convert the digit characters to integers.
	level = *p++ - '0';
	while (chartype(*p) == DIGIT) level = level*10 + *p++ - '0';

	// Pass any white space that precedes the key or tag.
	while (iswhite(*p)) p++;

	// If at the end of the string it is an error.
	if (*p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "Gedcom line is incomplete.");
		return ReadError;
	}

	// If @ is the next character, this line has a key.
	if (*p != '@') goto gettag;

	// Get the key including @'s.
	key = p++;  // MNOTE: key points into the input string.
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

	// Get the tag field.
gettag:
	while (iswhite(*p)) p++;  // Allow additional white space before the tag (non-standard Gedcom).
	if ((int) *p == 0) {
		*error = createError(syntaxError, fileName, fileLine, "The line is incomplete");
		return ReadError;
	}
	tag = p++; // MNOTE: tag points into the input string.
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;

	// Get the value field.
	while (iswhite(*p)) p++;
	value = p;  // MNOTE: value points into the input string.
	return ReadOkay;
}

//  fileToLine -- Reads the next Gedcom line from a file. Empty lines are counted and ignored.
//    The line is passed to extractFields for field extraction. An error message is returned if
//    a problem is found. Returns a value of ReadOkay, ReadEOF, or ReadError, which is found
//    when fileToLine calls extractFields. The function uses fgets to read the lines.
//--------------------------------------------------------------------------------------------------
static ReadReturn fileToLine(FILE *file, Error **error)
//  file -- File to read the line from.
//  weeie -- (out) Error, if any.
{
	static char buffer[MAXLINELEN];  // Buffer to store the line.
	char *p = buffer;  // Buffer cursor.
	*error = null;
	while (true) {
		//  Read a line from the file; if fgets returns 0 assume reading is over.
		if (!(p = fgets(buffer, MAXLINELEN, file))) {
			ateof = true;
			return ReadEOF;
		}
		//if (debugging) printf("        FILE TO LINE: %s", buffer);
		fileLine++;  // Increment the file line number.
		if (!allwhite(p)) break; // If the line is all white continue to the next line.
	}

	// Read a line and convert it to field values. The values point to locations in the buffer.
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

//  createNodeListElement -- Create an element for a NodeList. Either node or error must be null
//    but not both.
//--------------------------------------------------------------------------------------------------
NodeListElement *createNodeListElement(GNode *node, int level, int lineNo, Error *error)
{
	ASSERT(node || error && (!node || !error));  // Ugly exclusive or.
	NodeListElement *element = (NodeListElement*) malloc(sizeof(NodeListElement));
	element->node = node;
	element->level = level;
	element->lineNo = lineNo;
	element->error = error;
	return element;
}

// getKey is the getKey function or NodeLists that returns the tag of the GNode in the element.
static String getKey(void* element) {
	return ((NodeListElement*) element)->node->tag;
}

// createNodeList creates a NodeList; one type holds all GNodes from a file, the other holds the
// list of root GNodes.
NodeList *createNodeList(void) {
	NodeList *nodeList = createList(getKey, null, null, false);
	return nodeList;
}

// getNodeListFromFile uses fileToLine and extractFields to create a GNode for each line in a
// Gedcom file. Lines with errors store Errors in the list rather than GNodes.
NodeList *getNodeListFromFile(FILE *fp, int *numErrors) {
	NodeList *nodeList = createNodeList();
	Error *error;

	*numErrors = 0;
	ReadReturn rc = fileToLine(fp, &error);
	while (rc != ReadEOF) {
		if (rc == ReadOkay) {
			// Create the GNode from the extracted fields and add it to the list.
			appendToList(nodeList, createNodeListElement(
							createGNode(key, tag, value, null), level, fileLine, null));
		} else {
			// Add an error entry to the node list.
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

//  showNodeList -- Show contents of a NodeList. For debugging.
//-------------------------------------------------------------------------------------------------
void showNodeList(NodeList *nodeList)
{
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
//  uses a three state state machine to track levels and errors.
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

//  numberNodesInNodeList -- Return the number of GNodes or GNode trees in a NodeList.
//-------------------------------------------------------------------------------------------------
int numberNodesInNodeList(NodeList *list)
{
	int numNodes = 0;
	FORLIST(list, element)
		NodeListElement *e = (NodeListElement*) element;
		if (e->node) numNodes++;
	ENDLIST
	return numNodes;
}

//  numberErrorsInNodeList -- Return the number of Errors in a NodeList.
//-------------------------------------------------------------------------------------------------
int numberErrorsInNodeList(NodeList *list)
{
	int numErrors = 0;
	FORLIST(list, element)
		NodeListElement *e = (NodeListElement*) element;
		if (e->error) numErrors++;
	ENDLIST
	return numErrors;
}
