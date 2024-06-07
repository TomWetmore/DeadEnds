// DeadEnds
//
// readnode.c has the functions that read GNodes (Gedcom nodes) and GNode trees from files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 29 May 2024.

#include "readnode.h"
#include "stringtable.h"
#include "gnodelist.h"
#include "list.h"
#include "errors.h"

extern bool importDebugging;
extern FILE* debugFile;

// Variables that maintain state between functions.
String xfileName;
int xfileLine = 0;
int xlevel;
String xkey;
String xtag;
String xvalue;
static bool ateof = false;

// extractFields processes a Gedcom line into static variables.
static ReadReturn extractFields(String p, Error** error) {
	if (!p || *p == 0) {
		*error = createError(syntaxError, xfileName, xfileLine, "Empty string");
		return ReadError;
	}
	xkey = xvalue = null;
	striptrail(p);
	if (strlen(p) > MAXLINELEN) {
		*error = createError(syntaxError, xfileName, xfileLine, "Gedcom line is too long.");
		return ReadError;
	}
	while (iswhite(*p)) p++; // Level.
	if (chartype(*p) != DIGIT) {
		*error = createError(syntaxError, xfileName, xfileLine, "Line does not begin with a level");
		return ReadError;
	}
	xlevel = *p++ - '0';
	while (chartype(*p) == DIGIT) xlevel = xlevel*10 + *p++ - '0';
	while (iswhite(*p)) p++; // Before key or tag.
	if (*p == 0) {
		*error = createError(syntaxError, xfileName, xfileLine, "Gedcom line is incomplete.");
		return ReadError;
	}
	if (*p == '@') { // Key.
		xkey = p++;
		if (*p == '@') { // @@ illegal.
			*error = createError(syntaxError, xfileName, xfileLine, "Illegal key (@@)");
			return ReadError;
		}
		while (*p != '@' && *p != 0) p++; // Read to 2nd @-sign.
		if (*p == 0) {
			*error = createError(syntaxError, xfileName, xfileLine, "Gedcom line is incomplete.");
			return ReadError;
		}
		if (*++p != ' ') {
			*error = createError(syntaxError, xfileName, xfileLine, "There must be space between the key and tag.");
			return ReadError;
		}
		*p++ = 0;
	}
	while (iswhite(*p)) p++; // Tag.
	if ((int) *p == 0) {
		*error = createError(syntaxError, xfileName, xfileLine, "The line is incomplete");
		return ReadError;
	}
	xtag = p++;
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;
	while (iswhite(*p)) p++; // Value.
	xvalue = p;
	return ReadOkay;
}

// fileToLine reads the next Gedcom line from a file. Empty lines okay.
ReadReturn fileToLine(FILE* file, Error** error) {
	static char buffer[MAXLINELEN];
	char *p = buffer;
	*error = null;
	while (true) {
		if (!(p = fgets(buffer, MAXLINELEN, file))) { // Read line.
			ateof = true;
			return ReadAtEnd;
		}
		xfileLine++;
		if (!allwhite(p)) break;
	}
	return extractFields(p, error);
}

// stringToLine gets the next Gedcom line as fields from a String with one or more Gedcom lines.
// Reads to the next newline, if any, and processes that part of the String. Returns updated
// pointer to next line.
static ReadReturn stringToLine(String* ps, Error** error) {
	String s0 = *ps;
	if (!s0 || *s0 == 0) return ReadAtEnd;
	String s = s0;
	while (*s && *s != '\n') s++;
	if (*s == 0)
		*ps = s;
	else {
		*s = 0;
		*ps = s + 1;
	}
	return extractFields(s0, error);
}

// stringToGNodeTree converts a String holding a Gedcom record into a GNode tree.
GNode* stringToGNodeTree(String str, ErrorLog *errorLog) {
	xfileLine = 0;
	Error* error;
	if (!stringToLine(&str, &error)) {
		addErrorToLog(errorLog, error);
		return null;
	}
	if (xlevel != 0) {
		error = createError(syntaxError, xfileName, 0, "First line in string not level 0");
		addErrorToLog(errorLog, error);
		return null;
	}
	int curlev = 0;
	GNode* root = createGNode(xkey, xtag, xvalue, null);
	GNode* curnode = root;
	while (stringToLine(&str, &error)) {
		GNode* node = null;
		if (xlevel == curlev) {
			node = createGNode(xkey, xtag, xvalue, curnode->parent);
			curnode->sibling = node;
			curnode = node;
		} else if (xlevel == curlev + 1) {
			node = createGNode(xkey, xtag, xvalue, curnode);
			curnode->child = node;
			curnode = node;
			curlev = xlevel;
		} else if (xlevel < curlev) {
			if (xlevel < 0) {
				printf("Error: line %d: illegal level", xfileLine);
				return null;
			}
			while (xlevel < curlev) {
				curnode = curnode->parent;
				curlev--;
			}
			node = createGNode(xkey, xtag, xvalue, curnode->parent);
			curnode->sibling = node;
			curnode = node;
		} else {
			printf("Error: line %d: illegal level", xfileLine);
			return null;
		}
	}
	return root;
}
