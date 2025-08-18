//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 18 August 2025.
//

#include <stdio.h>
#include "context.h"
#include "frame.h"
#include "file.h"
#include "functiontable.h"
#include "hashtable.h"
#include "interp.h"
#include "list.h"
#include "pnode.h"
#include "pvalue.h"
#include "symboltable.h"

/// Creates a Program structure.
Program* createProgram(void) {
    return (Program*) stdalloc(sizeof(Program));
}

/// Deletes a Program structure.
void deleteProgram(Program* program) {
    deleteFunctionTable(program->procedures);
    deleteFunctionTable(program->functions);
    deleteList(program->parsedFiles);
    deleteList(program->globalIdents);
    stdfree(program);
}

/// Creates a Context structure.
Context* createContext(Program* program, Database* database, File* outfile) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    context->program = program;
    context->database = database;
    context->file = outfile;
    context->frame = null;
    context->globals = createSymbolTable();
    FORLIST(program->globalIdents, ident)
        assignValueToSymbolTable(context->globals, (String) ident, nullPValue);
    ENDLIST
    return context;
}

/// Deletes a Context structure.
void deleteContext(Context *context) {
    deleteSymbolTable(context->globals);
    if (context->file) closeFile(context->file);
    stdfree(context);
}

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

/// Checks that PNProcCall and PNFuncCall PNodes call defined procedures and functions.
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
