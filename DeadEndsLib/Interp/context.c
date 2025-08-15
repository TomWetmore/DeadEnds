//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 12 August 2025.
//

#include <stdio.h>
#include "context.h"
#include "frame.h"
#include "file.h"
#include "functiontable.h"
#include "hashtable.h"
#include "interp.h"
#include "pnode.h"

// createEmptyContext creates an empty Context.
Context* createEmptyContext(void) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    if (context) memset(context, 0, sizeof(Context));
    return context;
}

// deleteContext deletes a Context; it deletes the run time stack and closes the file, but leaves everything else.
void deleteContext(Context *context) {
    Frame* frame = context->frame;
    while (frame) {
        Frame* parent = frame->caller;
        deleteFrame(frame);
        frame = parent;
    }
    if (context->file) closeFile(context->file);
    stdfree(context);
}

static void validatePNodeCalls(PNode* node, Context* context);
static bool isBuiltinFunction(const char* name);

/// Checks that the functions and procedures called by a script are defined.
void validateCalls(Context* context) {

    ASSERT(context);
    FORHASHTABLE(context->procedures, entry) // Check procedures.
        PNode* proc = ((FunctionElement*) entry)->function;
        validatePNodeCalls(proc->procBody, context);
    ENDHASHTABLE
    FORHASHTABLE(context->functions, entry) // Check functions.
        PNode* func = ((FunctionElement*) entry)->function;
        validatePNodeCalls(func->funcBody, context);
    ENDHASHTABLE
}

/// Checks that PNProcCall and PNFuncCall PNodes that call defined modules.
static void validatePNodeCalls(PNode* node, Context* context) {
    while (node) {
        switch (node->type) {
        case PNProcCall: {
            String name = node->procName;
            if (!searchHashTable(context->procedures, name)) {
                scriptError(node, "undefined procedure: %s", name);
            }
            break;
        }
        case PNFuncCall: {
            String name = node->funcName;
            if (!searchHashTable(context->functions, name) &&
                !isBuiltinFunction(name)) {
                scriptError(node, "undefined function: %s", name);
            }
            break;
        }
        case PNBltinCall: break;
        case PNIf:
            validatePNodeCalls(node->condExpr, context);
            validatePNodeCalls(node->thenState, context);
            validatePNodeCalls(node->elseState, context);
            break;
        case PNWhile:
            validatePNodeCalls(node->condExpr, context);
            validatePNodeCalls(node->loopState, context);
            break;
        case PNReturn:
            validatePNodeCalls(node->returnExpr, context);
            break;
        default:
            // Loops and blocks
            validatePNodeCalls(node->condExpr, context);
            validatePNodeCalls(node->expression, context);
            validatePNodeCalls(node->gnodeExpr, context);
            validatePNodeCalls(node->listExpr, context);
            validatePNodeCalls(node->setExpr, context);
            validatePNodeCalls(node->sequenceExpr, context);
            validatePNodeCalls(node->pnodeOne, context);
            validatePNodeCalls(node->pnodeTwo, context);
            break;
        }
        node = node->next;
    }
}

static bool isBuiltinFunction(const char* name) {
    for (int i = 0; i < numBuiltIns; i++) {
        if (eqstr(builtIns[i].name, name)) return true;
    }
    return false;
}
