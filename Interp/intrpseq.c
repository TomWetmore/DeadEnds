//
//  DeadEnds
//
//  intrpseq.c -- Builit-in functions that give program access the sequence functions. In the
//    programming language this datatype is called an indiset.
//
//  Created by Thomas Wetmore on 4 March 2023.
//  Last changed on 16 November 2023.
//

#include <stdio.h>
#include "standard.h"
#include "symboltable.h"
#include "gedcom.h"
#include "interp.h"
#include "sequence.h"
#include "list.h"
#include "pvalue.h"
#include "evaluate.h"  // evaluatePerson

//  __indiset -- Declare and create a sequence and assign it to an identifier in a symbol table.
//    usage: indiset(IDEN) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __indiset(PNode *programNode, Context *context, bool* errorFlag)
{
    ASSERT(programNode && programNode->arguments && context);

    //  The argument must be an identifier that is assigned to the sequence value.
    PNode *argument = programNode->arguments;
    if (argument->type != PNIdent) {
        *errorFlag = true;
        prog_error(programNode, "The argument to indiset must be an identifier.");
        return nullPValue;
    }

    //  Create a new sequence and assign the identifier to it.
    *errorFlag = false;
    assignValueToSymbol(context->symbolTable, argument->identifier,
                        PVALUE(PVSequence, uSequence, createSequence(context->database)));
    return nullPValue;
}

//  __addtoset -- Add a person to a sequence.
//    usage: addtoset(SET, INDI, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __addtoset(PNode *programNode, Context *context, bool *errflg)
{
    ASSERT(programNode && programNode->arguments && programNode->arguments->next && context);

    // Get the first argument which must be a sequence.
    PNode *setarg = programNode->arguments;
    PValue pvalue = evaluate(setarg, context, errflg);
    if (*errflg || pvalue.type != PVSequence) {
        *errflg = true;
        prog_error(programNode, "The first argument to addtoset must be a set.");
        return nullPValue;
    }
    Sequence *sequence = pvalue.value.uSequence;

    // Get the second argument which must be a person.
    PNode *indiarg = setarg->next;
    GNode *indi = evaluatePerson(indiarg, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(programNode, "The second argument to addtoset must be a person.");
        return nullPValue;
    }

    // Get the person's key.
    String key = indi->key;
    if (!key || *key == 0) {
        *errflg = true;
        prog_error(programNode, "could not get the key of the person.");
        return nullPValue;
    }

    // Get the third argument which can be any program value.
    PNode *anyarg = indiarg->next;
    PValue value = evaluate(anyarg, context, errflg);
    if (*errflg) {
        prog_error(programNode, "the third argument to addtoset has an error.");
        return nullPValue;
    }

    //  Program values in sequences must be kept in the heap.
    PValue *ppvalue = allocPValue(value.type, value.value);
    if (ppvalue->type == PVString) ppvalue->value.uString = strsave(value.value.uString);
    //  MNOTE: No need to save key--appendToSequence does.
    appendToSequence(sequence, key, null, ppvalue);
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
        prog_error(programNode, "The arg to lengthset must be a set.");
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
        prog_error(programNode, "the first argument to inset must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return falsePValue;

    // Get the person argument.
    PNode *arg2 = arg1->next;
    PValue value2 = evaluate(arg2, context, eflg);
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        prog_error(programNode, "the second argument to inset must be a person.");
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
        prog_error(node, "the first argument to deletefromset must be a set.");
        return nullPValue;
    }
    Sequence *seq = value1.value.uSequence;
    if (!seq || lengthSequence(seq) == 0) return nullPValue;

    // Get the person argument.
    PValue value2 = evaluate(arg2, context, eflg);
    if (*eflg || value2.type != PVPerson) {
        *eflg = true;
        prog_error(node, "the second argument to deletefromset must be a person.");
        return nullPValue;
    }
    GNode *indi = value2.value.uGNode;
    if (!indi) return falsePValue;
    //  MNOTE: No need to save key.
    String key = indi->key;
    if (!key || *key == 0) return nullPValue;

    // Get the boolean argument. If true remove all elements with the key, else just the first.
    PValue value3 = evaluate(arg3, context, eflg);
    if (*eflg || value3.type != PVBool) {
        *eflg = true;
        prog_error(node, "the third argument to deletefromset must be a boolean.");
    }
    bool all = value3.value.uBool;
    bool rc;
    do {
        rc = removeFromSequence(seq, key, null, 0);
    } while (rc && all);
    return nullPValue;
}

//  __namesort -- Sort a sequence by name.
//    usage: namesort(SET) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __namesort(PNode *pnode, Context *context, bool *errflg)
{
    PValue value = evaluate(pnode->arguments, context, errflg);
    if (*errflg || value.type != PVSequence) {
        prog_error(pnode, "the argument to namesort must be a set.");
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
        prog_error(node, "the arg to keysort must be a set.");
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
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        prog_error(node, "the arg to valuesort must be a set.");
        return nullPValue;
    }
    Sequence *sequence = value.value.uSequence;
    valueSortSequence(sequence);
    return nullPValue;
}

//  __uniqueset -- Eliminate duplicates from a sequence.
//    usage: uniqueset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __uniqueset (PNode *node, Context *context, bool *eflg)
{
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg || value.type != PVSequence) {
        prog_error(node, "the arg to uniqueset must be a set");
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
        prog_error(node, "the first argument to union must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the union operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        prog_error(node, "the second arg to union must be a set.");
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
        prog_error(node, "the first argument to intersect must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the intersection operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        prog_error(node, "the second arg to intersect must be a set.");
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
        prog_error(node, "the first argument to difference must be a set.");
        return nullPValue;
    }
    Sequence *op1 = val.value.uSequence;

    // Get the second sequence for the difference operatoin.
    val = evaluate(arg2, context, eflg);
    if (*eflg || val.type != PVSequence) {
        prog_error(node, "the second arg to difference must be a set.");
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
        prog_error(node, "the arg to parentset must be a set.");
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
        prog_error(node, "the arg to childset must be a set.");
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
        prog_error(node, "the argument to siblingset must be a set");
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
        prog_error(node, "the argument to spouseset must be a set");
        return nullPValue;
    }
    Sequence *seq = val.value.uSequence;
    return PVALUE(PVSequence, uSequence, spouseSequence(seq));
}

//  __ancestorset -- Create the ancestor sequence of a sequence.
//    usage: ancestorset(SET) -> SET
//--------------------------------------------------------------------------------------------------
PValue __ancestorset (PNode *programNode, Context *context, bool *errorFlag)
{
    PValue programValue = evaluate(programNode->arguments, context, errorFlag);
    if (*errorFlag || programValue.type != PVSequence) {
        *errorFlag = true;
        prog_error(programNode, "the argument to ancestorset must be a set.");
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
        prog_error(programNode, "the arg to descendentset must be a set.");
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
        prog_error(programNode, "the argument to gengedcom must be a set");
        return nullPValue;
    }

    //  Generate a Gedcom file from the persons in the sequence.
    sequenceToGedcom(val.value.uSequence, null);  // Null sends to stdout.
    return nullPValue;
}
