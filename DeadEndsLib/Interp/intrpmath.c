//
//  DeadEnds
//
//  intrpmath.c has the built-in script functions for math and logic.
//
//  Created by Thomas Wetmore on 17 March 2023.
//  Last changed on 3 June 2025.
//

#include "context.h"
#include "evaluate.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "standard.h"
#include "symboltable.h"

// evalBinary is a local function that ...
static void evalBinary(PNode* pnode, Context* context, PValue* val1, PValue* val2, bool* eflag);

// __add is the builtin add operation, taking up to 32 arguments. It supports integer and floats.
// The result is an integer only if all operands are integers.
// usage: add((INT|FLOAT) [,(INT|FLOAT)]+) -> (INT|FLOAT)
PValue __add(PNode* pnode, Context* context, bool* eflg) {
    PVType currentType = PVInt;
    long intSum = 0;
    double floatSum = 0;
    PValue value;
    for (PNode *arg = pnode->arguments; arg; arg = arg->next) { // Arg loop.
        value = evaluate(arg, context, eflg);
        if (*eflg || (value.type != PVInt && value.type != PVFloat)) return nullPValue;
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

// __eq -- Compare two values for equality.
// usage: eq(ANY, ANY) -> BOOL
PValue __chatGPTeq(PNode* node, Context* context, bool* eflg) {
    PValue a = evaluate(node->arguments, context, eflg);
    if (*eflg) return nullPValue;

    PValue b = evaluate(node->arguments->next, context, eflg);
    if (*eflg) return nullPValue;

    // Handle nulls gracefully
    if (a.type == PVNull && b.type == PVNull) return PVALUE(PVBool, uBool, true);
    if (a.type == PVNull) {
        if (b.type == PVInt)    return PVALUE(PVBool, uBool, b.value.uInt == 0);
        if (b.type == PVString) return PVALUE(PVBool, uBool, b.value.uString == NULL || b.value.uString[0] == '\0');
        return PVALUE(PVBool, uBool, false);
    }
    if (b.type == PVNull) {
        if (a.type == PVInt)    return PVALUE(PVBool, uBool, a.value.uInt == 0);
        if (a.type == PVString) return PVALUE(PVBool, uBool, a.value.uString == NULL || a.value.uString[0] == '\0');
        return PVALUE(PVBool, uBool, false);
    }

    // If both are ints
    if (a.type == PVInt && b.type == PVInt)
        return PVALUE(PVBool, uBool, a.value.uInt == b.value.uInt);

    // If both are strings
    if (a.type == PVString && b.type == PVString)
        return PVALUE(PVBool, uBool, strcmp(a.value.uString, b.value.uString) == 0);

    // Mixed int/string comparison
    if (a.type == PVInt && b.type == PVString) {
        return PVALUE(PVBool, uBool, a.value.uInt == atoi(b.value.uString));
    }
    if (a.type == PVString && b.type == PVInt) {
        return PVALUE(PVBool, uBool, atoi(a.value.uString) == b.value.uInt);
    }

    // For other types (e.g., GNode, List), require exact match by pointer
    return PVALUE(PVBool, uBool, false);
}

// __sub is the builtin subtract function; operands must be both int or both float.
// usage: sub(INT, INT) -> INT or sub(FLOAT, FLOAT) -> FLOAT
PValue __sub(PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : subPValues(val1, val2, eflg);
}

// __mul is the builtin multipy function, taking up to 32 args. The result is an int only
// if all args are ints.
// usage: mul(INT|FLOAT [,INT|FLOAT]+) -> INT|FLOAT
PValue __mul(PNode* pnode, Context* context, bool *eflg) {
    PVType currentType = PVInt;
    long intProd = 1;
    double floatProd = 1.0;
    PValue value = (PValue){};
    for (PNode *arg = pnode->arguments; arg; arg = arg->next) { // Arg loop.
        value = evaluate(arg, context, eflg);
        if (*eflg || (value.type != PVInt && value.type != PVFloat)) return nullPValue;
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

// __div is the builtin division function. The args must be either int or float.
// usage: div(INT, INT) -> INT and div(FLOAT, FLOAT) -> FLOAT
PValue __div(PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : divPValues(val1, val2, eflg);
}

// __mod is the builtin modulus function. Both args must be ints.
// usage: mod(INT, INT) -> INT
PValue __mod(PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : modPValues(val1, val2, eflg);
}

// __exp is the builtin exponentiation operation. Both args must be ints.
// usage: exp(INT, INT) -> INT
PValue __exp (PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    return *eflg ? nullPValue : expPValues(val1, val2, eflg);
}

// __neg is the negation operator.
// usage: neg(INT) -> INT
PValue __neg (PNode* node, Context* context, bool* eflg) {
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg) return nullPValue;
    return negPValue(value, eflg);
}

// __incr increments an integer variable.
// usage: incr(VARB) -> VOID
PValue __incr(PNode* pnode, Context* context, bool* errflg) {
    pnode = pnode->arguments; // Get ident.
    *errflg = true;
    if (pnode->type != PNIdent) return nullPValue;
    String ident = pnode->identifier;
    if (!ident)  return nullPValue;
    PValue pvalue = getValueOfSymbol(context, ident);
    if (pvalue.type != PVInt) return nullPValue;
    *errflg = false;
    pvalue.value.uInt += 1;
    assignValueToSymbol(context, ident, pvalue);
    return nullPValue;
}

// __decr decrements an integer variable.
// usage: decr(VARB) -> VOID
PValue __decr(PNode* pnode, Context* context, bool* errflg) {
    pnode = pnode->arguments; // Get ident.
    *errflg = true;
    if (pnode->type != PNIdent) return nullPValue;
    String ident = pnode->identifier;
    if (!ident) return nullPValue;
    PValue pvalue = getValueOfSymbol(context, ident);
    if (pvalue.type != PVInt) return nullPValue;
    *errflg = false;
    pvalue.value.uInt -= 1;
    assignValueToSymbol(context, ident, pvalue);
    return nullPValue;
}

// __eq checks for equality between two numeric Pvalues.
// usage: eq(NUMERIC, NUMERIC) -> BOOL
PValue __eq(PNode* pnode, Context* context, bool* errflg) {
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt == val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat == val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
	return result ? truePValue : falsePValue;
}

// __ne checks for nonequality between two numeric PValues.
// usage: ne(NUMERIC, NUMERIC) -> BOOL
PValue __ne(PNode* pnode, Context* context, bool* errflg) {
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt != val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat != val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

// __le checks the less than or equal relation between two numeric PValues.
// usage: le(ANY, ANY) -> BOOL
PValue __le(PNode *node, Context *context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt <= val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat <= val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

// __ge checks the greater than or equal relation between two numeric PValues.
// usage: ge(ANY, ANY) -> BOOL
PValue __ge(PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt >= val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat >= val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

// __lt checks the less than relation between two numeric PValues.
// usage: lt(NUMERIC,NUMERIC) -> BOOL
PValue __lt(PNode* pnode, Context* context, bool* errflg) {
    PValue val1, val2;
    evalBinary(pnode, context, &val1, &val2, errflg);
    if (*errflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt < val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat < val2.value.uFloat;
    else { *errflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

// __gt checks the greater than relation between two numeric PValues.
// usage: gt(NUMERIC, NUMERIC) -> BOOL
PValue __gt(PNode* node, Context* context, bool* eflg) {
    PValue val1, val2;
    evalBinary(node, context, &val1, &val2, eflg);
    if (*eflg) return nullPValue;
    bool result = false;
    if (val1.type ==  PVInt) result = val1.value.uInt > val2.value.uInt;
    else if (val1.type == PVFloat) result = val1.value.uFloat > val2.value.uFloat;
    else { *eflg = true; return nullPValue; }
    return PVALUE(PVBool, uBool, result);
}

// __and checks the and relation between two or more boolean PValues.
// usage: and(BOOL [,BOOL]+) -> BOOL
PValue __and(PNode *node, Context *context, bool* eflg) {
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

// __or checks the or relation between two or ore boolean PValues.
// usage: or(BOOL [,BOOL]+) -> BOOL
PValue __or (PNode *node, Context *context, bool* eflg) {
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

// __not is the builtin logical not operation.
// usage: not(BOOL) -> BOOL
PValue __not (PNode *node, Context *context, bool *errflg) {
	PValue value = evaluateBoolean(node->arguments, context, errflg);
	if (*errflg || value.type != PVBool) return nullPValue;
	return value.value.uBool ? falsePValue : truePValue;
}

// __isnull returns whether its argument PValue is nullPValue.
PValue __isnull(PNode* node, Context* context, bool *errflg) {
    PValue pvalue = evaluate(node->arguments, context, errflg);
    if (*errflg) return nullPValue;
    return (pvalue.type == PVNull) ? truePValue : falsePValue;
}

// evalBinary evaluates and returns the arguments of a binary expression.
static void evalBinary(PNode* pnode, Context* context, PValue* val1, PValue* val2, bool* eflag) {
    extern bool numericPValue(PValue pvalue);
    PNode* arg1 = pnode->arguments; // Get args as PNodes.
    if (!arg1) { *eflag = true; return; }
    PNode* arg2 = arg1->next;
    if (!arg2) { *eflag = true; return; }
    PValue pval1 = evaluate(arg1, context, eflag); // Eval args.
    PValue pval2 = evaluate(arg2, context, eflag);
    if (pval1.type != pval2.type || !numericPValue(pval1)) { // Must be numeric with same type.
        *eflag = true;
        return;
    }
    *val1 = pval1;
    *val2 = pval2;
}
