// DeadEnds
//
// builtinlist.c has the functions that implement the list datatype built-ins. They use the List
// datatype. The elements of the lists are program values.
// MNOTE: Memory management is an issue to be dealt with carefully.
//
// Created by Thomas Wetmore on 16 April 2023.
// Last changed on 29 April 2024.

#include "interp.h"
#include "list.h"

// __list creates a list.
// usage: list(IDENT) -> VOID
PValue __list(PNode* pnode, Context* context, bool* eflg) {
    PNode *var = pnode->arguments;
    if (var->type != PNIdent) {
        scriptError(pnode, "the argument to list must be an identifier");
        *eflg = true;
        return nullPValue;
    }
    String ident = var->identifier;
    ASSERT(ident);
    List *list = createList(null, null, null, false);
    assignValueToSymbol(context->symbolTable, ident, PVALUE(PVList, uList, list));
    return nullPValue;
}

// __push treats the list as a stack and pushes an element onto the front.
// usage: push(LIST, ANY) -> VOID or enqueue(LIST, ANY) -> VOID
PValue __push(PNode* node, Context* context, bool* eflg) {
    PNode *arg = node->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        *eflg = true;
        scriptError(node, "the first first argument to push/enqueue must be a list");
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list);
    pvalue = evaluate(arg->next, context, eflg); // Second arg is a PValue.
    if (*eflg) {
        scriptError(node, "the second argument to push/enqueue must have a program value");
        return nullPValue;
    }
    PValue *ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    prependToList(list, ppvalue);
    return nullPValue;
}

// __requeue adds an element to the back of a list.
// usage: requeue(LIST, ANY) -> VOID
PValue __requeue (PNode* node, Context* context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg); // First arg is a list.
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to requeue must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    pvalue = evaluate(node->arguments->next, context, eflg); // Second arg is a PValue.
    if (*eflg) {
        scriptError(node, "the second argument to requeue must be a program value");
        return nullPValue;
    }
    PValue *ppvalue = (PValue*) malloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    appendToList(list, ppvalue);
    return nullPValue;
}

// __pop treats the list as a stack and pops an element from the front.
// usage: pop(LIST) -> ANY
PValue __pop(PNode* node, Context* context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the argument to pop must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list && lengthList(list) >= 0);
    PValue *ppvalue = (PValue*) popList(list);
    if (!ppvalue) return nullPValue;
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    stdfree(ppvalue);
    return pvalue;
}

// __dequeue treats the list as a queue and dequeues an element from the back.
// usage dequeue(LIST) -> ANY
PValue __dequeue(PNode* node, Context* context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the argument to pop must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list && lengthList(list) >= 0);
    PValue *ppvalue = (PValue*) dequeueList(list);
    if (!ppvalue) return nullPValue;
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    stdfree(ppvalue);
    return pvalue;
}

// __empty  checks if a list is empty.
// usage: empty(LIST) -> BOOL
PValue __empty (PNode* pnode, Context* context, bool* eflg) {
    PValue pvalue = evaluate(pnode->arguments, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(pnode, "the argument to empty is not a list");
        *eflg = true;
        return nullPValue;
    }
    ASSERT(pvalue.value.uList);
    return PVALUE(PVBool, uBool, isEmptyList(pvalue.value.uList));
}

// __getel treats the list as an array and returns the nth element.
// usage: getel(LIST, INT) -> ANY
PValue __getel (PNode *node, Context *context, bool *eflg) {
    PNode *arg = node->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to getel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    pvalue = evaluate(arg->next, context, eflg); // Second argument is an index.
    if (*eflg || pvalue.type != PVInt) {
        scriptError(node, "the second argument to getel must be an integer index");
        *eflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;
    if (index < 0 || index >= lengthList(list)) {
        scriptError(node, "the index to getel is out of range");
        *eflg = true;
        return nullPValue;
    }
    PValue *ppvalue = (PValue*) getListElement(list, index);
    memcpy(&pvalue, ppvalue, sizeof(PValue));
    return pvalue;
}

// __setel treats the list as an array and sets the nth value.
// usage: setel(LIST, INT, ANY) -> VOID
PValue __setel (PNode* node, Context* context, bool* eflg) {
    PNode *arg = node->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to setel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    arg = arg->next; // Second arg is an index.
    pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVInt) {
        scriptError(node, "the second argument to setel must be a integer");
        *eflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;
    if (index < 0) {
        scriptError(node, "the index to setel is out of range");
        *eflg = true;
        return nullPValue;
    }
    // Extend the list with NULLs as needed
    // Patched 27 April 2025 to get setel to work as it should.
    while (index >= lengthList(list)) {
        PValue *filler = (PValue*) stdalloc(sizeof(PValue));
        memcpy(filler, &nullPValue, sizeof(PValue));
        appendToList(list, filler);
    }
    pvalue = evaluate(arg->next, context, eflg); // Third arg is a PValue.
    if (*eflg) {
        scriptError(node, "the third argument to setel is in error");
        return nullPValue;
    }
    PValue *ppvalue = (PValue*) stdalloc(sizeof(PValue));
    memcpy(ppvalue, &pvalue, sizeof(PValue));
    setListElement(list, ppvalue, index);
    return nullPValue;
}

// __length returns the length of a list.
// usage: length(LIST) -> INT
PValue __length(PNode* node, Context* context, bool* eflg) {
    PNode *arg = node->arguments; // Arg is the list.
    PValue pvalue = evaluate(arg, context, eflg);
    if (*eflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to setel must be a list");
        *eflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    return PVALUE(PVInt, uInt, lengthList(list));
}

// interpForList interprets the list loop.
// usage: forlist(LIST, ANY, INT) { BODY }
InterpType interpForList(PNode* node, Context* context, PValue* pval) {
    bool eflg = false;
    PValue pvalue = evaluate(node->listExpr, context, &eflg); // First arg is the list.
    if (eflg) {
        scriptError(node, "The first argument to forlist must be a list");
        return InterpError;
    }
    List *list = pvalue.value.uList;
    if (!list) {
        scriptError(node, "The first argument to forlist is in error");
        return InterpError;
    }
    int count = 0;
    InterpType irc;
	Block* block = &(list->block);
    for (int i = 0; i < block->length; i++) {
        memcpy(&pvalue, (PValue*) block->elements[i], sizeof(PValue));
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
