// DeadEnds
//
// writenode.c has the functions that write GNodes and GNode trees and forests to Strings and
// FILEs.
//
// Created by Thomas Wetmore on 2 May 2023.
// Last changed on 13 May 2024.

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
    if (gnode->value) fprintf(fp, " %s", gnode->value);
    fprintf(fp, "\n");
}

// writeGNodeRecord writes a GNode record to a file.
void writeGNodeRecord(FILE* fp, GNode* root, bool indent) {
	writeGNodes(fp, 0, root, false, true, true);
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

// gnodeToString returns a GNode as a String without newline.
String gnodeToString(GNode* gnode, int level) {
    int length = nodeStringLength(level, gnode);
    String string = (String) malloc(length);
    swriteGNode(level, gnode, string);
    string[strlen(string) - 1] = 0;
    return string;
}

// swriteGNode writes a GNode to a string and returns the position in string of next GNode.
static String swriteGNode(int level, GNode* node, String p) {
    char scratch[600];
    String q = scratch;
    sprintf(q, "%d ", level);
    q += strlen(q);
    if (node->key) {
        strcpy(q, node->key);
        q += strlen(q);
        *q++ = ' ';
    }
    strcpy(q, node->tag);
    q += strlen(q);
    if (node->value) {
        *q++ = ' ';
        strcpy(q, node->value);
        q += strlen(q);
    }
    *q++ = '\n';
    *q = 0;
    strcpy(p, scratch);
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
    size_t len;
    char scratch[10];  // To hold the level.
    sprintf(scratch, "%d", level);
    len = strlen(scratch) + 1;
    if (gnode->key) len += strlen(gnode->key) + 1;
    len += strlen(gnode->tag);
    if (gnode->value) len += strlen(gnode->value) + 1;
    return (int) len + 1;
}
