//
//  DeadEnds Library
//
//  intrpseq.c has the built-in script functions that access the Sequence functions. In the script
//  language this datatype is called an indiset. Each builtin calls one of the Sequence functions.
//
//  Created by Thomas Wetmore on 4 March 2023.
//  Last changed on 4 June 2025.
//

#include "context.h"
#include "database.h"
#include "evaluate.h"
#include "gedcom.h"
#include "gnode.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "sequence.h"
#include "standard.h"
#include "symboltable.h"

// __indiset creates a sequence and assigns it to an identifier in a symbol table.
// usage: indiset(IDEN) -> VOID
PValue __indiset(PNode* pnode, Context* context, bool* errorFlag) {
    ASSERT(pnode && pnode->arguments && context);
    PNode *arg = pnode->arguments; // Ident.
    if (arg->type != PNIdent) {
        *errorFlag = true;
        scriptError(pnode, "The argument to indiset must be an identifier.");
        return nullPValue;
    }
    *errorFlag = false;
    assignValueToSymbol(context, arg->identifier,
                        PVALUE(PVSequence, uSequence, createSequence(context->database->recordIndex)));
    return nullPValue;
}

// __addtoset adds a person to a sequence.
// usage: addtoset(SET, INDI, ANY) -> VOID
PValue __addtoset(PNode *pnode, Context *context, bool *errflg) {
    ASSERT(pnode && pnode->arguments && pnode->arguments->next && context);
    PNode *setarg = pnode->arguments; // Sequence.
    PValue pvalue = evaluate(setarg, context, errflg);
    if (*errflg || pvalue.type != PVSequence) {
        *errflg = true;
        scriptError(pnode, "The first argument to addtoset must be a set.");
        return nullPValue;
    }
    Sequence *sequence = pvalue.value.uSequence;
    PNode *indiarg = setarg->next; // GNode.
    GNode *indi = evaluatePerson(indiarg, context, errflg);
    if (*errflg) {
        scriptError(pnode, "second argument to addtoset() must be a person.");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    String key = indi->key; // Person key.
    if (!key || *key == 0) {
        *errflg = true;
        scriptError(pnode, "could not get the key of the person.");
        return nullPValue;
    }
    PNode *anyarg = indiarg->next; // PValue.
    PValue value = evaluate(anyarg, context, errflg);
    if (*errflg) {
        scriptError(pnode, "the third argument to addtoset has an error.");
        return nullPValue;
    }
    PValue *ppvalue = allocPValue(value.type, value.value); // Sequence PValues are in the heap.
    if (ppvalue->type == PVString) ppvalue->value.uString = strsave(value.value.uString);
    appendToSequence(sequence, key, ppvalue);
    return nullPValue;
}

// __lengthset returns the length of a sequence.
// usage: lengthset(SET) -> INT
PValue __lengthset(PNode* pnode, Context* context, bool* errorFlag) {
    ASSERT(pnode && pnode->arguments && context);
    PValue val = evaluate(pnode->arguments, context, errorFlag);
    if (*errorFlag || val.type != PVSequence) {
        *errorFlag = true;
        scriptError(pnode, "the arg to lengthset must be a set.");
        return nullPValue;
    }
    Sequence *sequence = val.value.uSequence;
    if (!sequence) return PVALUE(PVInt, uInt, 0);
    return PVALUE(PVInt, uInt, lengthSequence(sequence));
}

// __inset checks if a person is in a sequence.
// usage: inset(SET, INDI) -> BOOL
PValue __inset(PNode* pnode, Context* context, bool* eflg) {
    PNode *arg1 = pnode->arguments;
    PValue value1 = evaluate(arg1, context, eflg); // Sequence.
    if (*eflg || value1.type != PVSequence) {
        *eflg = true;
        scriptError(pnode, "first argument to inset() must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return falsePValue;
    PNode *arg2 = arg1->next; // Person.
    PValue value2 = evaluate(arg2, context, eflg);
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        scriptError(pnode, "second argument to inset() must be a person.");
        return nullPValue;
    }
    GNode *indi = value2.value.uGNode;
    if (!indi) return falsePValue;
    String key = indi->key;
    return isInSequence(seq, key) ? truePValue : falsePValue;
}

// __deletefromset removes a person from a sequence.
// usage: deletefromset(SET, INDI, BOOL) -> VOID
PValue __deletefromset(PNode* pnode, Context* context, bool* eflg) {
    PNode *arg1 = pnode->arguments, *arg2 = arg1->next, *arg3 = arg2->next;
    PValue value1 = evaluate(arg1, context, eflg); // Sequence.
    if (*eflg || value1.type != PVSequence) {
        *eflg = true;
        scriptError(pnode, "the first argument to deletefromset must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return nullPValue;
    PValue value2 = evaluate(arg2, context, eflg); // Person.
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        scriptError(pnode, "the second argument to deletefromset must be a person.");
        return nullPValue;
    }
    GNode *indi = value2.value.uGNode;
    if (!indi) return falsePValue;
    String key = indi->key;
    if (!key || *key == 0) return nullPValue;
    PValue value3 = evaluateBoolean(arg3, context, eflg); // Remove all with key?
    if (*eflg || value3.type != PVBool) {
        *eflg = true;
        scriptError(pnode, "the third argument to deletefromset must be a boolean.");
    }
    bool all = value3.value.uBool;
    bool rc;
    do {
        rc = removeFromSequence(seq, key);
    } while (rc && all);
    return value1;
}

// __namesort sorts a sequence by name.
// usage: namesort(SET) -> VOID
PValue __namesort(PNode* pnode, Context* context, bool* errflg) {
    PValue value = evaluate(pnode->arguments, context, errflg);
    if (*errflg || value.type != PVSequence) {
        scriptError(pnode, "the argument to namesort must be a set.");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    nameSortSequence(sequence);
    return nullPValue;
}

// __keysort sorts a sequence by key.
// usage: keysort(SET) -> VOID
PValue __keysort(PNode* pnode, Context* context, bool* eflg)
{
    PValue value = evaluate(pnode->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        scriptError(pnode, "the arg to keysort must be a set.");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    keySortSequence(sequence);
    return nullPValue;
}

// __valuesort sort a sequence by its value.
// usage: valuesort(SET) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __valuesort(PNode* pnode, Context* context, bool* eflg)
{
	scriptError(pnode, "valuesort has been removed from the script language");
    //PValue value = evaluate(node->arguments, context, eflg);
    //if (*eflg || value.type != PVSequence) {
        //scriptError(pnode, "the arg to valuesort must be a set.");
        //return nullPValue;
    //}
    //Sequence *sequence = value.value.uSequence;
    //valueSortSequence(sequence);
    return nullPValue;
}

// __uniqueset removes duplicates from a sequence.
// usage: uniqueset(SET) -> SET
PValue __uniqueset(PNode* pnode, Context* context, bool* eflg) {
    PValue value = evaluate(pnode->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        scriptError(pnode, "the arg to uniqueset must be a set");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    return PVALUE(PVSequence, uSequence, uniqueSequence(sequence));
}

// __union creates the union of two sequences.
// usage: union(SET, SET) -> SET
PValue __union(PNode* pnode, Context* context, bool* eflg) {
    PNode *arg1 = pnode->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg); // Sequence one.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the first argument to union must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;
    val = evaluate(arg2, context, eflg); // Sequence two.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the second arg to union must be a set.");
        return nullPValue;
    }
    Sequence *op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, unionSequence(op1, op2));
}

// __intersect creates the intersection of two sequences.
// usage: intersect(SET, SET) -> SET
PValue __intersect(PNode* pnode, Context* context, bool* eflg) {
    PNode *arg1 = pnode->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg); // Sequence one.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the first argument to intersect must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;
    val = evaluate(arg2, context, eflg); // Sequence two.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the second arg to intersect must be a set.");
        return nullPValue;
    }
    Sequence *op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, intersectSequence(op1, op2));
}

