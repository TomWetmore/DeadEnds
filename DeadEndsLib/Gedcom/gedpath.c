// DeadEnds
//
// gedpath.c has the functions that implement Gedcom paths. A Gedcom path is a linked list of
// structs that represents a path from a GNode to one or more GNodes in the tree below.
//
// Created by Thomas Wetmore on 13 October 2024.
// Last changed on 22 October 2024.

#include "gedpath.h"
#include "gnode.h"
#include "gnodelist.h"

static bool traverseDebug = true;

// buildPath parses a Gedcom path expression to a GedPath. For example the string
// "INDI->ANY*->DATE*" / is converted to a linked list of three GedPath structs.
GedPath* buildGedPath(String string) {
	GedPath* first = null;
	GedPath* prev = null;
	char *expression = strdup(string);
	String token = strtok(expression, "->"); // First component.
	while (token) {
		GedPath* path = createGedPath();
		int length = (int) strlen(token);
		if (token[length - 1] == '*') { // All component
			token = strndup(token, length - 1);
			path->all = true;
		}
		if (eqstr("ANY", token)) {
			path->tag = null; // Any tag component.
			path->any = true;
		} else {
			path->tag = strdup(token);
		}
		path->next = null;
		if (!first) first = path;
		else prev->next = path;
		prev = path;
		token = strtok(null, "->"); // Next component.
	}
	free(expression);
	return first;
}

// traverseGedPath traverses a Gedcom path collecting the GNodes at the endpoints.
void traverseGedPath(GNode *node, GedPath *path, GNodeList* results, int *count) {
	if (traverseDebug) printf("TGP: path: %s.\n", path->tag ? path->tag : "ANY");
	if (!node) return;
	while (node) {
		if (traverseDebug) printf("TGP: node: %s.\n", node->tag);
		if (path->any || (path->tag && eqstr(node->tag, path->tag))) { // Check for match.
			if (!path->next) { // At end of path?
				appendToList(results, node); // Record match.
				(*count)++;
				if (traverseDebug) printf("TGP: fmatch: %s, count: %d.\n", node->tag, *count);
			} else { // Take next step on path.
				if (traverseDebug) printf("TGP: imatch: %s.\n", node->tag);
				int count0 = *count;
				if (traverseDebug) printf(">>>>\n");
				traverseGedPath(node->child, path->next, results, count);
				if (traverseDebug) printf("<<<<\n");
				if (*count > count0 && !path->all) return; // Matches found during recursion.
			}
		}
		node = node->sibling;
	}
}

// createGedPath creates a GedPath node.
GedPath* createGedPath(void) {
	GedPath* node = stdalloc(sizeof(GedPath));
	node->tag = "";
	node->all = false;
	node->any = false;
	node->next = null;
	return node;
}

// showGedPath is a debug function that prints a GedPath.
void showGedPath(GedPath* path) {
	while (path) {
		printf("%s", path->tag ? path->tag : "any");
		printf("%d%d ", path->any, path->all);
		path = path->next;
	}
	printf("\n");
}

// showGNodePath prints the path from the root of a GNode tree to a given node in the tree.
// Intended for debubbing the GedPath feature.
int showGNodePath(GNode* node, int level) {
	// Base case: at the root.
	if (!node->parent) {
		printf("%d %s %s\n", level, node->tag, node->value ? node-> value : "");
		return level + 1;
	}
	// Recursive case: below the root.
	level = showGNodePath(node->parent, level);
	printf("%d %s %s\n", level, node->tag, node->value ? node->value : "");
	return level + 1;
}

// deleteGedPath returns the memory used by a GedPath list.
void deleteGedPath(GedPath* path) {
	if (!path) return;
	deleteGedPath(path->next);
	if (path->tag) free(path->tag);
	free(path);
}
