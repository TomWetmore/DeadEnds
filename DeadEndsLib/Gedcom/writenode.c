//  DeadEnds
//
//  writenode.c has the functions that write GNodes and GNode trees and forests to Strings and
//  FILEs.
//
//  Created by Thomas Wetmore on 2 May 2023.
//  Last changed on 25 August 2025.
//

#include "standard.h"
#include "gnode.h"
#include "writenode.h"

void writeGNodes(FILE*, int level, GNode*, bool indent, bool kids, bool sibs);
void writeGNode(FILE*, int level, GNode*, bool indent);

static String swriteGNodes(int level, GNode*, String);
static String swriteGNode(int level, GNode*, String);
static int nodeStringLength(int, GNode*);

// gnodesToFile writes a GNode tree or forest to a file. Opens the file, calls writeGNodes to
// write the nodes, and closes the file. Returns whether the write occurred. Level is level of
// the top GNode (always zero?); gnode is the root, fileName is the file name and indent
// indicates whether the first should be indented.
bool gnodesToFile(int level, GNode* gnode, String fileName, bool indent) {
    FILE *fp;
    if (!(fp = fopen(fileName, "w"))) {
        printf("Could not open file: `%s'\n", fileName);
        return false;
    }
    writeGNodes(fp, level, gnode, indent, true, true);
    fclose(fp);
    return true;
}

// writeGNode writes a single GNode to a file. Level is the node's level; indent specifies whether
// to indent the lines.
void writeGNode(FILE* fp, int level, GNode* gnode, bool indent) {
    if (indent) for (int i = 1; i < level; i++) fputs("  ", fp);
    fprintf(fp, "%d", level);
    if (gnode->key) fprintf(fp, " %s", gnode->key);
    fprintf(fp, " %s", gnode->tag);
    if (gnode->value && *gnode->value) fprintf(fp, " %s", gnode->value);
    fprintf(fp, "\n");
}

// writeGNodeRecord writes a GNode record to a file.
void writeGNodeRecord(FILE* fp, GNode* root, bool indent) {
	writeGNodes(fp, 0, root, indent, true, true);
}

// writeGNodes writes a GNode tree or forest to a file. Level is the current level; indent
// specifies indenting; kids and sibs indicate recursing to children and siblings.
void writeGNodes(FILE* fp, int level, GNode* gnode, bool indent, bool kids, bool sibs) {
    if (!gnode) return;
    writeGNode(fp, level, gnode, indent);
    if (kids) writeGNodes(fp, level + 1, gnode->child, indent, true, true);
    if (sibs) writeGNodes(fp, level, gnode->sibling, indent, kids, true);
}

// gnodesToString returns a GNode tree converted to a String. Finds the length of the String,
// allocates it, and fills it with Gedcom text.
String gnodesToString(GNode* gnode) {
    int length = treeStringLength(0, gnode) + 1; // + 1 for final \0.
    if (length <= 0) return null;
    String string = (String) stdalloc(length);
    (void) swriteGNodes(0, gnode, string);
    return string;
}

/// Returns a `GNode` as a Gedcom `String` without newline.
String gnodeToString(GNode* gnode, int level) {
    int length = nodeStringLength(level, gnode) + 1;
    String string = (String) stdalloc(length);
    swriteGNode(level, gnode, string);
    string[strlen(string) - 1] = 0;
    return string;
}

// swriteGNode writes a GNode to a string and returns the position in string of next GNode.
static String swriteGNode(int level, GNode* node, String p) {
    size_t length = (size_t) nodeStringLength(level, node) + 1;
    if (node->key && node->value) {
        snprintf(p, length, "%d %s %s %s\n", level, node->key, node->tag, node->value);
    } else if (node->key) {
        snprintf(p, length, "%d %s %s\n", level, node->key, node->tag);
    } else if (node->value) {
        snprintf(p, length, "%d %s %s\n", level, node->tag, node->value);
    } else {
        snprintf(p, length, "%d %s\n", level, node->tag);
    }
    return p + strlen(p);
}

// swriteGNodes writes a GNode tree or forest to a String. Recurses to children and siblings.
static String swriteGNodes (int level, GNode* gnode, String p) {
    while (gnode) {
        p = swriteGNode(level, gnode, p);
        if (gnode->child) p = swriteGNodes(level + 1, gnode->child, p);
        gnode = gnode->sibling;
    }
    return p;
}

// treeStringLength computes the string length of a GNode forest.
int treeStringLength(int level, GNode* gnode) {
    int length = 0;
    while (gnode) {
        length += nodeStringLength(level, gnode);
        if (gnode->child)
            length += treeStringLength(level + 1, gnode->child);
        gnode = gnode->sibling;
    }
    return length;
}

// nodeStringLength returns the a GNode's string length; it counts the \n but not the final 0.
static int nodeStringLength(int level, GNode* gnode) {
    char scratch[32];
    int wrote = snprintf(scratch, sizeof(scratch), "%d", level);
    if (wrote < 0) return 0;
    size_t len = (size_t) wrote + 1;
    if (gnode->key) len += strlen(gnode->key) + 1;
    len += strlen(gnode->tag);
    if (gnode->value) len += strlen(gnode->value) + 1;
    return (int) len + 1;
}
