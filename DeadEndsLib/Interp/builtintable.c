// DeadEnds
//
// builtintable.c holds the three built-in functions that provide the table data type for the
// DeadEnds script language. It is implemented using PValueTable.
//
// Created by Thomas Wetmore on 19 April 2023.
// Last changed on 23 May 2024.

#include "pvaluetable.h"
#include "interp.h"
#include "context.h"

// __table creates a DeadEnds script value table.
// usage: table(IDENT) -> VOID
PValue __table(PNode* pnode, Context* context, bool* errflg) {
    PNode* var = pnode->arguments; // Table's identifier.
    if (var->type != PNIdent) {
        *errflg = true;
        scriptError(pnode, "the argument to table must be an identifier");
        return nullPValue;
    }
    PValueTable *pvtable = createPValueTable();
    assignValueToSymbol(context->frame->table, var->identifier, PVALUE(PVTable, uTable, pvtable));
    return nullPValue;
}

// __insert adds an element to a script value table.
// usage: insert(TAB, STRING, ANY) -> VOID
PValue __insert(PNode *pnode, Context *context, bool *errflg) {
    PNode* arg = pnode->arguments; // Table.
    PValue tvalue = evaluate(arg, context, errflg);
    if (*errflg || tvalue.type != PVTable) {
        *errflg = true;
        scriptError(pnode, "the first argument to insert must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;
    arg = arg->next; // Key for table entry.
    PValue svalue = evaluate(arg, context, errflg);
    if (*errflg || svalue.type != PVString) {
        *errflg = true;
        scriptError(pnode, "the second argument to insert must be a string");
        return nullPValue;
    }
    String key = strsave(svalue.value.uString); // Need ownership.
    arg = arg->next; // Value of table entry.
    PValue evalue = evaluate(arg, context, errflg);
    if (*errflg) {
        scriptError(pnode, "the third argument to insert must be a value");
        return nullPValue;
    }
    insertInPValueTable(table, key, *clonePValue(&evalue));
    return nullPValue;
}

// __lookup looks up an element in a script value table.
// usage: lookup(TAB, STRING) -> ANY
PValue __lookup(PNode* pnode, Context* context, bool* errflg) {
    PNode* arg = pnode->arguments; // Table.
    PValue tvalue = evaluate(arg, context, errflg);
    if (*errflg || tvalue.type != PVTable) {
        *errflg = true;
        scriptError(pnode, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;
    arg = arg->next;
    PValue svalue = evaluate(arg, context, errflg); // Key.
    if (*errflg || svalue.type != PVString) {
        *errflg = true;
        scriptError(pnode, "the second argument to lookup must be a string");
        return nullPValue;
    }
    String key = svalue.value.uString;
    PValue pvalue = getValueOfPValueElement(table, key);
    return cloneAndReturnPValue(&pvalue);
}
