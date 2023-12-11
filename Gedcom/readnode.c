//
//  DeadEnds
//
//  readnode.c -- Functions that read GNodes (Gedcom nodes) and GNode trees from files
//    and strings.
//
//  Created by Thomas Wetmore on 17 December 2022.
//  Last changed on 10 December 2023.
//

#include "readnode.h"
#include "stringtable.h"
#include "list.h"
#include "errors.h"

static bool debugging = true;

//  Return codes from fileToLine and extractFields.
//-------------------------------------------------------------------------------------------------
typedef enum ReadReturn {
	ReadEOF = 0, // File is at end of file.
	ReadOkay,    // Last line read was syntactically correct.
	ReadError    // Last line read had an error.
} ReadReturn;

//  Error messages used in this file.
//--------------------------------------------------------------------------------------------------
//static String fileof = (String) "The file is positioned at EOF.";
//static String reremp = (String) "Line %d: This line is empty; EOF?";
//static String rerlng = (String) "Line %d: This line is too long.";
//static String rernlv = (String) "Line %d: This line has no level number.";
//static String rerinc = (String) "Line %d: This line is incomplete.";
//static String rerbln = (String) "Line %d: This line has a bad link.";
//static String rernwt = (String) "Line %d: This line needs white space before tag.";
//static String rerilv = (String) "Line %d: This line has an illegal level.";
//static String rerwlv = (String) "The record begins at wrong level.";

//  Static variables that maintain state between functions in this file.
//--------------------------------------------------------------------------------------------------
static String fileName;   // Name of the file being read.
static int fileLine = 0;  // Current line number in the file.
static int level;     // Level of the last line read.
static String key;    // Key, if any, on the last line read.
static String tag;    // Tag on the last line read.
static String value;  // Value, if any, on the last line read.
static bool ateof = false;  //  Whether the Gedcom file has reached end of file.

//  extractFields -- Process a String holding a single Gedcom line by extracting the level, key
//    (if any), tag, and value (if any). The line may have a newline at the end. This function is
//    called by both fileToLine and stringToLine. The key, tag and value fields are stored in
//    the state variables key, tag and value.
//
//    MNOTE: The three variables will all hold pointers into the input string p. Callers must
//    copy those strings before the next line is extracted.
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
	key = p++;  // Points into original string.
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
	tag = p++; // MNOTE: Tag points into the original string.
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;

	// Get the value field.
	while (iswhite(*p)) p++;
	value = p;  // MNOTE: Value points into the original string.
	return ReadOkay;
}

//  fileToLine -- Reads the next Gedcom line from a file. Empty lines are counted and ignored.
//    The line is passed to extractFields for field extraction. An error message is returned if
//    a problem is found. Returns a code of be ReadOkay, ReadEOF, or ReadError, which is found
//    when fileToLine calls extractFields. The function uses fgets to read the lines.
//--------------------------------------------------------------------------------------------------
static ReadReturn fileToLine(FILE *file, Error **error)
//  file -- File to read the line from.
//  level -- (out) Level of the returned line.
//  key -- (out) Key (cross reference) of the returned line; can be null.
//  tag -- (out) Tag of the returned line; manadatory.
//  value -- (out) Value of the returned line; can be null.
//  message -- (out) Error message when things go wrong.
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



// firstNodeTreeFromFile -- Convert the first Gedcom record in a file to a Gedcom node tree.
//--------------------------------------------------------------------------------------------------
GNode* firstNodeTreeFromFile (FILE *fp, String fName, int *lineNo, ErrorLog *errorLog)
//  fp -- (in) File that holds Gedcom records.
//  fName 
//  pmsg -- (out) Possible error message.
//  peof -- (out) Set to true if the file is at end of file.
{
	if (debugging) printf("    FIRST NODE TREE FROM FILE: fName: %s\n", fName);
	ateof = false;
	fileName = fName; // Put the file name in a state variable.
	fileLine = 0;     // Initialize the line number state variable at start of each file.
	Error *error = null;
	ReadReturn rc = fileToLine(fp, &error);
	if (rc == ReadEOF) {
		ateof = true;
		addErrorToLog(errorLog, createError(systemError, fileName, fileLine, "The Gedcom file is empty."));
		if (error) deleteError((Word) error);
		return null;
	} else if (rc == ReadError) {
		addErrorToLog(errorLog, error);
		return null;
	}
	return nextNodeTreeFromFile(fp, lineNo, errorLog);
}

