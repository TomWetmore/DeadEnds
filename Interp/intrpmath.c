//
//  DeadEnds
//
//  intrpmath.c -- Arithmetic and logic built-in functions.
//
//  Created by Thomas Wetmore on 17 March 2023.
//  Last changed on 16 November 2023.
//

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"
#include "interp.h"

// Local helper function.
//--------------------------------------------------------------------------------------------------
static void evalBinary(PNode *pnode, Context *context, PValue* val1, PValue* val2, bool* eflag);

// __add -- Built-in add operation, takes one to 32 arguments. Supports both integer (C long)
//    and floating point (C double) and mixed operands.
//    usage: add((INT|FLOAT) [,(INT|FLOAT)]+) -> (INT|FLOAT)
//--------------------------------------------------------------------------------------------------
PValue __add(PNode *pnode, Context *context, bool* eflg)
{
    PVType currentType = PVInt;  // Integer until the first floating argument appears.
    long intSum = 0;
    double floatSum = 0;
    PValue value;

    // Loop through the arguments.
    for (PNode *arg = pnode->arguments; arg; arg = arg->next) {
        value = evaluate(arg, context, eflg);
        if (*eflg || (value.type != PVInt && value.type != PVFloat)) return nullPValue;

        // Allow mixed integer and floating point operands.
        if (currentType == PVInt && value.type == PVInt)
            intSum += value.value.uInt;
        else if (currentType == PVFloat && value.type == PVFloat)
            floatSum += value.value.uFloat;
        else if (currentType == PVFloat && value.type == PVInt)
            floatSum += (double) value.value.uInt;
        else if (currentType == PVInt && value.type == PVFloat) {
            currentType = PVFloat;
            floatSum = (double) intSum + value.value.uFloat;
        } else
            return nullPValue;
    }
    value.type = currentType;
    if (currentType == PVInt) value.value.uInt = intSum;
    else value.value.uFloat = floatSum;
    return value;
}

//  __sub -- Subtract builtin function.
//    usage: sub(INT, INT) -> INT
//           sub(FLOAT, FLOAT) -> FLOAT
//--------------------------------------------------------------------------------------------------
PValue __sub(PNode *node, Context *context, bool *eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : subPValues(val1, val2, eflg);
}

//  __mul -- Multiply operation
//    usage: mul(INT|FLOAT [,INT|FLOAT]+) -> INT|FLOAT
//--------------------------------------------------------------------------------------------------
PValue __mul(PNode *pnode, Context *context, bool *eflg)
{
    PVType currentType = PVInt;  //  Integer until any float argument is encountered.
    long intProd = 1;
    double floatProd = 1.0;
    PValue value = (PValue){};
    
    // Loop through the arguments.
    for (PNode *arg = pnode->arguments; arg; arg = arg->next) {
        value = evaluate(arg, context, eflg);
        if (*eflg || (value.type != PVInt && value.type != PVFloat)) return nullPValue;

        // Mixed integer and floating operands are okay.
        if (currentType == PVInt && value.type == PVInt)
            intProd *= value.value.uInt;
        else if (currentType == PVFloat && value.type == PVFloat)
            floatProd *= value.value.uFloat;
        else if (currentType == PVFloat && value.type == PVInt)
            floatProd *= (double) value.value.uInt;
        else if (currentType == PVInt && value.type == PVFloat) {
            currentType = PVFloat;
            floatProd = (double) intProd * value.value.uFloat;
        } else
            return nullPValue;
    }
    value.type = currentType;
    if (currentType == PVInt) value.value.uInt = intProd;
    else value.value.uFloat = floatProd;
    return value;
}

//  __div -- Divide builtin function.
//    usage: div(INT, INT) -> INT
//           div(FLOAT, FLOAT) -> FLOAT
//--------------------------------------------------------------------------------------------------
PValue __div(PNode *node, Context *context, bool* eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : divPValues(val1, val2, eflg);
}

//  __mod -- Modulus builtin function.
//    usage: mod(INT, INT) -> INT
//--------------------------------------------------------------------------------------------------
PValue __mod(PNode *node, Context *context, bool* eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : modPValues(val1, val2, eflg);
}

//  __exp -- Exponentiation operation
//    usage: exp(INT, INT) -> INT
//--------------------------------------------------------------------------------------------------
PValue __exp (PNode *node, Context *context, bool* eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : expPValues(val1, val2, eflg);
}

//  __neg -- Negation operation
//    usage: neg(INT) -> INT
//--------------------------------------------------------------------------------------------------
PValue __neg (PNode *node, Context *context, bool* eflg)
{
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg) return nullPValue;
    return negPValue(value, eflg);
}

