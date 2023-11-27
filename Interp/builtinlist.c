//
//  DeadEnds
//
//  builtinlist.c -- Functions that implement the list datatype built-ins. They use the list
//    structure as the underlying C type. The elements of the lists are program values.
//
//  MNOTE: Memory management is an issue to be dealt with carefully.
//
//  Created by Thomas Wetmore on 16 April 2023.
//  Last changed on 16 November 2023.
//

#include "interp.h"
#include "list.h"

//  __list -- Create a list.
//    usage: list(IDENT) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __list(PNode *pnode, Context *context, bool *eflg)
{
    // Get the identifier.
    PNode *var = pnode->arguments;
    if (var->type != PNIdent) {
        prog_error(pnode, "the argument to list must be an identifier");
        *eflg = true;
        return nullPValue;
    }
    String ident = var->identifier;
    ASSERT(ident);

    //  Create the list the identifier will refer to.
    List *list = createList(null, null, null);  //  compare, delete, getkey
    //  MNOTE: Shouldn't there be a delete function?
    assignValueToSymbol(context->symbolTable, ident, PVALUE(PVList, uList, list));
    return nullPValue;
}

//  __push -- Push an element onto the front of a list.
//    usage: push(LIST, ANY) -> VOID
//    usage: enqueue(LIST, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __push(PNode *node, Context *context, bool *eflg)
{
    //  The first argument must be a list.
    PNode *arg = node->arguments;
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        *eflg = true;
        prog_error(node, "the first first argument to push/enqueue must be a list");
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list);

    //  The second argument must be a program value.
    pvalue = evaluate(arg->next, context, eflg);
    if (*eflg) {
        prog_error(node, "the second argument to push/enqueue must have a program value");
        return nullPValue;
    }

    //  Program values in a list are put in the heap.
    PValue *ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    prependListElement(list, ppvalue);
    return nullPValue;
}


//  __requeue -- Add an element to back of a list.
//    usage: requeue(LIST, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __requeue (PNode *node, Context *context, bool *eflg)
{
    //  The first argument must be a list.
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the first argument to requeue must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;

    //  The second argument must be a program value.
    pvalue = evaluate(node->arguments->next, context, eflg);
    if (*eflg) {
        prog_error(node, "the second argument to requeue must be a program value");
        return nullPValue;
    }

    //  Program values in a list must be stored in the heap.
    PValue *ppvalue = (PValue*) sizeof(PValue);
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    appendListElement(list, ppvalue);
    return nullPValue;
}

//  __pop -- Pop an element from the front of a list.
//    usage: pop(LIST) -> ANY
//--------------------------------------------------------------------------------------------------
PValue __pop(PNode *node, Context *context, bool *eflg)
{
    //  The first argument must be a list.
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the argument to pop must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list && list->length >= 0);

    //  Remove and return the first element of the list.
    PValue *ppvalue = removeFirstListElement(list);
    if (!ppvalue) return nullPValue;
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    stdfree(ppvalue);  //  MNOTE: Free the popped heap version of the program value.
    return pvalue;     //  MNOTE: Return the stack version of the program value.
}

//  __dequeue -- Remove an element from the back of a list.
//    usage dequeue(LIST) -> ANY
//--------------------------------------------------------------------------------------------------
PValue __dequeue(PNode *node, Context *context, bool *eflg)
{
    //  The argument must be a list.
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the argument to pop must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list && list->length >= 0);

    //  Remove and return the last element of the list.
    PValue *ppvalue = removeLastListElement(list);
    if (!ppvalue) return nullPValue;
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    stdfree(ppvalue);  //  MNOTE: Free the dequeued heap version of the program value.
    return pvalue;     //  MNOTE: Return the stack version of the program value.
}

//  __empty -- Check if a list is empty.
//    usage: empty(LIST) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __empty (PNode *pnode, Context *context, bool *eflg)
{
    //  The argument must be a list.
    PValue pvalue = evaluate(pnode->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(pnode, "the argument to empty is not a list");
        *eflg = true;
        return nullPValue;
    }
    ASSERT(pvalue.value.uList);
    return PVALUE(PVBool, uBool, isEmptyList(pvalue.value.uList));
}

//  __getel -- Get the nth value from a list.
//    usage: getel(LIST, INT) -> ANY
//--------------------------------------------------------------------------------------------------
PValue __getel (PNode *node, Context *context, bool *eflg)
{
    //  Get the list.
    PNode *arg = node->arguments;
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the first argument to getel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;

    //  Get the index.
    pvalue = evaluate(arg->next, context, eflg);
    if (*eflg || pvalue.type != PVInt) {
        prog_error(node, "the second argument to getel must be an integer index");
        *eflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;

    // Be sure the index is within range.
    if (index < 0 || index >= lengthList(list)) {
        prog_error(node, "the index to getel is out of range");
        *eflg = true;
        return nullPValue;
    }

    //  Return the indexed element which will be a pointer to a program value.
    PValue *ppvalue = (PValue*) getListElement(list, index);
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    return pvalue;
}

//  __setel -- Set nth value in list
//    usage: setel(LIST, INT, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __setel (PNode *node, Context *context, bool *eflg)
{
    //  Get the list.
    PNode *arg = node->arguments;
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the first argument to setel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;

    //  Get the index.
    arg = arg->next;
    pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVInt) {
        prog_error(node, "the second argument to setel must be a integer");
        *eflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;

    // Be sure the index is within range.
    if (index < 0 || index >= lengthList(list)) {
        prog_error(node, "the index to setel is out of range");
        *eflg = true;
        return nullPValue;
    }

    //  Get the value.
    arg = arg->next;
    pvalue = evaluate(arg, context, eflg);
    if (*eflg) {
        prog_error(node, "the third argument to setel is in error");
        return nullPValue;
    }
    PValue *ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    setListElement(list, index, ppvalue);
    return nullPValue;
}

//  __length -- Find the length of a list.
//    usage: length(LIST) -> INT
//--------------------------------------------------------------------------------------------------
PValue __length (PNode *node, Context *context, bool *eflg)
{
    //  Get the list.
    PNode *arg = node->arguments;
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        prog_error(node, "the first argument to setel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    return PVALUE(PVInt, uInt, lengthList(list));
}

//  interpForList -- Interpret list loop
//    usage: forlist(LIST, ANY, INT) {BODY}
//--------------------------------------------------------------------------------------------------
InterpType interpForList(PNode *node, Context *context, PValue *pval)
{
    //  Get the list.
    bool eflg = false;
    PValue pvalue = evaluate(node->listExpr, context, &eflg);
    if (eflg) {
        prog_error(node, "The first argument to forlist must be a list");
        return InterpError;
    }
    List *list = pvalue.value.uList;
    if (!list) {
        prog_error(node, "The first argument to forlist is in error");
        return InterpError;
    }
    int count = 0;
    InterpType irc;

    for (int i = 0; i < list->length; i++) {
        memcpy(&pvalue, (PValue*) list->data[i], sizeof(PValue));
        assignValueToSymbol(context->symbolTable, node->elementIden, pvalue);
        assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, count++));
        switch (irc = interpret(node->loopState, context, pval)) {
            case InterpContinue:
            case InterpOkay: goto i;
            case InterpBreak: return InterpOkay;
            default: return irc;
        }
    i:  ;
    }
    return InterpOkay;
}
