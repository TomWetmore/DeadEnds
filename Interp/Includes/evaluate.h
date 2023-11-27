//
//  DeadEnds
//
//  evaluate.h
//
//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 15 November 2023.
//

#ifndef evaluate_h
#define evaluate_h

#include "symboltable.h"   // Table.
#include "pnode.h"   // PNode.
#include "gnode.h"   // GNode.
#include "pvalue.h"  // PValue.
#include "interp.h"  // Context.

// TODO: Should some of these be made static inside evaluate.c?
PValue evaluate(PNode*, Context*, bool* errflag);          // Generic evaluator.
PValue evaluateIdent(PNode*, Context*, bool* errflag);     // Evaluate an identifier.
PValue evaluateBuiltin(PNode*, Context*, bool* errflag);   // Evaluate a built-in.
PValue evaluateUserFunc(PNode*, Context*, bool* errflag);  // Evaluate a user function.
GNode* evaluatePerson(PNode*, Context*, bool* errflag);    // Evaluate and return a person.
GNode* evaluateFamily(PNode*, Context*, bool* errflg);     // Evaluate and return a family.
PValue evaluateBoolean(PNode*, Context*, bool* errflg);    // Evaluate a boolean expression.

#endif /* evaluate_h */
