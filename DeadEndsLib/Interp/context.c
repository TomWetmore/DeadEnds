//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 3 July 2025.
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

// createContext creates a Context from a Database and a File.
//Context* createContext(Database *database, File* file) {
//    Context* context = (Context*) stdalloc(sizeof(Context));
//    context->frame = null;
//    context->database = database;
//    context->file = file;
//    return context;
//}

// deleteContext deletes a Context; it deletes the run time stack and closes the file, but leaves the database.
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

// validateCalls checks that all functions and procedures called in a script are already defined.
void validateCalls(Context* context) {
    ASSERT(context);

    // Check that all user defined procedures call defined modules.
    FORHASHTABLE(context->procedures, entry)
        PNode* proc = ((FunctionElement*) entry)->function;
        validatePNodeCalls(proc->procBody, context);
    ENDHASHTABLE

    // Check that all user defined functions call defined modules.
    FORHASHTABLE(context->functions, entry)
        PNode* func = ((FunctionElement*) entry)->function;
        validatePNodeCalls(func->funcBody, context);
    ENDHASHTABLE
}

// validatePNode validates that individual PNodes that call modules call defined modules.
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
        case PNBltinCall: /* skip; already handled above */ break;
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
