//
//  DeadEnds
//
//  evaluate.c -- Contains the functions that evaluate program node expressions as part of the
//    DeadEnds program interpreter. Program nodes are heap objects that hold graphs of DeadEnds
//    programs. They represent functions, procedures, builtins, statements and expressions. The
//    program nodes for function calls and other expressions are evaluated to PValues (program
//    values), which are stack objects. There are a few constant program values that are created
//    in data space. Program values in symbol tables are an exception to this rule as they have
//    to be kept in the heap.
//
//  Builtin and user functions are evaluated. Procedures are interpreted. Symbol tables map
//    identifiers to program value pointers.

//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 31 May 2023.
//

#include "evaluate.h"
#include "standard.h"
#include "symboltable.h"
#include "functiontable.h"
#include "interp.h"
#include "pvalue.h"
#include "pnode.h"

static bool debugging = false;

extern SymbolTable *globalTable;
extern FunctionTable *functionTable;
extern bool traceprogram;
extern bool programDebugging;
extern const PValue nullPValue;

static bool pvalueToBoolean(PValue pvalue);

//  evaluate -- Generic evaluator.
//    Evaluation takes a program node (PNode) expression and evaluates it to a program value
//    (PValue). Evaluation begins in this function. Based on the type of the program node,
//    more specialized functions may be called. In the simple cases evaluation is done here.
//    This function checks for the program nodes of type of PNIdent (identifiers with values in
//    symbol tables), PNBltinCall and PNFuncCall (built-in and user-defined function calls), and
//    PNICons, PNSCons and PNFCons (integer, float and string constants). No other program node
//    types can be evaluated.
//
//  Program nodes are heap objects because they form graph structures that must persist after
//    they are the parser builds them.
//--------------------------------------------------------------------------------------------------
PValue evaluate(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node to evaluate.
//  symtab -- Symbol table with the current values the variables.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);
    if (programDebugging) {
        printf("evaluate:%d ", pnode->lineNumber); showPNode(pnode);
    }

    //  Identifiers. Call evaluateIdent to look them up in the symbol tables.
    if (pnode->type == PNIdent) return evaluateIdent(pnode, context, errflg);

    //  Built-in function calls. Call evaluateBuiltIn which calls the built-in's C code.
    if (pnode->type == PNBltinCall) return evaluateBuiltin(pnode, context, errflg);

    //  User-defined function calls. Call evaluateUserFunction with the function's root node.
    if (pnode->type == PNFuncCall) return evaluateUserFunc(pnode, context, errflg);

    *errflg = false;
    //  Integer (C long) constants. Evaluate directly.
    if (pnode->type == PNICons) return PVALUE(PVInt, uInt, pnode->intCons);

    // String constants. Evaluate directly,
    if (pnode->type == PNSCons) return PVALUE(PVString, uString, pnode->stringCons);

    // Float (C double) constants. Evaluate directly.
    if (pnode->type == PNFCons) return PVALUE(PVFloat, uFloat, pnode->floatCons);

    // Any other type of program node is an error.
    *errflg = true;
    return nullPValue;
}

