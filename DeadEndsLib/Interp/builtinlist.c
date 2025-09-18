//
//  DeadEnds Library
//
//  builtinlist.c has the functions that implement the list datatype built-ins. They use the List
//  datatype. The elements of the lists are program values.
//
//  Created by Thomas Wetmore on 16 April 2023.
//  Last changed on 27 July 2025.
//

#include "context.h"
#include "frame.h"
#include "hashtable.h"
#include "interp.h"
#include "list.h"
#include "pnode.h"
#include "pvalue.h"
#include "pvaluelist.h"
#include "sequence.h"
#include "symboltable.h"

static bool localDebugging = false;

// __list creates a list.
// usage: list(IDENT) -> VOID
// TODO: Script Lists are composed of PValues. The Lists will own their elements. So when lists are emptied or
// freed the delete function should free the PValues. This is how we have set things in the List's delete
// function above.
PValue __list(PNode* pnode, Context* context, bool* errflg) {
    PNode *var = pnode->arguments;
    if (var->type != PNIdent) {
        scriptError(pnode, "the argument to list must be an identifier");
        *errflg = true;
        return nullPValue;
    }
    String ident = var->identifier;
    ASSERT(ident);
    List *list = createPValueList();
    assignValueToSymbol(context, ident, PVALUE(PVList, uList, list));
    if (localDebugging) showSymbolTable(context->frame->table);
    return nullPValue;
}

