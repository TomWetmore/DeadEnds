//
//  DeadEnds
//
//  builtintable.c -- The three built-in functions that provide the table data type for the
//    DeadEnds programming language. The table data type is implemented using the PValueTable
//    hash table.
//
//  Created by Thomas Wetmore on 19 April 2023.
//  Last changed on 15 November 2023.
//

#include "pvaluetable.h"
#include "interp.h"

//  __table -- Create a program value table.
//    usage: table(IDENT) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __table(PNode *pnode, Context *context, bool *eflg)
{
    //  Get the identifier of the table.
    PNode *var = pnode->arguments;
    if (var->type != PNIdent) {
        *eflg = true;
        prog_error(pnode, "the argument to table must be an identifier");
        return nullPValue;
    }

    //  Create the program value table and add it to the symbol table.
    PValueTable *pvtable = createPValueTable();
    assignValueToSymbol(context->symbolTable, var->identifier, PVALUE(PVTable, uTable, pvtable));
    return nullPValue;
}

//  __insert -- Add an element to a program value table.
//    usage: insert(TAB, STRING, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __insert (PNode *node, Context *context, bool *eflg)
{
    //  Get the identifier of the table.
    PNode *arg = node->arguments;
    if (arg->type != PNIdent) {
        *eflg = true;
        prog_error(node, "the first argument to insert must be an identifier");
        return nullPValue;
    }

    //  Get the table.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVTable) {
        *eflg = true;
        prog_error(node, "the first argument to insert must identify a table");
    }
    PValueTable *table = pvalue.value.uTable;

    //  Get the key of the element to insert.
    arg = arg->next;
    if (arg->type != PNIdent) {
        *eflg = true;
        prog_error(node, "the second argument to insert must be an identifier");
        return nullPValue;
    }
    String key = arg->stringCons;

    //  Get the element to insert into the program value table.
    arg = arg->next;
    pvalue = evaluate(arg, context, eflg);
    if (*eflg) {
        prog_error(node, "the third argument to insert must be a value");
        return nullPValue;
    }

    //  Add a (key, value) element into the program value table.
    insertInPValueTable(table, key, pvalue);
    return nullPValue;
}

//  __lookup -- Look up element in table
//    usage: lookup(TAB, STRING) -> ANY
//--------------------------------------------------------------------------------------------------
PValue __lookup (PNode *node, Context *context, bool *eflg)
{
    //  Get the table.
    PNode *arg = node->arguments;
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVTable) {
        *eflg = true;
        prog_error(node, "the first argument to lookup must be a table");
        return nullPValue;
    }
    PValueTable *table = pvalue.value.uTable;

    //  Get the key string
    arg = arg->next;
    if (arg->type != PNIdent) {
        *eflg = true;
        prog_error(node, "the second argument to lookup must be an identifier");
        return nullPValue;
    }
    String key = arg->stringCons;

    pvalue = getValueOfPValueElement(table, key);
    return pvalue;
}