//  nextNodeTreeFromFile -- Convert the next Gedcom record in a file to a Node tree.
//--------------------------------------------------------------------------------------------------
GNode* nextNodeTreeFromFile(FILE *fp, int *lineNo, ErrorLog *errorLog)
//  fp -- File that holds the Gedcom records.
//  pmsg -- (out) Possible error message.
//  peof -- (out) Set to true if the file is at end of file.
{
	if (debugging) printf("      NEXT NODE TREE FROM FILE\n");
	ReadReturn bcode, rc;
	GNode *root, *node, *curnode;
	Error *error;
	// If file is at end return EOF.
	if (ateof) return null;

	//  The first line in the record has been read and must have level 0.
	*lineNo = fileLine;  // Transfer static fileLine of first record line to "outside world" form.
	int curlev = level;
	if (curlev != 0)  {
		addErrorToLog(errorLog, createError(syntaxError, fileName, fileLine, "Record does not start at level 0"));
		//  Read ahead for a line with level 0.
		//while ((rc = fileToLine(fp, &level, &key, &value, &error)))
		return null;
	}

	//  Create the root of a node tree.
	root = curnode = createGNode(key, tag, value, null);
	if (debugging) printf("      NNTFF: Creating root node: %s\n", gnodeToString(root, level));
	bcode = ReadOkay;

	//  Read the lines of the current record and build its tree.
	if (debugging) printf("      NNTFF: before read loop: fileToLine on next line\n");
	rc = fileToLine(fp, &error);
	while (rc == ReadOkay) {

		//  If the level is zero the the record has been read and built.
		if (level == 0) {
			bcode = ReadOkay;
			break;
		}

		//  If the level of this line is the same as the last, add a sibling node.
		if (level == curlev) {
			node = createGNode(key, tag, value, curnode->parent);
			if (debugging) printf("      NNTFF: read node at same level: %s\n", gnodeToString(node, level));
			curnode->sibling = node;
			curnode = node;

		//  If the level of this line is one deeper than the last, add a child node.
		} else if (level == curlev + 1) {
			node = createGNode(key, tag, value, curnode);
			if (debugging) printf("      NNTFF: read node a level deeper; add child: %s\n", gnodeToString(node, level));
			curnode->child = node;
			curnode = node;
			curlev = level;
		//  If the level of this line is less than the last move up the parent chain.
		} else if (level < curlev) {
			if (debugging) printf("      NNTFF: read node at higher level: moving up...\n");
			// Check for an illegal level.
			if (level < 0) {
				addErrorToLog(errorLog, createError(syntaxError, fileName, fileLine, "Illegal level number."));
				bcode = ReadError;
				break;
			}
			//  Move up the parent list until reaching the node with the same level.
			while (level < curlev) {
				curnode = curnode->parent;
				curlev--;
			}
			//  Add the new node as a sibling.
			node = createGNode(key, tag, value, curnode->parent);
			if (debugging) printf("      NNTFF: adding and moving to sibling: %s\n", gnodeToString(node, 0));
			curnode->sibling = node;
			curnode = node;

		//  Anything else is an error.
		} else {
			if (debugging) printf("      NNTFF: illegal line level\n");
			Error *error = createError(syntaxError, fileName, fileLine, "Illegal level number");
			if (debugging) showError(error);
			addErrorToLog(errorLog, error);
			bcode = ReadError;
			break;
		}
		//  The line was converted to a node and inserted. Read the next line and continue.
		if (debugging) printf("      NNTFF: reading next line and looping back up\n");
		rc = fileToLine(fp, &error);
	}

	//  If successful return the tree root.
	if (bcode == ReadOkay) return root;
	if (bcode == ReadError || rc == ReadError) {
		freeGNodes(root);
		return null;
	}
	ateof = true ;
	return root;
}

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

//  Following code is an experiemnet in adding another layer to the read stack in order to make
//    the next higher layer simpler.
//--------------------------------------------------------------------------------------------------

NodeListElement *createNodeListElement(GNode *node, int level, int lineNo, Error *error)
{
	NodeListElement *element = (NodeListElement*) malloc(sizeof(NodeListElement));
	element->node = node;
	element->level = level;
	element->lineNo = lineNo;
	element->error = error;
	return element;
}

typedef List NodeList;

NodeList *createNodeList(void)
{
	NodeList *nodeList = createList(null, null, null);  // TODO: Do need to write one of the functions.
	return nodeList;
}

NodeList *getNodeList(FILE *fp, ErrorLog *errorLog)
{
	NodeList *nodes = createNodeList();
	Error *error;

	ReadReturn rc = fileToLine(fp, &error);
	while (rc != ReadEOF) {
		if (rc == ReadOkay) {
			// Create the GNode from the extracted fields and add it to the list.
			appendListElement(nodes, createNodeListElement(createGNode(key, tag, value, null), level, fileLine, null));
		} else {
			// Add an error entry to the node list.
			appendListElement(nodes, createNodeListElement(null, level, fileLine, error));
		}
		rc = fileToLine(fp, &error);
	}

	if (debugging) {
		printf("Length of the root nodes and errors list is %d\n", lengthList(nodes));
	}
	return nodes;
}

void showNodeList(NodeList *nodeList)
{
	FORLIST(nodeList, element)
		NodeListElement *nodeListElement = (NodeListElement*) element;
		printf("%d ", nodeListElement->lineNo);
		if (nodeListElement->error) {
			printf("Error: "); showError(nodeListElement->error);
		} else if (nodeListElement->node) {
			printf("Node: "); showSingleGNode(nodeListElement->level, nodeListElement->node);
		}
	ENDLIST
}

enum ExStates { InitialState, MainState, ErrorState, DoneState };

#define isZeroLevelNode(element) ((element)->node && (element)->level == 0)
#define isNonZeroLevelNode(element) ((element)->node && (element)->level != 0)
#define isNodeElement(element) ((element)->node)
#define isErrorElement(element) ((element)->error)
#define elementFields(element) element->node->key, element->node->tag, element->node->value, null

//=================================== UPPER HALF; UPPER HALF ======================================
NodeList *upperHalf(NodeList *lowerList, ErrorLog *errorLog)
{
	enum ExStates state = InitialState;
	int prevLevel = 0;
	int curLevel = 0;
	int line0Number;  // Line number of current root node.
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
				appendListElement(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
				state = ErrorState;
				continue;
			}
			// A 0-level node is the first node of the next record. Return current node tree.
			if (isZeroLevelNode(element)) {
				appendListElement(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
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
			appendListElement(rootNodeList, createNodeListElement(curRoot, 0, line0Number, null));
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
	return rootNodeList;
}