// __prepend handles all the cases that add to the front of a list.
// usage: prepend(LIST, ANY) -> VOID
// usage: push(LIST, ANY) -> VOID
// usage: enqueue(LIST, ANY) -> VOID
PValue __prepend(PNode* pnode, Context* context, bool* errflg) {
    PNode *arg = pnode->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        *errflg = true;
        scriptError(pnode, "the first argument to prepend/push/enqueue must be a list");
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    ASSERT(list);
    pvalue = evaluate(arg->next, context, errflg); // Second arg is a PValue.
    if (*errflg) {
        scriptError(pnode, "the second argument to prepend/push/enqueue must be a program value");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    prependToList(list, ppvalue);
    return nullPValue;
}

// __append handles the cases that add to the end of a list.
// usage: append(LIST, ANY) -> VOID
// usage: requeue(LIST, ANY) -> VOID
PValue __append(PNode* pnode, Context* context, bool* errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg); // First arg is a list.
    if (*errflg || pvalue.type != PVList) {
        scriptError(pnode, "the first argument to append/requeue must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    pvalue = evaluate(pnode->arguments->next, context, errflg); // Second arg is a PValue.
    if (*errflg) {
        scriptError(pnode, "the second argument to append/requeue must be a program value");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    appendToList(list, ppvalue);
    return nullPValue;
}

// __removeFirst treats the list as a stack and removes the first element.
// usage: remfirst(LIST) -> ANY
// usage: pop(LIST) -> ANY
PValue __removeFirst(PNode* pnode, Context* context, bool* errflg) {
    PValue arg = evaluate(pnode->arguments, context, errflg);
    if (*errflg || arg.type != PVList) {
        scriptError(pnode, "the argument to pop/rmvfirst must be a list");
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
PValue __removeLast(PNode* pnode, Context* context, bool* errflg) {
    PValue arg = evaluate(pnode->arguments, context, errflg);
    if (*errflg || arg.type != PVList) {
        scriptError(pnode, "the argument to dequeue/rmvlast must be a list");
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
PValue __setel (PNode* pnode, Context* context, bool* errflg) {
    PNode *arg = pnode->arguments; // First arg is a list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        scriptError(pnode, "the first argument to setel must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = pvalue.value.uList;
    arg = arg->next; // Second arg is an index.
    pvalue = evaluate(arg, context, errflg);
    if (*errflg || pvalue.type != PVInt) {
        scriptError(pnode, "the second argument to setel must be a integer");
        *errflg = true;
        return nullPValue;
    }
    int index = (int) pvalue.value.uInt;
    if (index < 0) {
        scriptError(pnode, "the index to setel is out of range");
        *errflg = true;
        return nullPValue;
    }
    // Extend the list with NULLs as needed
    // Patched 27 April 2025 to get setel to work in crazy places.
    while (index >= lengthList(list)) {
        PValue *filler = (PValue*) stdalloc(sizeof(PValue));
        *filler = nullPValue;
        appendToList(list, filler);
    }
    pvalue = evaluate(arg->next, context, errflg); // Third arg is a PValue.
    if (*errflg) {
        scriptError(pnode, "the third argument to setel is in error");
        return nullPValue;
    }
    PValue *ppvalue = clonePValue(&pvalue);
    setListElement(list, ppvalue, index);
    return nullPValue;
}

// __getel treats the list as an array and returns the nth element.
// usage: getel(LIST, INT) -> ANY
PValue __getel(PNode *pnode, Context *context, bool *errflg) {
    PNode *arg = pnode->arguments;

    // Evaluate first argument: should be a list.
    PValue listVal = evaluate(arg, context, errflg);
    if (*errflg || listVal.type != PVList) {
        scriptError(pnode, "the first argument to getel must be a list");
        *errflg = true;
        return nullPValue;
    }
    List *list = listVal.value.uList;

    // Evaluate second argument: should be an integer index.
    PValue indexVal = evaluate(arg->next, context, errflg);
    if (*errflg || indexVal.type != PVInt) {
        scriptError(pnode, "the second argument to getel must be an integer index");
        *errflg = true;
        return nullPValue;
    }
    int index = (int) indexVal.value.uInt;
    if (index < 0 || index >= lengthList(list)) {
        scriptError(pnode, "the index to getel is out of range");
        *errflg = true;
        return nullPValue;
    }

    // Retrieve and return a copy of the PValue.
    PValue *ppvalue = (PValue*) getListElement(list, index);
    if (!ppvalue) return nullPValue;
    return cloneAndReturnPValue(ppvalue);
}

// __length returns the length of a list, table or sequence.
// usage: length(LIST|TABLE|SEQUENCE) -> INT
// usage: size(LIST|TABLE|SEQUENCE) -> INT
PValue __length(PNode* pnode, Context* context, bool* errflg) {
    PNode *arg = pnode->arguments; // Arg is the list.
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg) return nullPValue;
    switch (pvalue.type) {
    case PVList:
        return PVALUE(PVInt, uInt, lengthList(pvalue.value.uList));
    case PVSequence:
        return PVALUE(PVInt, uInt, lengthSequence(pvalue.value.uSequence));
    case PVTable:
        return PVALUE(PVInt, uInt, sizeHashTable(pvalue.value.uTable));
    default:
        scriptError(pnode, "the argument to length must be a list");
        *errflg = true;
        return nullPValue;
    }
}

// __inlist checks whether a specific PValue is found in a List of PValues.
PValue __inlist(PNode* pnode, Context* context, bool* errflg) {
    PNode* arg = pnode->arguments;
    PValue plist = evaluate(arg, context, errflg);
    if (*errflg) return nullPValue;
    if (plist.type != PVList) {
        scriptError(pnode, "the first argument to inlist must be a list");
        *errflg = true;
        return nullPValue;
    }
    PValue parg = evaluate(arg->next, context, errflg);
    if (*errflg) return nullPValue;
    FORLIST(plist.value.uList, element)
        PValue el = *((PValue*) element);
        if (equalPValues(parg, el)) return truePValue;
    ENDLIST
    return falsePValue;
}

// interpForList interprets the list loop.
// usage: forlist(LIST, ANY, INT) { BODY }
InterpType oldinterpForList(PNode* pnode, Context* context, PValue* pval) {
    bool eflg = false;
    PValue pvalue = evaluate(pnode->listExpr, context, &eflg); // First arg is the list.
    if (eflg) {
        scriptError(pnode, "The first argument to forlist must be a list");
        return InterpError;
    }
    List *list = pvalue.value.uList;
    if (!list) {
        scriptError(pnode, "The first argument to forlist is in error");
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
        assignValueToSymbol(context, pnode->elementIden, copy);
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, count++));
        switch (irc = interpret(pnode->loopState, context, pval)) {
            case InterpContinue:
            case InterpOkay: goto i;
            case InterpBreak: return InterpOkay;
            default: return irc;
        }
    i:  ;
    }
    return InterpOkay;
}
InterpType interpForList(PNode* pnode, Context* context, PValue* pval) {
    bool eflg = false;
    PValue pvalue = evaluate(pnode->listExpr, context, &eflg); // First arg is the list.
    if (eflg) {
        scriptError(pnode, "The first argument to forlist must be a list");
        return InterpError;
    }
    List *list = pvalue.value.uList;
    if (!list) {
        scriptError(pnode, "The first argument to forlist is in error");
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
        assignValueToSymbol(context, pnode->elementIden, copy);
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, count++));
        switch (irc = interpret(pnode->loopState, context, pval)) {
            case InterpContinue:
            case InterpOkay: goto i;
            case InterpBreak: return InterpOkay;
            default: return irc;
        }
    i:  ;
    }
    return InterpOkay;
}