//  evaluateIdent -- Evaluate an identifier by looking it up in the symbol tables.
//--------------------------------------------------------------------------------------------------
PValue evaluateIdent(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node holding an identifier.
//  symtab -- Local symbol table.
//  errflag -- Error flag.
{
    ASSERT((pnode->type == PNIdent) && context);
    if (programDebugging)
        printf("evaluateIden: %d: %s\n", pnode->lineNumber, pnode->identifier);

    // Get the identifer from the PNode.
    String ident = pnode->identifier;
    ASSERT(ident);

    // Look up the identifier in the symbol tables.
    return getValueOfSymbol(context->symbolTable, ident);
}

//  evaluateConditional -- Evaluate a conditional expression. Conditional expressions have the
//    form ([iden,] expr), where the identifier is optional. If it is there the value of the
//    expression is assigned to it. This function is called from interpIfStatement and
//    interpWhileStatement.
//--------------------------------------------------------------------------------------------------
bool evaluateConditional(PNode *pnode, Context *context, bool *errflg)
//  pnode -- One or two Program nodes.
//  symtab -- Symbol table.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);
    // Assume there is both an identifier and an expression.
    PNode *iden = pnode, *expr = pnode->next;

    // If the expression is null there is no variable, so adjust.
    if (!expr) {
        expr = iden;
        iden = null;
    }

    // The identifier must be an identifier.
    if (iden && iden->type != PNIdent) {
        *errflg = true;
        prog_error(pnode, "The first argument in a conditional expression must be an identifier.");
        return false;
    }

    // Evaluate the expression.
    PValue value = evaluate(expr, context, errflg);
    if (*errflg) {
        prog_error(pnode, "There was an error evaluating the conditional expression.");
        return false;
    }

    // If there is an identifier, set it to the expression value.
    if (iden) assignValueToSymbol(context->symbolTable, pnode->identifier, value);

    // The expression is used as a conditional, so coerce it to boolean.
    return pvalueToBoolean(value);
}

