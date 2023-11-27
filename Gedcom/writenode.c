//
//  DeadEnds
//
//  writenode.c -- Functions that deal with writing gedcom nodes or node trees to strings and
//    files.
//
//  Created by Thomas Wetmore on 2 May 2023.
//  Last changed on 2 May 2023
//

#include "standard.h"
#include "gnode.h"
#include "writenode.h"

// Static functions defined in this file.
//--------------------------------------------------------------------------------------------------
void writeGNodes(FILE*, int level, GNode*, bool indent, bool kids, bool sibs);
void writeGNode(FILE*, int level, GNode*, bool indent);
static String swriteGNodes(int level, GNode*, String);
static String swriteGNode(int level, GNode*, String);
static int nodeStringLength(int, GNode*);

//  gnodesToFile -- Write a gedcom tree to a gedcom file. Opens the file, calls writeGNodes to
//    write the nodes, and closes the file. Returns whether the write occurred.
//    TODO: SHOULD WE GET RID OF THE LEVEL PARAMETER AND PASS 0 TO WRITEGNODES?
//--------------------------------------------------------------------------------------------------
bool gnodesToFile(int level, GNode* gnode, String fileName, bool indent)
// level -- Level of node; always zero?
// node -- GNode at root of tree to write.
// fname -- Output file name.
// indent -- True if lines are indented.
{
    FILE *fp;
    if (!(fp = fopen(fileName, "w"))) {
        printf("Could not open file: `%s'\n", fileName);
        return false;
    }
    writeGNodes(fp, level, gnode, indent, true, true);
    fclose(fp);
    return true;
}

//  writeGNode -- Write a single gedcom node to a file. Called by writeGNodes.
//--------------------------------------------------------------------------------------------------
void writeGNode(FILE *fp, int level, GNode* gnode, bool indent)
//  fp -- Output file.
//  level -- Level of the node.
//  gnode -- Node to write.
//  indent -- True if lines are indented.
{
    if (indent) for (int i = 1; i < level; i++) fprintf(fp, "  ");
    fprintf(fp, "%d", level);
    if (gnode->key) fprintf(fp, " %s", gnode->key);
    fprintf(fp, " %s", gnode->tag);
    if (gnode->value) fprintf(fp, " %s", gnode->value);
    fprintf(fp, "\n");
}

//  writeGNodes -- Write a node tree or forest to a Gedcom file. Recurse to children and siblings.
//--------------------------------------------------------------------------------------------------
void writeGNodes(FILE *fp, int level, GNode* gnode, bool indent, bool kids, bool sibs)
// fp -- Output file.
// level -- Level of the GNode of this call.
// gnode -- GNode of this call.
// indent -- True if lines are indented.
// kids -- True if children are included.
// sibs -- True if siblings are included.
{
    if (!gnode) return;

    // Write the specfied node to the file.
    writeGNode(fp, level, gnode, indent);

    //  Recurse to children and siblings if so indicated.
    if (kids) writeGNodes(fp, level + 1, gnode->child, indent, true, true);
    if (sibs) writeGNodes(fp, level, gnode->sibling, indent, kids, true);
}

//  gnodesToString -- Return a gedcom record tree converted to a string. Two steps. First find
//    the length of String needed and allocate it. Second fill the string with the output.
//    Return the string.
//--------------------------------------------------------------------------------------------------
String gnodesToString(GNode* gnode)
// gnode -- Root gedcom node of a record to be put into string form.
{
    // Find the length of a string to hold the record.
    int length = treeStringLength(0, gnode) + 1;  //  + 1 for the final \0.
    if (length <= 0) return null;

    // Allocate the needed memory.
    String string = (String) stdalloc(length);

    // Call swriteGNodes to write the String.
    (void) swriteGNodes(0, gnode, string);
    return string;
}

//  gnodeToString -- Return a single gedcom node as a string. This does not add a newline at
//    the end. Calls nodeStringLength to find the length of string needed, then calls swriteGNode
//    to write the node to a string, and overwrites the final newline with a \0.
//--------------------------------------------------------------------------------------------------
String gnodeToString(GNode* gnode, int level)
{
    // Get the length of String needed and allocate space.
    int length = nodeStringLength(level, gnode);
    String string = (String) stdalloc(length);
    // Use swriteGNode to convert to String and then write a zero on the newline.
    swriteGNode(level, gnode, string);
    string[strlen(string) - 1] = 0;
    return string;
}

//  swriteGNode -- Write a Node to a string. Returns the next location in the String p to add the
//    next GNode.
//--------------------------------------------------------------------------------------------------
static String swriteGNode(int level, GNode* node, String p)
// level -- Level of this GNode.
// node -- GNode to render as a String.
// p -- Location to copy the String to before returning.
{
    // Buffer use only to compose the line.
    char scratch[600];
    String q = scratch;
    // Write the level to the buffer.
    sprintf(q, "%d ", level);
    q += strlen(q);
    // If the GNode has a key write it to buffer.
    if (node->key) {
        strcpy(q, node->key);
        q += strlen(q);
        *q++ = ' ';
    }
    // Write the tag to the buffer.
    strcpy(q, node->tag);
    q += strlen(q);
    // If the GNode has a value write it to the buffer.
    if (node->value) {
        *q++ = ' ';
        strcpy(q, node->value);
        q += strlen(q);
    }
    // Add a newline to the buffer, and then copy the buffer to the end of the input String.
    *q++ = '\n';
    *q = 0;
    strcpy(p, scratch);
    // Return the position in the input String where the next GNode line will go.
    return p + strlen(p);
}

//  swriteGNodes -- Write GNode tree to String. Recurses to children and siblings.
//--------------------------------------------------------------------------------------------------
static String swriteGNodes (int level, GNode* gnode, String p)
// level -- Level of the GNode being written on this call.
// node -- GNode being written on this call.
// p -- Location in String where the representation of this GNode goes.
{
    // Iterate through this GNode and its siblings.
    while (gnode) {
        // Output this GNode.
        p = swriteGNode(level, gnode, p);
        // If this GNode has children recurse.
        if (gnode->child) p = swriteGNodes(level + 1, gnode->child, p);
        gnode = gnode->sibling;
    }
    return p;
}

//  treeStringLength -- Recursively compute string length of a gedcom tree. This is used for the
//    pre-allocation of the memory needed to hold the full string.
//--------------------------------------------------------------------------------------------------
int treeStringLength(int level, GNode* gnode)
//  level -- Level of this GNode.
//  gnode -- Current GNode.
{
    int length = 0;
    while (gnode) {
        length += nodeStringLength(level, gnode);
        if (gnode->child)
            length += treeStringLength(level + 1, gnode->child);
        gnode = gnode->sibling;
    }
    return length;
}

//  nodeStringLength -- Compute a gedcom node's string length; counts the \n but not the final 0.
//    TODO: UNICODE IMPACT.
//--------------------------------------------------------------------------------------------------
static int nodeStringLength(int level, GNode* gnode)
{
    size_t len;
    char scratch[10];  // To hold the level.
    sprintf(scratch, "%d", level);
    len = strlen(scratch) + 1;
    if (gnode->key) len += strlen(gnode->key) + 3; // + 3 for space and 2 @-signs.
    len += strlen(gnode->tag);
    if (gnode->value) len += strlen(gnode->value) + 1;  // + 1 for the space after the tag.
    return (int) len + 1;  // + 1 for the newline.
}
