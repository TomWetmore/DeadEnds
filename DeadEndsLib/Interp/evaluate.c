//
//  DeadEnds
//  evaluate.c contains the functions that evaluate PNode expressions during interpretation.
//  PNodes form the abstract syntax trees of DeadEnds scripts. They represent functions,
//  procedures, builtins, statements and expressions. PNodes for function calls and other
//  expressions evaluate to PValues.
//
//  Builtin and user functions are evaluated. Procedures are interpreted. Symbol tables map
//  identifiers to PValue pointers.

//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 1 June 2025.
//

#include "evaluate.h"
#include "standard.h"
#include "symboltable.h"
#include "functiontable.h"
#include "interp.h" 
#include "pvalue.h"
#include "pnode.h"
#include "utils.h"
#include "context.h"

static bool debugging = false;
static bool builtInDebugging = false;
 bool symbolTableDebugging = false;
extern SymbolTable *globals;
extern FunctionTable *functions;
extern bool traceprogram;
extern bool programDebugging;
extern const PValue nullPValue;
static bool pvalueToBoolean(PValue pvalue);

// evaluate is the generic evaluator. It evaluates a PNode expression into a PValue. Evaluation
// begins in this function. Based on PNode type, more specialied functions may be called.
PValue evaluate(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
	if (programDebugging) {
		printf("evaluate:%d ", pnode->lineNumber);
		showPNode(pnode);
	}
    if (pnode->type == PNIdent) return evaluateIdent(pnode, context);
    if (pnode->type == PNBltinCall) return evaluateBuiltin(pnode, context, errflg);
    if (pnode->type == PNFuncCall) return evaluateUserFunc(pnode, context, errflg);
    *errflg = false;
    if (pnode->type == PNICons) return PVALUE(PVInt, uInt, pnode->intCons);
    if (pnode->type == PNSCons) return createStringPValue(pnode->stringCons);
    if (pnode->type == PNFCons) return PVALUE(PVFloat, uFloat, pnode->floatCons);
    *errflg = true;
    return nullPValue;
}

// evaluateIdent evaluates an identifier by looking it up in the symbol tables.
PValue evaluateIdent(PNode* pnode, Context* context) {
    ASSERT((pnode->type == PNIdent) && context);
    if (programDebugging)
        printf("evaluateIdent: %d: %s\n", pnode->lineNumber, pnode->identifier);
    String ident = pnode->identifier;
    ASSERT(ident);
    if (symbolTableDebugging) showSymbolTable(context->frame->table);
    PValue orig = getValueOfSymbol(context, ident);
    // If the PValue is a string, copy the string.
    if (orig.type == PVString) {
        return createStringPValue(orig.value.uString);
    } else {
        return orig; // safe for non-owning types like int, float, etc.
    }
}

// evaluateConditional evaluates a conditional expression. They have the form ([iden,] expr),
// where iden is optional. If there the value of the expression is assigned to it.
bool evaluateConditional(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    PNode *iden = pnode, *expr = pnode->next; // Assume both.
    if (!expr) { // If expr is null there is no iden.
        expr = iden;
        iden = null;
    }
    if (iden && iden->type != PNIdent) { // iden must be an identifier.
        *errflg = true;
        scriptError(pnode, "The first argument in a conditional expression must be an identifier.");
        return false;
    }
    PValue value = evaluate(expr, context, errflg); // Evaluate the expression.
    if (*errflg) {
        scriptError(pnode, "There was an error evaluating the conditional expression");
        return false;
    }
    if (iden) assignValueToSymbol(context, pnode->identifier, value);
    return pvalueToBoolean(value); // Coerce to bool.
}

// evaluateBuiltin evaluates a built-in function call by calling its C code function.
PValue evaluateBuiltin(PNode* pnode, Context* context, bool* errflg) {
	if (builtInDebugging) printf("%s %2.3f\n", pnode->stringOne, getMseconds());
    return (*(BIFunc) pnode->builtinFunc)(pnode, context, errflg);
}

