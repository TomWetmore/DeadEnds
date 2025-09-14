//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 13 September 2025.
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

static void validatePNodeCalls(PNode*, FunctionTable*, FunctionTable*);
static bool isBuiltinFunction(const char* name);

/// Checks that functions and procedures called by a program are defined.
void validateCalls(FunctionTable* procedures, FunctionTable* functions) {
    ASSERT(procedures && functions);
    FORHASHTABLE(procedures, entry) // Check procedures.
        PNode* proc = ((FunctionElement*) entry)->function;
        validatePNodeCalls(proc->procBody, procedures, functions);
    ENDHASHTABLE
    FORHASHTABLE(functions, entry) // Check functions.
        PNode* func = ((FunctionElement*) entry)->function;
        validatePNodeCalls(func->funcBody, procedures, functions);
    ENDHASHTABLE
}

/// Checks that PNProcCall and PNFuncCall PNodes call defined procedures and functions.
static void validatePNodeCalls(PNode* node, FunctionTable* procedures, FunctionTable* functions) {
    while (node) {
        switch (node->type) {
        case PNProcCall: {
            String name = node->procName;
            if (!searchHashTable(procedures, name)) {
                scriptError(node, "undefined procedure: %s", name);
            }
            break;
        }
        case PNFuncCall: {
            String name = node->funcName;
            if (!searchHashTable(functions, name) &&
                !isBuiltinFunction(name)) {
                scriptError(node, "undefined function: %s", name);
            }
            break;
        }
        case PNBltinCall: break;
        case PNIf:
            validatePNodeCalls(node->condExpr, procedures, functions);
            validatePNodeCalls(node->thenState, procedures, functions);
            validatePNodeCalls(node->elseState, procedures, functions);
            break;
        case PNWhile:
            validatePNodeCalls(node->condExpr, procedures, functions);
            validatePNodeCalls(node->loopState, procedures, functions);
            break;
        case PNReturn:
            validatePNodeCalls(node->returnExpr, procedures, functions);
            break;
        default:
            // Loops and blocks
            validatePNodeCalls(node->condExpr, procedures, functions);
            validatePNodeCalls(node->expression, procedures, functions);
            validatePNodeCalls(node->gnodeExpr, procedures, functions);
            validatePNodeCalls(node->listExpr, procedures, functions);
            validatePNodeCalls(node->setExpr, procedures, functions);
            validatePNodeCalls(node->sequenceExpr, procedures, functions);
            validatePNodeCalls(node->pnodeOne, procedures, functions);
            validatePNodeCalls(node->pnodeTwo, procedures, functions);
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
