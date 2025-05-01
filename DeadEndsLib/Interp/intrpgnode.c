//
//  DeadEnds
//
//  intrpgnode.c -- This file has the interpreter's built-in functions that handle program
//    nodes that hold gedcom nodes.
//
//  Created by Thomas Wetmore on 17 March 2023.
//  Last changed on 30 April 2025.
//

#include "standard.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"

// __key returns key of a root Gedcom node.
// usage: key(INDI|FAM) -> STRING
// NOTE: THIS IS A DIFFERENT INTERFACE THAN THAT USED BY LIFELINES.
PValue __key(PNode* pnode, Context* context, bool* eflg) {
    ASSERT(pnode && context);
    PValue value = evaluate(pnode->arguments, context, eflg);
    if (*eflg || !isRecordType(value.type)) return nullPValue;
    GNode* gnode = value.value.uGNode;
    if (gnode && gnode->key) return createStringPValue(gnode->key);
    return nullPValue;
}

// __xref returns the xref (aka key) field of a GNode.
// usage: xref(NODE) -> STRING
PValue __xref (PNode* pnode, Context* context, bool* eflg) {
    ASSERT(pnode && context);
    PValue value = evaluate(pnode->arguments, context, eflg);
    if (*eflg || !isGNodeType(value.type)) return nullPValue;
    GNode* gnode = value.value.uGNode;
    if (!gnode->key) return nullPValue;
    return createStringPValue(gnode->key);
}

// __tag returns the tag of a Gedcom node.
// usage: tag(NODE) -> STRING
PValue __tag (PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    GNode *gnode = evaluateGNode(pnode->arguments, context, errflg);
    if (*errflg || !gnode) {
        *errflg = true;
        return nullPValue;
    }
    return createStringPValue(gnode->tag);
}

// __value returns the value of a gedcom node; it may be empty.
// usage: value(NODE) -> STRING
PValue __value (PNode* pnode, Context* context, bool* eflg) {
    ASSERT(pnode && context);
    GNode *gnode = evaluateGNode(pnode->arguments, context, eflg);
    if (*eflg || !gnode) {
        *eflg = true;
        return nullPValue;
    }
    if (!gnode->value) return nullPValue;
    return createStringPValue(gnode->value);
}

// __parent returns the parent node of a gedcom node.
//  usage: parent(NODE) -> NODE
PValue __parent(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    GNode* gnode = evaluateGNode(pnode->arguments, context, errflg);
    if (*errflg || !gnode) {
        *errflg = true;
        return nullPValue;
    }
    if (!gnode->parent) return nullPValue;
    return PVALUE(PVGNode, uGNode, gnode->parent);
}

// __child returns the child node of a gedcom node.
//  usage: child(GNODE) -> GNODE
PValue __child(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    GNode* gnode = evaluateGNode(pnode->arguments, context, errflg);
    if (*errflg || !gnode) {
        *errflg = true;
        return nullPValue;
    }
    if (!gnode->child) return nullPValue;
    return PVALUE(PVGNode, uGNode, gnode->child);
}

// __sibling returns the sibling of a gedcom node.
// usage: sibling(NODE) -> NODE
PValue __sibling(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    GNode *gnode = evaluateGNode(pnode->arguments, context, errflg);
    if (*errflg || !gnode) {
        *errflg = true;
        return nullPValue;
    }
    if (!gnode->sibling) return nullPValue;
    return PVALUE(PVGNode, uGNode, gnode->sibling);
}
