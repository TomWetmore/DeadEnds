//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 17 August 2025.
//

#include <stdio.h>
#include "context.h"
#include "frame.h"
#include "file.h"
#include "functiontable.h"
#include "hashtable.h"
#include "interp.h"
#include "pnode.h"

// Creates an empty Program structure.
Program* createProgram(void) {
    return (Program*) stdalloc(sizeof(Program));
}

// createContext creates an empty Context.
Context* createContext(void) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    if (context) memset(context, 0, sizeof(Context));
    return context;
}

// deleteContext deletes a Context; it deletes the run time stack and closes the file, but leaves everything else.
//void deleteContext(Context *context) {
//    Frame* frame = context->frame;
//    while (frame) {
//        Frame* parent = frame->caller;
//        deleteFrame(frame);
//        frame = parent;
//    }
//    if (context->file) closeFile(context->file);
//    stdfree(context);
//}

static void validatePNodeCalls(PNode*, Program*);
static bool isBuiltinFunction(const char* name);

/// Checks that the functions and procedures called by a script are defined.
void validateCalls(Program* program) {

    ASSERT(program);
    FORHASHTABLE(program->procedures, entry) // Check procedures.
        PNode* proc = ((FunctionElement*) entry)->function;
        validatePNodeCalls(proc->procBody, program);
    ENDHASHTABLE
    FORHASHTABLE(program->functions, entry) // Check functions.
        PNode* func = ((FunctionElement*) entry)->function;
        validatePNodeCalls(func->funcBody, program);
    ENDHASHTABLE
}

/// Checks that PNProcCall and PNFuncCall PNodes that call defined modules.
static void validatePNodeCalls(PNode* node, Program* program) {
    while (node) {
        switch (node->type) {
        case PNProcCall: {
            String name = node->procName;
            if (!searchHashTable(program->procedures, name)) {
                scriptError(node, "undefined procedure: %s", name);
            }
            break;
        }
        case PNFuncCall: {
            String name = node->funcName;
            if (!searchHashTable(program->functions, name) &&
                !isBuiltinFunction(name)) {
                scriptError(node, "undefined function: %s", name);
            }
            break;
        }
        case PNBltinCall: break;
        case PNIf:
            validatePNodeCalls(node->condExpr, program);
            validatePNodeCalls(node->thenState, program);
            validatePNodeCalls(node->elseState, program);
            break;
        case PNWhile:
            validatePNodeCalls(node->condExpr, program);
            validatePNodeCalls(node->loopState, program);
            break;
        case PNReturn:
            validatePNodeCalls(node->returnExpr, program);
            break;
        default:
            // Loops and blocks
            validatePNodeCalls(node->condExpr, program);
            validatePNodeCalls(node->expression, program);
            validatePNodeCalls(node->gnodeExpr, program);
            validatePNodeCalls(node->listExpr, program);
            validatePNodeCalls(node->setExpr, program);
            validatePNodeCalls(node->sequenceExpr, program);
            validatePNodeCalls(node->pnodeOne, program);
            validatePNodeCalls(node->pnodeTwo, program);
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