// __difference create the difference of two sequences.
// usage: difference(SET, SET) -> SET
PValue __difference(PNode* pnode, Context* context, bool* eflg) {
    PNode* arg1 = pnode->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg); // Sequence one.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the first argument to difference must be a set.");
        return nullPValue;
    }
    Sequence* op1 = val.value.uSequence;
    val = evaluate(arg2, context, eflg); // Sequence two.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the second arg to difference must be a set.");
        return nullPValue;
    }
    Sequence* op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, differenceSequence(op1, op2));
}

// __parentset creates the parent sequence of a sequence.
// usage: parentset(SET) -> SET
PValue __parentset(PNode* pnode, Context* context, bool* eflg) {
    PValue val = evaluate(pnode->arguments, context, eflg); // Sequence
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the arg to parentset must be a set.");
        return nullPValue;
    }
    Sequence* seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, parentSequence(seq));
}

// __childset create the children sequence of a sequence.
// usage: childset(SET) -> SET
PValue __childset(PNode* pnode, Context* context, bool* eflg) {
    PValue val = evaluate(pnode->arguments, context, eflg); // Sequence
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the arg to childset must be a set.");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, childSequence(seq));
}

// __siblingset creates the sibling sequence of a sequence.
// usage: siblingset(SET) -> SET
PValue __siblingset(PNode *node, Context *context, bool *eflg) {
    PValue val = evaluate(node->arguments, context, eflg); // Sequence
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the argument to siblingset must be a set");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, siblingSequence(seq, false));
}