//  __incr -- Increment variable. The variable must have an integer value.
//    usage: incr(VARB) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __incr(PNode* pnode, Context *context, bool* errflg)
{
    // Get the identifier.
    pnode = pnode->arguments;
    *errflg = true;  // Guilty.
    if (pnode->type != PNIdent) return nullPValue;
    String ident = pnode->identifier;
    if (!ident)  return nullPValue;

    // Make sure the identifier has an integer value.
    PValue pvalue = getValueOfSymbol(context->symbolTable, ident);
    if (pvalue.type != PVInt) return nullPValue;

    // Increment the value of the identifier.
    *errflg = false;  // Innocent.
    pvalue.value.uInt += 1;
    assignValueToSymbol(context->symbolTable, ident, pvalue);
    return nullPValue;  // Increment does not return a value.
}

//  __decr -- Decrement variable
//    usage: decr(VARB) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __decr(PNode *pnode, Context *context, bool* errflg)
{
    // Get the identifier.
    pnode = pnode->arguments;
    *errflg = true;  // Guilty.
    if (pnode->type != PNIdent) return nullPValue;
    String ident = pnode->identifier;
    if (!ident) return nullPValue;

    // Make sure the identifier has an integer value.
    PValue pvalue = getValueOfSymbol(context->symbolTable, ident);
    if (pvalue.type != PVInt) return nullPValue;

    // Decrement the value of the identifier.
    *errflg = false;  // Innocent.
    pvalue.value.uInt -= 1;
    assignValueToSymbol(context->symbolTable, ident, pvalue);
    return nullPValue;  // Decrement does not return a value.
}

//  __eq -- Equal operation
//    usage: eq(NUMERIC, NUMERIC) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __eq(PNode *pnode, Context *context, bool* errflg)
{
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt == val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat == val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __ne -- Not equal operation
//    usage: ne(NUMERIC, NUMERIC) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __ne(PNode* pnode, Context *context, bool* errflg)
{
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt != val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat != val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __le -- Less or equal operation
//    usage: le(ANY, ANY) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __le(PNode *node, Context *context, bool* eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    // DEBUG
    //printf("Back from evalBinary: val1: %ld, val2: %ld", val1.pvValue.uInt, val2.pvValue.uInt);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt <= val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat <= val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __ge -- Greater or equal operation
//    usage: ge(ANY, ANY) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __ge(PNode* node, Context *context, bool* eflg)
{
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt >= val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat >= val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __lt -- Less than operation
//    usage: lt(NUMERIC,NUMERIC) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __lt(PNode *pnode, Context *context, bool* errflg)
{
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt < val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat < val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __gt -- Greater than operation
//    usage: gt(NUMERIC, NUMERIC) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __gt(PNode *node, Context *context, bool* eflg)
{
    PValue val1, val2;
    //showSymbolTable(stab);
    evalBinary(node, context, &val1, &val2, eflg);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt > val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat > val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

//  __and -- Logical and operation.
//    usage: and(ANY [,ANY]+) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __and(PNode *node, Context *context, bool* eflg)
{
    PNode *arg = node->arguments;
    PValue pvalue = evaluateBoolean(arg, context, eflg);
    if (*eflg) return nullPValue;
    bool bvalue = pvalue.value.uBool;
    while (bvalue && (arg = arg->next)) {
        pvalue = evaluateBoolean(arg, context, eflg);
        if (*eflg) return nullPValue;
        bvalue = bvalue && pvalue.value.uBool;
        if (!bvalue) return falsePValue;
    }
    return bvalue ? truePValue : falsePValue;
}

//  __or -- Or operation
//    usage: or(ANY [,ANY]+) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __or (PNode *node, Context *context, bool* eflg)
{
    PNode *arg = node->arguments;
    PValue pvalue = evaluateBoolean(arg, context, eflg);
    if (*eflg) return nullPValue;
    bool bvalue = pvalue.value.uBool;
    while (!bvalue && (arg = arg->next)) {
        pvalue = evaluateBoolean(arg, context, eflg);
        if (*eflg) return nullPValue;
        bvalue = bvalue || pvalue.value.uBool;
    }
    return bvalue ? truePValue : falsePValue;
}

//  evalBinary -- Evaluate a binary expression.
//--------------------------------------------------------------------------------------------------
static void evalBinary(PNode *pnode, Context *context, PValue* val1, PValue* val2, bool* eflag)
//  pnode -- Program node of a function or builtin call that takes two numeric arguments.
//  stab -- Symbol table to lookup identifiers if needed.
//  val1 -- (output) Program value of the evaluated first argument.
//  val2 -- (output) Program value of the evaluated second argument.
//  eflag -- (output) Error flag.
{
    extern bool numericPValue(PValue pvalue);

    // Get the two arguments as program nodes.
    PNode *arg1 = pnode->arguments;
    if (!arg1) { *eflag = true; return; }
    PNode *arg2 = arg1->next;
    if (!arg2) { *eflag = true; return; }

    // Evaluate the two arguments into PValues.
    PValue pval1 = evaluate(arg1, context, eflag);
    PValue pval2 = evaluate(arg2, context, eflag);

    // The two PValues must have the same type and be numeric.
    if (pval1.type != pval2.type || !numericPValue(pval1)) {
        *eflag = true;
        return;
    }
    *val1 = pval1;
    *val2 = pval2;
}
