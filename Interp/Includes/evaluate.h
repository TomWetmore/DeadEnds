//
//  DeadEnds
//
//  evaluate.h
//
//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 10 February 2024.
//

#ifndef evaluate_h
#define evaluate_h

#include "symboltable.h"   // Table.
#include "pnode.h"   // PNode.
#include "gnode.h"   // GNode.
#include "pvalue.h"  // PValue.
#include "interp.h"  // Context.

// TODO: Should some of these be made static inside evaluate.c?
PValue evaluate(PNode*, Context*, bool*);          // Generic evaluator.
PValue evaluateIdent(PNode*, Context*, bool*);     // Evaluate an identifier.
PValue evaluateBuiltin(PNode*, Context*, bool*);   // Evaluate a built-in.
PValue evaluateUserFunc(PNode*, Context*, bool*);  // Evaluate a user function.
GNode* evaluatePerson(PNode*, Context*, bool*);    // Evaluate and return a person.
GNode* evaluateFamily(PNode*, Context*, bool*);    // Evaluate and return a family.
PValue evaluateBoolean(PNode*, Context*, bool*);   // Evaluate a boolean expression.
int evaluateInteger(PNode*, Context*, bool*);      // Evaluate an integer expression.
String evaluateString(PNode*, Context*, bool*);    // Evaluate a string expression.

#endif /* evaluate_h */
