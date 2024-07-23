// DeadEnds
//
// readnode.c has the functions that read GNodes (Gedcom nodes) and GNode trees from files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 2 July 2024.

#include "readnode.h"
#include "stringtable.h"
#include "gnodelist.h"
#include "list.h"
#include "errors.h"
#include "file.h"

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

// extractFields processes a String with a Gedcom line into its fields. Important: pkey, ptag and
// pvalue point into the original String.
static ReadReturn extractFields(String p, int* plevel, String* pkey, String *ptag,
						String* pvalue, String* errorString) {
	*pvalue = "";
	*pkey = "";
	if (!p || *p == 0) {
		*errorString = "Empty string";
		return ReadError;
	}
	striptrail(p);
	if (strlen(p) > MAXLINELEN) {
		*errorString = "Gedcom line is too long";
		return ReadError;
	}
	while (iswhite(*p)) p++; // Level.
	if (chartype(*p) != DIGIT) {
		*errorString = "Line does not beging with a level";
		return ReadError;
	}

	int level = *p++ - '0';
	while (chartype(*p) == DIGIT) level = level*10 + *p++ - '0';
	*plevel = level;
	while (iswhite(*p)) p++; // Before key or tag.
	if (*p == 0) {
		*errorString = "Gedcom line is incomplete";
		return ReadError;
	}
	if (*p == '@') { // Key.
		String key = p++;
		if (*p == '@') { // @@ illegal.
			*errorString = "Illegal key (@@)";
			return ReadError;
		}
		while (*p != '@' && *p != 0) p++; // Read to 2nd @-sign.
		if (*p == 0) {
			*errorString = "Gedcom line is incomplete.";
			return ReadError;
		}
		if (*++p != ' ') {
			*errorString = "There must be a space between the key and tag";
			return ReadError;
		}
		*p++ = 0;
		*pkey = key;
	}
	while (iswhite(*p)) p++; // Tag.
	if ((int) *p == 0) {
		*errorString = "The line is incomplete";
		return ReadError;
	}
	*ptag = p++;
	while (!iswhite(*p) && *p != 0) p++;
	if (*p == 0) return ReadOkay;
	*p++ = 0;
	while (iswhite(*p)) p++; // Value.
	*pvalue = p;
	return ReadOkay;
}

// fileToLine reads the next Gedcom line from a file. Empty lines are okay.
ReadReturn fileToLine(FILE* fp, int* pline, int* plevel, String* pkey, String* ptag,
					  String* pvalue, String* err) {
	static char buffer[MAXLINELEN];
	char *p = buffer;
	*err = null;
	while (true) {
		if (!(p = fgets(buffer, MAXLINELEN, fp))) { // Read line.
			ateof = true;
			return ReadAtEnd;
		}
		(*pline)++;
		if (!allwhite(p)) break;
	}
	return extractFields(p, plevel, pkey, ptag, pvalue, err);
}

// stringToLine gets the next Gedcom line as fields from a String with one or more Gedcom lines.
// Reads to the next newline, if any, and processes that part of the String. Returns updated
// pointer to next line.
ReadReturn stringToLine(String* ps, int* line, int* level, String* key, String* tag,
							   String *value, String* err) {
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
	return extractFields(s0, level, key, tag, value, err);
}

// stringToGNodeTree converts a String holding a Gedcom record into a GNode tree.
//GNode* stringToGNodeTree(String str, ErrorLog *errorLog) {
//	xfileLine = 0;
//	String error;
//	int line = 0;
//	int level;
//	String key;
//	String tag;
//	String value;
//	if (!stringToLine(&str, &line, &level, &key, &tag, &value, &error)) {
//		addErrorToLog(errorLog, error);
//		return null;
//	}
//	if (xlevel != 0) {
//		error = createError(syntaxError, xfileName, 0, "First line in string not level 0");
//		addErrorToLog(errorLog, error);
//		return null;
//	}
//	int curlev = 0;
//	GNode* root = createGNode(xkey, xtag, xvalue, null);
//	GNode* curnode = root;
//	while (stringToLine(&str, &error)) {
//		GNode* node = null;
//		if (xlevel == curlev) {
//			node = createGNode(xkey, xtag, xvalue, curnode->parent);
//			curnode->sibling = node;
//			curnode = node;
//		} else if (xlevel == curlev + 1) {
//			node = createGNode(xkey, xtag, xvalue, curnode);
//			curnode->child = node;
//			curnode = node;
//			curlev = xlevel;
//		} else if (xlevel < curlev) {
//			if (xlevel < 0) {
//				printf("Error: line %d: illegal level", xfileLine);
//				return null;
//			}
//			while (xlevel < curlev) {
//				curnode = curnode->parent;
//				curlev--;
//			}
//			node = createGNode(xkey, xtag, xvalue, curnode->parent);
//			curnode->sibling = node;
//			curnode = node;
//		} else {
//			printf("Error: line %d: illegal level", xfileLine);
//			return null;
//		}
//	}
//	return root;
//}