//  evaluateBuiltin -- Evaluate a built-in function by calling its C code.
//--------------------------------------------------------------------------------------------------
PValue evaluateBuiltin(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node holding a built-in function call with its arguments.
//  symtab -- Local symbol table.
//  errflg -- Error flag.
{
    // Call the C function that implements the built-in.
    return (*(BIFunc)pnode->builtinFunc)(pnode, context, errflg);
}

//  evaluateUserFunc -- Evaluate a user defined function.
//--------------------------------------------------------------------------------------------------
PValue evaluateUserFunc(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node holding a user-defined function.
//  symtab -- Local symbol table.
//  errflg -- Error flag.
{
    // Get the name of the function.
    String name = pnode->funcName;

    *errflg = true;
    // Look up the function in the function table.
    PNode *func;
    if ((func = (PNode*) searchFunctionTable(functionTable, name)) == null) {
        prog_error(pnode, "The function %s is undefined", pnode->funcName);
        return nullPValue;
    }
    // Create a symbol table for the function.
    SymbolTable *newtab = createSymbolTable();
    if (debugging) printf("evaluateUserFunc: creating symbol table %p for %s\n", newtab, name);

    // Get the first argument and parameter pair.
    PNode *arg = pnode->arguments;
    PNode *parm = func->parameters;

    // Evaluate the arguments and assign them to the parameters in the symbol table.
    while (arg && parm) {
        //bool eflg;
        //  Evaluate the current argument; return if there is an error.
        PValue value = evaluate(arg, context, errflg);
        if (*errflg) {
            prog_error(pnode, "could not evaluate an argument expression");
            return nullPValue;
        }
        //  Assign the value of the argument to the parameter.
        assignValueToSymbol(newtab, parm->identifier, value);

        //  Loop to the next argument and parameter.
        arg = arg->next;
        parm = parm->next;
    }
    //  Check there are the same number of arguments and parameters.
    if (arg || parm) {
        prog_error(pnode, "there are different numbers of arguments and parameters");
        deleteHashTable(newtab);
        return nullPValue;
    }
    //  Iterpret the function's body. The return value is passed back as the third parameter.
    PValue value;
    InterpType irc = interpret((PNode*) func->funcBody, context, &value);
    deleteHashTable(newtab);
    switch (irc) {
        case InterpReturn:
        case InterpOkay:
            *errflg = false;
            return value;
        case InterpBreak:
        case InterpContinue:
        case InterpError:
            *errflg = true;
            return nullPValue;
    }
    return nullPValue;
}

//  evaluateBoolean -- Evaluate an expression and convert it to a boolean program value using
//    C-like rules. In all but the error case this returns truePValue or falsePValue.
//--------------------------------------------------------------------------------------------------
PValue evaluateBoolean(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Expression to evaluate and interpret as a boolean.
//  symtab -- Local symbol table.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);

    // Use the generic evaluator.
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg) return nullPValue;

    // Interpret the returned value as a boolean.
    switch (pvalue.type) {
        case PVNull: return falsePValue;
        case PVBool: return pvalue;
        case PVInt: return pvalue.value.uInt ? truePValue : falsePValue;
        case PVFloat: return pvalue.value.uFloat != 0.0 ? truePValue : falsePValue;
        case PVString: return strlen(pvalue.value.uString) > 0 ? truePValue : falsePValue;
        case PVPerson:
        case PVFamily:
        case PVSource:
        case PVEvent:
        case PVOther:
        case PVGNode: return pvalue.value.uGNode ? truePValue : falsePValue;
        case PVSequence: return pvalue.value.uSequence ? truePValue : falsePValue;
        //case PVTable: return pvalue.pvValue.uTable ? truePValue : falsePValue;
        //case PVList: return pvalue.pvValue.uList ? truePValue : falsePValue;
        default: *errflg = true; return nullPValue;
    }
}

//  pvalueToBoolean -- Convert a program value to a boolean using C-like rules.
//--------------------------------------------------------------------------------------------------
static bool pvalueToBoolean(PValue pvalue)
//  pvalue -- Program value to be treated as a boolean.
{
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

//  evaluatePerson -- Evaluate a person expression. Return the root node of the person if there.
//--------------------------------------------------------------------------------------------------
GNode* evaluatePerson(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node expression that should evaluate to a person root gedcom node.
//  context -- Symbol table.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg ||  pvalue.type != PVPerson) return null;
    GNode* indi = pvalue.value.uGNode;
    if (nestr("INDI", indi->tag)) return null;
    return indi;
}

// evaluateFamily -- Evaluate family expression. Return the root node of the family if there.
//--------------------------------------------------------------------------------------------------
GNode* evaluateFamily(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node expression that should evaluate to a family root gedcom node.
//  symtab -- Symbol table.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg || pvalue.type != PVFamily) return null;
    GNode* fam = pvalue.value.uGNode;
    if (nestr("FAM", fam->tag)) return null;
    return fam;
}

//  evaluateGNode -- Evaluate any Gedcom node expression. Return the Gedcom node.
//--------------------------------------------------------------------------------------------------
GNode* evaluateGNode(PNode *pnode, Context *context, bool* errflg)
//  pnode -- Program node expression that should evaluate to an arbitrary Gedcom node.
//  context -- Local symbol table.
//  errflg -- Error flag.
{
    ASSERT(pnode && context);
    PValue pvalue = evaluate(pnode, context, errflg);
    if (*errflg || !isGNodeType(pvalue.type)) return null;
    return pvalue.value.uGNode;
}

// iistype -- See if a program node has the specified type.
//--------------------------------------------------------------------------------------------------
bool iistype (PNode *pnode, int type)
{
    return pnode->type == type;
}

// num_params -- Return the number of parameters in a list. (TODO: THIS IS JUST A GENERAL PURPOSE
// LENGTH OF LIST ROUTINE FOR PNODES.)
//--------------------------------------------------------------------------------------------------
int num_params (PNode *node)
// PNode node -- First PNode in a list of parameter nodes.
{
    int np = 0;
    while (node) {
        np++;
        node = node->next;
    }
    return np;
}

//  eval_and_coerce -- Generic evaluator and coercer.
//--------------------------------------------------------------------------------------------------
PValue eval_and_coerce(int type, PNode *node, Context *context, bool* eflg)
// type -- Type to coerce to.
// stab -- Local symbol table.
// eflg -- Possible error flag.
{
    PValue pvalue = evaluate(node, context, eflg);
    if (*eflg || pvalue.type == PVNull) {
        *eflg = true;
        return nullPValue;
    }
    coercePValue(&pvalue, type, eflg);
    return pvalue;
}
