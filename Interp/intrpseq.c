// DeadEnds
//
// intrpseq.c has the built-in script functions that access the Sequence functions. In the script
// language this datatype is called an indiset.
//
// Created by Thomas Wetmore on 4 March 2023.
// Last changed on 3 May 2024.

#include <stdio.h>
#include "standard.h"
#include "symboltable.h"
#include "gedcom.h"
#include "interp.h"
#include "sequence.h"
#include "list.h"
#include "pvalue.h"
#include "evaluate.h"  // evaluatePerson

// __indiset create a sequence and assigns it to an identifier in a symbol table.
// usage: indiset(IDEN) -> VOID
PValue __indiset(PNode *programNode, Context *context, bool* errorFlag) {
    ASSERT(programNode && programNode->arguments && context);
    PNode *argument = programNode->arguments; // Arg must be an identifier.
    if (argument->type != PNIdent) {
        *errorFlag = true;
        scriptError(programNode, "The argument to indiset must be an identifier.");
        return nullPValue;
    }

    //  Create a new sequence and assign the identifier to it.
    *errorFlag = false;
    assignValueToSymbol(context->symbolTable, argument->identifier,
                        PVALUE(PVSequence, uSequence, createSequence(context->database)));
    return nullPValue;
}

// __addtoset adds a person to a sequence.
// usage: addtoset(SET, INDI, ANY) -> VOID
PValue __addtoset(PNode *programNode, Context *context, bool *errflg) {
    ASSERT(programNode && programNode->arguments && programNode->arguments->next && context);
    PNode *setarg = programNode->arguments; // Arg 1 should be a Sequence.
    PValue pvalue = evaluate(setarg, context, errflg);
    if (*errflg || pvalue.type != PVSequence) {
        *errflg = true;
        scriptError(programNode, "The first argument to addtoset must be a set.");
        return nullPValue;
    }
    Sequence *sequence = pvalue.value.uSequence;

    PNode *indiarg = setarg->next; // Arg 2 should be a person GNode.
    GNode *indi = evaluatePerson(indiarg, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        scriptError(programNode, "The second argument to addtoset must be a person.");
        return nullPValue;
    }
    String key = indi->key; // Get person's key.
    if (!key || *key == 0) {
        *errflg = true;
        scriptError(programNode, "could not get the key of the person.");
        return nullPValue;
    }
    PNode *anyarg = indiarg->next; // Arg 3 can be any PValue.
    PValue value = evaluate(anyarg, context, errflg);
    if (*errflg) {
        scriptError(programNode, "the third argument to addtoset has an error.");
        return nullPValue;
    }
    PValue *ppvalue = allocPValue(value.type, value.value); // Sequence PValues are in the heap.
    if (ppvalue->type == PVString) ppvalue->value.uString = strsave(value.value.uString);
    //  MNOTE: No need to save key--appendToSequence does.
    appendToSequence(sequence, key, ppvalue);
    return nullPValue;
}

//  __lengthset -- Return the length of a sequence.
//    usage: lengthset(SET) -> INT
//--------------------------------------------------------------------------------------------------
PValue __lengthset (PNode *programNode, Context *context, bool *errorFlag)
{
    ASSERT(programNode && programNode->arguments && context);
    PValue val = evaluate(programNode->arguments, context, errorFlag);
    if (*errorFlag || val.type != PVSequence) {
        *errorFlag = true;
        scriptError(programNode, "The arg to lengthset must be a set.");
        return nullPValue;
    }
    Sequence *sequence = val.value.uSequence;
    if (!sequence) return PVALUE(PVInt, uInt, 0);
    return PVALUE(PVInt, uInt, lengthSequence(sequence));
}

