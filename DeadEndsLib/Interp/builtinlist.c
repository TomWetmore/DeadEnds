// DeadEnds
//
// builtinlist.c has the functions that implement the list datatype built-ins. They use the List
// datatype. The elements of the lists are program values.
// MNOTE: Memory management is an issue to be dealt with carefully.
//
// Created by Thomas Wetmore on 16 April 2023.
// Last changed on 13 May 2024.

#include "interp.h"
#include "list.h"
#include "pvalue.h"

static bool localDebugging = false;

// __list creates a list.
// usage: list(IDENT) -> VOID
static void delete(void* element) { stdfree(element); } // Free strings when list freed.
PValue __list(PNode* pnode, Context* context, bool* errflg) {
    PNode *var = pnode->arguments;
    if (var->type != PNIdent) {
        scriptError(pnode, "the argument to list must be an identifier");
        *errflg = true;
        return nullPValue;
    }
    String ident = var->identifier;
    ASSERT(ident);
    List *list = createList(null, null, delete, false);
    assignValueToSymbol(context->symbolTable, ident, PVALUE(PVList, uList, list));
    if (localDebugging) showSymbolTable(context->symbolTable);
    return nullPValue;
}

// __prepend handles all the cases that add to the front of a list.
// usage: prepend(LIST, ANY) -> VOID
// usage: push(LIST, ANY) -> VOID
// usage: enqueue(LIST, ANY) -> VOID
PValue __prepend(PNode* node, Context* context, bool* errflg) {
    PNode *arg = node->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        *errflg = true;
        scriptError(node, "the first argument to prepend/push/enqueue must be a list");
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list);
    pvalue = evaluate(arg->next, context, errflg); // Second arg is a PValue.
    if (*errflg) {
        scriptError(node, "the second argument to prepend/push/enqueue must be a program value");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    prependToList(list, ppvalue);
    return nullPValue;
}

// __append handle the cases that add to the end of a list.
// usage: append(LIST, ANY) -> VOID
// usage: requeue(LIST, ANY) -> VOID
PValue __append(PNode* node, Context* context, bool* errflg) {
    PValue pvalue = evaluate(node->arguments, context, errflg); // First arg is a list.
    if (*errflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to append/requeue must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    pvalue = evaluate(node->arguments->next, context, errflg); // Second arg is a PValue.
    if (*errflg) {
        scriptError(node, "the second argument to append/requeue must be a program value");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    appendToList(list, ppvalue);
    return nullPValue;
}

// __removeFirst treats the list as a stack and removes the first element.
// usage: remfirst(LIST) -> ANY
// usage: pop(LIST) -> ANY
PValue __removeFirst(PNode* node, Context* context, bool* errflg) {
    PValue arg = evaluate(node->arguments, context, errflg);
    if (*errflg || arg.type != PVList) {
        scriptError(node, "the argument to pop/rmvfirst must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = arg.value.uList;
    ASSERT(list);
    PValue *ppvalue = (PValue*) popList(list);
    if (!ppvalue) return nullPValue;
    PValue result = cloneAndReturnPValue(ppvalue);
    freePValue(ppvalue);
    return result;
}

// __removeLast treats the list as a queue and removes the last element.
// usage: removelast(LIST) -> ANY
// usage: dequeue(LIST) -> ANY
PValue __removeLast(PNode* node, Context* context, bool* errflg) {
    PValue arg = evaluate(node->arguments, context, errflg);
    if (*errflg || arg.type != PVList) {
        scriptError(node, "the argument to dequeue/rmvlast must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = arg.value.uList;
    ASSERT(list);
    PValue *ppvalue = (PValue*) dequeueList(list);
    if (!ppvalue) return nullPValue;
    PValue result = cloneAndReturnPValue(ppvalue);
    freePValue(ppvalue);
    return result;
}

// __empty  checks if a list is empty.
// usage: empty(LIST) -> BOOL
PValue __empty (PNode* pnode, Context* context, bool* errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        scriptError(pnode, "the argument to empty is not a list");
        *errflg = true;
        return nullPValue;
    }
    ASSERT(pvalue.value.uList);
    return PVALUE(PVBool, uBool, isEmptyList(pvalue.value.uList));
}

// __setel treats the list as an array and sets the nth value. If the index is outside the range of the
// current "array" the array is expanded and the unused element set to the nullPValue.
// usage: setel(LIST, INT, ANY) -> VOID
PValue __setel (PNode* node, Context* context, bool* errflg) {
    PNode *arg = node->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to setel must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    arg = arg->next; // Second arg is an index.
    pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVInt) {
        scriptError(node, "the second argument to setel must be a integer");
        *errflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;
    if (index < 0) {
        scriptError(node, "the index to setel is out of range");
        *errflg = true;
        return nullPValue;
    }
    // Extend the list with NULLs as needed
    // Patched 27 April 2025 to get setel to work as it should.
    while (index >= lengthList(list)) {
        PValue *filler = (PValue*) stdalloc(sizeof(PValue));
        *filler = nullPValue;
        appendToList(list, filler);
    }
    pvalue = evaluate(arg->next, context, errflg); // Third arg is a PValue.
    if (*errflg) {
        scriptError(node, "the third argument to setel is in error");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    setListElement(list, ppvalue, index);
    return nullPValue;
}

// __getel treats the list as an array and returns the nth element.
// usage: getel(LIST, INT) -> ANY
PValue __getel(PNode *node, Context *context, bool *errflg) {
    PNode *arg = node->arguments;

    // Evaluate first argument: should be a list.
    PValue listVal = evaluate(arg, context, errflg);
    if (*errflg || listVal.type != PVList) {
        scriptError(node, "the first argument to getel must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = listVal.value.uList;

    // Evaluate second argument: should be an integer index.
    PValue indexVal = evaluate(arg->next, context, errflg);
    if (*errflg || indexVal.type != PVInt) {
        scriptError(node, "the second argument to getel must be an integer index");
        *errflg = true;
        return nullPValue;
    }
    int index = (int) indexVal.value.uInt;
    if (index < 0 || index >= lengthList(list)) {
        scriptError(node, "the index to getel is out of range");
        *errflg = true;
        return nullPValue;
    }

    // Retrieve and return a copy of the PValue.
    PValue *ppvalue = (PValue*) getListElement(list, index);
    if (!ppvalue) return nullPValue;
    return cloneAndReturnPValue(ppvalue);
}

// __length returns the length of a list.
// usage: length(LIST) -> INT
PValue __length(PNode* node, Context* context, bool* errflg) {
    PNode *arg = node->arguments; // Arg is the list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        scriptError(node, "the first argument to setel must be a list");
        *errflg = true;
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
        PValue* fromList = (PValue*) block->elements[i];
        PValue copy = *fromList;
        if (copy.type == PVString && copy.value.uString)
            copy.value.uString = strsave(copy.value.uString);  // deep copy

        assignValueToSymbol(context->symbolTable, node->elementIden, copy);
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
