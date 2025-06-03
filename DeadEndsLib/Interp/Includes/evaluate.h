//
//  DeadEnds Library
//
//  evaluate.h
//
//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 8 May 2025.
//

#ifndef evaluate_h
#define evaluate_h

#include "standard.h"

typedef struct Context Context;
typedef struct GNode GNode;
typedef struct PNode PNode;
typedef struct PValue PValue;

// TODO: Should some of these be made static inside evaluate.c?
PValue evaluate(PNode*, Context*, bool*);          // Generic evaluator.
PValue evaluateIdent(PNode*, Context*);     // Evaluate an identifier.
PValue evaluateBuiltin(PNode*, Context*, bool*);   // Evaluate a built-in.
PValue evaluateUserFunc(PNode*, Context*, bool*);  // Evaluate a user function.
GNode* evaluatePerson(PNode*, Context*, bool*);    // Evaluate and return a person.
GNode* evaluateFamily(PNode*, Context*, bool*);    // Evaluate and return a family.
PValue evaluateBoolean(PNode*, Context*, bool*);   // Evaluate a boolean expression.
int evaluateInteger(PNode*, Context*, bool*);      // Evaluate an integer expression.
String evaluateString(PNode*, Context*, bool*);    // Evaluate a string expression.

#endif /* evaluate_h */
