//
//  DeadEnds Library
//  frame.c
//
//  Created by Thomas Wetmore on 20 May 2025.
//  Last changed on 28 July 2025.
//

#include "hashtable.h"
#include "pnode.h"
#include "pvalue.h"
#include "standard.h"
#include "set.h"
#include "stringset.h"
#include "symboltable.h"
#include "frame.h"

static StringSet* getParameterSet(PNode*);

/// Creates a new frame for the run time stack.
///
/// Called by interpProcCall and evaluateUserFunc.
///
/// - Parameters:
///   - pnode: `PNProcCall` node in the calling function.
///   - routine: `PNProcDef` node of the called function.
///   - table: `SymbolTable` for the called function.
///   - caller: `Frame` of the calling function.
Frame* createFrame(PNode* pnode, PNode* routine, SymbolTable* table, Frame* caller) {
    Frame* frame = (Frame*) stdalloc(sizeof(Frame));
    frame->call = pnode;
    frame->defn = routine;
    frame->table = table;
    frame->caller = caller;
    return frame;
}

/// Deletes a Frame.
void deleteFrame(Frame* frame) {
    deleteSymbolTable(frame->table);
    stdfree(frame);
}

/// Shows a Fame of the run time stack.
void showFrame(Frame* frame) {
    if (!frame) return;
    String name = frame->call->procName;
    int callline = frame->call->lineNumber;
    int defnline = frame->defn->lineNumber;
    printf("Frame: %s: defined: %d called: %d\n", name, defnline, callline);
    printf("  parameters:\n");
    StringSet* params = getParameterSet(frame->defn);
    SymbolTable* table = frame->table;
    FORSET(params, element)
        String param = (String) element;
        PValue pvalue = getValueFromSymbolTable(table, param);
        String type = typeOfPValue(pvalue);
        String value = valueOfPValue(pvalue);
        printf("    %s: %s: %s\n", param, type, value);
    ENDSET
    printf("  automatics:\n");
    FORHASHTABLE(table, element)
        Symbol* symbol = (Symbol*) element;
        String ident = symbol->ident;
        if (!isInSet(params, ident)) {
            PValue pvalue = getValueFromSymbolTable(table, ident);
            String type = typeOfPValue(pvalue);
            String value = valueOfPValue(pvalue);
            printf("    %s: %s: %s\n", ident, type, value);
        }
    ENDHASHTABLE
    deleteStringSet(params, false);
}

/// Gets the set of parameter names used by a procedure or user function.
static StringSet* getParameterSet(PNode* pnode) {
    StringSet* set = createStringSet();
    for (PNode* param = pnode->parameters; param; param = param->next) {
        addToSet(set, param->identifier);
    }
    return set;
}
