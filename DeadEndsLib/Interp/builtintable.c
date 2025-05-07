// DeadEnds
//
// builtintable.c holds the three built-in functions that provide the table data type for the
// DeadEnds script language. It is implemented using PValueTable.
//
// Created by Thomas Wetmore on 19 April 2023.
// Last changed on 5 May 2024.

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
    PNode* arg = node->arguments; // Table.
    PValue tvalue = evaluate(arg, context, eflg);
    if (*eflg || tvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;
    arg = arg->next; // Key.
    PValue svalue = evaluate(arg, context, eflg);
    if (*eflg || svalue.type != PVString) {
        *eflg = true;
        scriptError(node, "the second argument to lookup must be a string");
        return nullPValue;
    }
    String key = strsave(svalue.value.uString); // Need ownership.
    arg = arg->next; // Element
    PValue evalue = evaluate(arg, context, eflg);
    if (*eflg) {
        scriptError(node, "the third argument to insert must be a value");
        return nullPValue;
    }
    insertInPValueTable(table, key, *clonePValue(&evalue));
    return nullPValue;
}

// __lookup looks up an element in a script value table.
// usage: lookup(TAB, STRING) -> ANY
PValue __lookup (PNode* node, Context* context, bool* eflg) {
    PNode* arg = node->arguments; // Table.
    PValue tvalue = evaluate(arg, context, eflg);
    if (*eflg || tvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;
    arg = arg->next;
    PValue svalue = evaluate(arg, context, eflg); // Key.
    if (*eflg || svalue.type != PVString) {
        *eflg = true;
        scriptError(node, "the second argument to lookup must be a string");
        return nullPValue;
    }
    String key = svalue.value.uString;
    PValue pvalue = getValueOfPValueElement(table, key);
    return cloneAndReturnPValue(&pvalue);
}

// __insert adds an element to a script value table.
// usage: insert(TAB, STRING_OR_IDENT, ANY) -> VOID
PValue __newinsert(PNode *node, Context *context, bool *eflg) {
    // --- Evaluate the first argument: the table
    PNode *arg = node->arguments;
    PValue tvalue = evaluate(arg, context, eflg);
    if (*eflg || tvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to insert must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;

    // --- Determine the key string (2nd argument)
    arg = arg->next;
    String key = NULL;
    if (arg->type == PNIdent) {
        // Treat identifier name as string
        key = strsave(arg->identifier);
    } else {
        PValue kvalue = evaluate(arg, context, eflg);
        if (*eflg || kvalue.type != PVString) {
            *eflg = true;
            scriptError(node, "the second argument to insert must be a string or identifier");
            return nullPValue;
        }
        key = strsave(kvalue.value.uString);
    }

    // --- Evaluate the third argument: the value to insert
    arg = arg->next;
    PValue evalue = evaluate(arg, context, eflg);
    if (*eflg) {
        scriptError(node, "the third argument to insert must be a value");
        stdfree(key);
        return nullPValue;
    }

    // --- Insert the cloned value into the table under the given key
    insertInPValueTable(table, key, *clonePValue(&evalue));
    stdfree(key); // Safe to free the key after insertion (insert makes its own copy)
    return nullPValue;
}

// __lookup retrieves a value from a script value table.
// usage: lookup(TAB, STRING_OR_IDENT) -> ANY
PValue __newlookup(PNode *node, Context *context, bool *eflg) {
    // --- Evaluate the first argument: the table
    PNode *arg = node->arguments;
    PValue tvalue = evaluate(arg, context, eflg);
    if (*eflg || tvalue.type != PVTable) {
        *eflg = true;
        scriptError(node, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = tvalue.value.uTable;

    // --- Determine the key string (2nd argument)
    arg = arg->next;
    String key = NULL;
    if (arg->type == PNIdent) {
        // Treat identifier name as string
        key = strsave(arg->identifier);
    } else {
        PValue kvalue = evaluate(arg, context, eflg);
        if (*eflg || kvalue.type != PVString) {
            *eflg = true;
            scriptError(node, "the second argument to lookup must be a string or identifier");
            return nullPValue;
        }
        key = strsave(kvalue.value.uString);
    }

    // --- Perform lookup
    PValue pvalue = getValueOfPValueElement(table, key);
    stdfree(key); // Safe to free — lookup does not hold onto the key

    // --- Return cloned result
    return cloneAndReturnPValue(&pvalue);
}