//  inset -- See if a person is in a sequence.
//    usage: inset(SET, INDI) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __inset (PNode *programNode, Context *context, bool *eflg)
{
    // Get the sequence argument.
    PNode *arg1 = programNode->arguments;
    PValue value1 = evaluate(arg1, context, eflg);
    if (*eflg || value1.type != PVSequence) {
        *eflg = true;
        scriptError(programNode, "the first argument to inset must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return falsePValue;

    // Get the person argument.
    PNode *arg2 = arg1->next;
    PValue value2 = evaluate(arg2, context, eflg);
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        scriptError(programNode, "the second argument to inset must be a person.");
        return nullPValue;
    }
    GNode *indi = value2.value.uGNode;
    if (!indi) return falsePValue;

    // Both arguments are okay, so search the sequence.
    String key = indi->key;
    //  MNOTE: No need to save key.
    return isInSequence(seq, key) ? truePValue : falsePValue;
}

//  __deletefromset -- Remove a person from a sequence.
//    usage: deletefromset(SET, INDI, BOOL) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __deletefromset (PNode *node, Context *context, bool *eflg)
{
    //  Get the sequence argument.
    PNode *arg1 = node->arguments, *arg2 = arg1->next, *arg3 = arg2->next;
    PValue value1 = evaluate(arg1, context, eflg);
    if (*eflg || value1.type != PVSequence) {
        *eflg = true;
        scriptError(node, "the first argument to deletefromset must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return nullPValue;

    // Get the person argument.
    PValue value2 = evaluate(arg2, context, eflg);
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        scriptError(node, "the second argument to deletefromset must be a person.");
        return nullPValue;
    }
    GNode *indi = value2.value.uGNode;
    if (!indi) return falsePValue;
    //  MNOTE: No need to save key.
    String key = indi->key;
    if (!key || *key == 0) return nullPValue;

    // Get the boolean argument. If true remove all elements with the key, else just the first.
    PValue value3 = evaluateBoolean(arg3, context, eflg);
    if (*eflg || value3.type != PVBool) {
        *eflg = true;
        scriptError(node, "the third argument to deletefromset must be a boolean.");
    }
    bool all = value3.value.uBool;
    bool rc;
    do {
        rc = removeFromSequence(seq, key);
    } while (rc && all);
    return value1;
}

//  __namesort -- Sort a sequence by name.
//    usage: namesort(SET) -> VOID
PValue __namesort(PNode *pnode, Context *context, bool *errflg) {
    PValue value = evaluate(pnode->arguments, context, errflg);
    if (*errflg || value.type != PVSequence) {
        scriptError(pnode, "the argument to namesort must be a set.");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    nameSortSequence(sequence);
    return nullPValue;
}

//  __keysort -- Sort a sequence by key.
//    usage: keysort(SET) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __keysort (PNode *node, Context *context, bool *eflg)
{
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        scriptError(node, "the arg to keysort must be a set.");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    keySortSequence(sequence);
    return nullPValue;
}

//  __valuesort -- Sort a sequence by its value.
//    usage: valuesort(SET) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __valuesort (PNode *node, Context *context, bool *eflg)
{
	scriptError(node, "valuesort has been removed from the script language");
    //PValue value = evaluate(node->arguments, context, eflg);
    //if (*eflg || value.type != PVSequence) {
        //scriptError(node, "the arg to valuesort must be a set.");
        //return nullPValue;
    //}
    //Sequence *sequence = value.value.uSequence;
    //valueSortSequence(sequence);
    return nullPValue;
}

//  __uniqueset -- Eliminate duplicates from a sequence.
//    usage: uniqueset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __uniqueset (PNode *node, Context *context, bool *eflg)
{
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        scriptError(node, "the arg to uniqueset must be a set");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    return PVALUE(PVSequence, uSequence, uniqueSequence(sequence));
}

//  __union -- Create union of two squences.
//    usage: union(SET, SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __union(PNode *node, Context *context, bool *eflg)
{
    // Get the first sequence for the union operation.
    PNode *arg1 = node->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the first argument to union must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the union operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the second arg to union must be a set.");
        return nullPValue;
    }
    Sequence *op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, unionSequence(op1, op2));
    //push_list(keysets, op2);
}

//  __intersect -- Create the intersection of two sequences.
//    usage: intersect(SET, SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __intersect (PNode *node, Context *context, bool *eflg)
{
    // Get the first sequence for the intersection operation.
    PNode *arg1 = node->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the first argument to intersect must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the intersection operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the second arg to intersect must be a set.");
        return nullPValue;
    }
    Sequence *op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, intersectSequence(op1, op2));
    //push_list(keysets, op2);
}