// evaluateUserFunc evaluates a user defined function. The evaluator 'call' the function. The steps are:
// 1. Find the function in the function table from the name in the pnode's funcName field.
// 2. Create a SymbolTable and Frame and add it to the Context.
// 3. Evaluate the arguments and bind them to the parameters in the SymbolTable.
PValue evaluateUserFunc(PNode *pnode, Context *context, bool* errflg) {
    String name = pnode->funcName;
	if (debugging) printf("evaulateUserFunc: %s\n", name);
    PNode *func = searchFunctionTable(context->functions, name);
    if (!func) {
        scriptError(pnode, "function %s is undefined", name);
        *errflg = true;
        return nullPValue;
    }
    // Create a SymbolTable and Frame for this function, and add them to the Context.
    SymbolTable *table = createSymbolTable();

    // Bind the arguments to the parameters.
    PNode *arg = pnode->arguments;
    PNode *parm = func->parameters;
    while (arg && parm) {
        PValue value = evaluate(arg, context, errflg); // Eval arg.
        if (*errflg) {
            scriptError(pnode, "could not evaluate an argument of %s", name);
            return nullPValue;
        }
        assignValueToSymbolTable(table, parm->identifier, value);
        arg = arg->next;
        parm = parm->next;
    }
    if (arg || parm) {
        scriptError(pnode, "different numbers of arguments and parameters to %s", name);
        *errflg = true;
        return nullPValue;
    }
	if (symbolTableDebugging) showSymbolTable(table);
    // Create the frame for the called function. Call it. Delete the frame.
    Frame* frame = createFrame(pnode, func, table, context->frame);
    context->frame = frame;
    PValue value;
    InterpType irc = interpret((PNode*) func->funcBody, context, &value);
    context->frame = frame->caller;
    deleteFrame(frame);
    
    switch (irc) {
        case InterpReturn:
        case InterpOkay:
            *errflg = false;
            return value;
        case InterpBreak:
        case InterpContinue:
        case InterpError:
        default:
            *errflg = true;
            return nullPValue;
    }
}

// evaluateBoolean evaluates a PNode expression and converts it to a boolean PValue using C like rules.
PValue evaluateBoolean(PNode* pnode, Context* context, bool* errflg) {
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg) return nullPValue;
    return pvalueToBoolean(pvalue) ? truePValue : falsePValue;
}

// pvalueToBoolean converts a program value to a boolean using C like rules.
static bool pvalueToBoolean(PValue pvalue) {
    switch (pvalue.type) {
        case PVNull: return false;
        case PVBool: return pvalue.value.uBool;
        case PVInt: return pvalue.value.uInt != 0;
        case PVFloat: return pvalue.value.uFloat != 0.0;
        case PVString: return strlen(pvalue.value.uString) > 0;
        case PVPerson:
        case PVFamily:
        case PVSource:
        case PVEvent:
        case PVOther:
        case PVGNode: return pvalue.value.uGNode;
        case PVSequence: return pvalue.value.uSequence;
        //case PVTable: return pvalue.pvValue.uTable ? truePValue : falsePValue;
        case PVList: return pvalue.value.uList;
        default: return false;
    }
}

// evaluatePerson evaluates a person expression. Returns the root GNode of the person if there.
GNode* evaluatePerson(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg || pvalue.type == PVNull) return null; // Error or null chaining.
    if (pvalue.type != PVPerson) {
        scriptError(pnode, "expression must be a person");
        *errflg = true;
        return null;
    }
    GNode* indi = pvalue.value.uGNode;
    if (nestr("INDI", indi->tag)) {
        scriptError(pnode, "serious error: expression must be a person");
        *errflg = true;
        return null;
    }
    return indi;
}

// evaluateFamily evaluates a family expression. Return the root GNode of the family if there.
GNode* evaluateFamily(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg || pvalue.type == PVNull) return null; // Error or null chaining.
    if (pvalue.type != PVFamily) {
        scriptError(pnode, "expression must be a family");
        *errflg = true;
        return null;
    }
    GNode* fam = pvalue.value.uGNode;
    if (nestr("FAM", fam->tag)) {
        scriptError(pnode, "serious error: expression must be a family");
        *errflg = true;
        return null;
    }
    return fam;
}

// evaluateGNode evaluate a GNode expression. Return the Gedcom node.
GNode* evaluateGNode(PNode* pnode, Context* context, bool* errflg) {
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg || !isGNodeType(pvalue.type)) return null;
    return pvalue.value.uGNode;
}

// evaluateInteger evaluates an expression that should resolve to an integer.
int evaluateInteger(PNode *pnode, Context *context, bool *errflg) {
	ASSERT(pnode && context);
	PValue pvalue = evaluate(pnode, context, errflg);
	if (*errflg || pvalue.type != PVInt) {
        scriptError(pnode, "expression must be an integer");
		*errflg = true;
		return 0;
	}
	return (int) pvalue.value.uInt;
}

//  evaluateString evaluates an expression that should resolve to a String.
String evaluateString(PNode* pnode, Context* context, bool *errflg) {
	ASSERT(pnode && context);
	PValue pvalue = evaluate(pnode, context, errflg);
	if (*errflg || pvalue.type != PVString) {
		*errflg = true;
		return "";
	}
	return pvalue.value.uString;
}

// iistype checks that a PNode has a specified type.
bool iistype (PNode* pnode, int type)
{
    return pnode->type == type;
}

// num_params returns the number of parameters in a list.
int num_params (PNode* node) {
    int np = 0;
    while (node) {
        np++;
        node = node->next;
    }
    return np;
}