// __spouseset create spouse sequence of a sequence.
// usage: spouseset(SET) -> SET
PValue __spouseset(PNode* pnode, Context* context, bool* eflg) {
    PValue val = evaluate(pnode->arguments, context, eflg); // Sequence
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the argument to spouseset must be a set");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, spouseSequence(seq));
}

// __ancestorset creates the ancestor sequence of a sequence.
// usage: ancestorset(SET) -> SET
PValue __ancestorset(PNode* pnode, Context* context, bool* errflag) {
    PValue programValue = evaluate(pnode->arguments, context, errflag);
    if (*errflag || programValue.type != PVSequence) {
        *errflag = true;
        scriptError(pnode, "the argument to ancestorset must be a set.");
        return nullPValue;
    }
    return PVALUE(PVSequence, uSequence, ancestorSequence(programValue.value.uSequence, false));
}

// __descendentset creates the descendent sequence of a sequence; two spellings allowed.
// usage: descendentset(SET) -> SET or descendantset(SET) -> SET
PValue __descendentset(PNode* pnode, Context* context, bool* eflg) {
    ASSERT(pnode && pnode->arguments && !pnode->arguments->next && context);
    PValue val = evaluate(pnode->arguments, context, eflg); // Sequence.
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the arg to descendentset must be a set.");
        return nullPValue;
    }
    return PVALUE(PVSequence, uSequence, descendentSequence(val.value.uSequence, false));
}
// __gengedcom -- Generate Gedcom output from a sequence.
// usage: gengedcom(SET) -> VOID
PValue __gengedcom(PNode *pnode, Context *context, bool *eflg) {
    printf("Inside __gengedcom\n");
    ASSERT(pnode && pnode->arguments && !pnode->arguments->next && context);

    //  The argument must evaluate to a sequence.
    PValue val = evaluate(pnode->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(pnode, "the argument to gengedcom must be a set");
        return nullPValue;
    }

    //  Generate a Gedcom file from the persons in the sequence.
    sequenceToGedcom(val.value.uSequence, null);  // Null sends to stdout.
    return nullPValue;
}