//  __difference -- Create the difference of two sequences.
//    usage: difference(SET, SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __difference(PNode *node, Context *context, bool *eflg)
{
    // Get the first sequence for the difference operation.
    PNode *arg1 = node->arguments, *arg2 = arg1->next;
    PValue val = evaluate(arg1, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the first argument to difference must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the difference operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the second arg to difference must be a set.");
        return nullPValue;
    }
    Sequence *op2 = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, differenceSequence(op1, op2));
    //push_list(keysets, op2);
}

//  __parentset -- Create the parent sequence of a sequence.
//    usage: parentset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __parentset(PNode *node, Context *context, bool *eflg)
{
    PValue val = evaluate(node->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the arg to parentset must be a set.");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, parentSequence(seq));
}

//  __childset -- Create the child sequence of a sequence.
//    usage: childset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __childset(PNode *node, Context *context, bool *eflg)
{
    PValue val = evaluate(node->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the arg to childset must be a set.");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, childSequence(seq));
}

//  __siblingset -- Create sibling sequence of a sequence.
//    usage: siblingset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __siblingset(PNode *node, Context *context, bool *eflg)
{
    PValue val = evaluate(node->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the argument to siblingset must be a set");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, siblingSequence(seq, false));
}

//  __spouseset -- Create spouse sequence of a sequence.
//    usage: spouseset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __spouseset (PNode *node, Context *context, bool *eflg)
{
    PValue val = evaluate(node->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(node, "the argument to spouseset must be a set");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, spouseSequence(seq));
}

// __ancestorset creates the ancestor sequence of a sequence.
// usage: ancestorset(SET) -> SET
PValue __ancestorset (PNode *programNode, Context *context, bool *errorFlag) {
    PValue programValue = evaluate(programNode->arguments, context, errorFlag);
    if (*errorFlag || programValue.type != PVSequence) {
        *errorFlag = true;
        scriptError(programNode, "the argument to ancestorset must be a set.");
        return nullPValue;
    }
    return PVALUE(PVSequence, uSequence, ancestorSequence(programValue.value.uSequence));
}

//  __descendentset -- Create the descendent sequence of a sequence. Two spellings allowed.
//    usage: descendentset(SET) -> SET
//    usage: descendantset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __descendentset (PNode *programNode, Context *context, bool *eflg)
{
    ASSERT(programNode && programNode->arguments && !programNode->arguments->next && context);

    //  The single argument must evaluate to a sequence.
    PValue val = evaluate(programNode->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(programNode, "the arg to descendentset must be a set.");
        return nullPValue;
    }
    return PVALUE(PVSequence, uSequence, descendentSequence(val.value.uSequence));
}
//  __gengedcom -- Generate Gedcom output from a sequence.
//    usage: gengedcom(SET) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __gengedcom(PNode *programNode, Context *context, bool *eflg)
{
    printf("Inside __gengedcom\n");
    ASSERT(programNode && programNode->arguments && !programNode->arguments->next && context);

    //  The argument must evaluate to a sequence.
    PValue val = evaluate(programNode->arguments, context, eflg);
    if (*eflg || val.type != PVSequence) {
        scriptError(programNode, "the argument to gengedcom must be a set");
        return nullPValue;
    }

    //  Generate a Gedcom file from the persons in the sequence.
    sequenceToGedcom(val.value.uSequence, null);  // Null sends to stdout.
    return nullPValue;
}
