// DeadEnds
//
// builtintable.c holds the three built-in functions that provide the table data type for the
// DeadEnds script language. It is implemented using PValueTable.
//
// Created by Thomas Wetmore on 19 April 2023.
// Last changed on 2 May 2024.

#include "pvaluetable.h"
#include "interp.h"

// __table creates a DeadEnds script value table.
// usage: table(IDENT) -> VOID
PValue __table(PNode* pnode, Context* context, bool* eflg) {
    PNode* var = pnode->arguments; // Ident.
    if (var->type != PNIdent) {
        *eflg = true;
        scriptError(pnode, "the argument to table must be an identifier");
        return nullPValue;
    }
    PValueTable *pvtable = createPValueTable();
    assignValueToSymbol(context->symbolTable, var->identifier, PVALUE(PVTable, uTable, pvtable));
    return nullPValue;
}

// __insert adds an element to a script value table.
// usage: insert(TAB, STRING, ANY) -> VOID
PValue __insert (PNode *node, Context *context, bool *eflg) {
    PNode *arg = node->arguments; // Ident.
    if (arg->type != PNIdent) {
        *eflg = true;
        scriptError(node, "the first argument to insert must be an identifier");
        return nullPValue;
    }
    PValue pvalue = evaluate(arg, context, eflg); // Table.
    if (*eflg || pvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to insert must identify a table");
    }
    PValueTable *table = pvalue.value.uTable;
    arg = arg->next; // Key.
    if (arg->type != PNIdent) {
        *eflg = true;
        scriptError(node, "the second argument to insert must be an identifier");
        return nullPValue;
    }
    String key = arg->stringCons;
    pvalue = evaluate(arg->next, context, eflg); // Element.
    if (*eflg) {
        scriptError(node, "the third argument to insert must be a value");
        return nullPValue;
    }
    insertInPValueTable(table, key, pvalue); // Add (key, value).
    return nullPValue;
}

// __lookup looks up an element in a script value table.
// usage: lookup(TAB, STRING) -> ANY
PValue __lookup (PNode* node, Context* context, bool* eflg) {
    PNode* arg = node->arguments; // Table.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = pvalue.value.uTable;
    if (arg->next->type != PNIdent) { // Key.
        *eflg = true;
        scriptError(node, "the second argument to lookup must be an identifier");
        return nullPValue;
    }
    String key = arg->stringCons;
    pvalue = getValueOfPValueElement(table, key);
    return pvalue;
}
