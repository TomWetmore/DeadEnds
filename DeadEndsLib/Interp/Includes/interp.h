//
//  DeadEnds Library
//
//  interp.h is the header file for the DeadEnds script interpreter.
//
//  Created by Thomas Wetmore on 8 December 2022.
//  Last changed on 18 August 2025.
//

#ifndef interp_h
#define interp_h

#include "standard.h"

typedef struct Context Context;
typedef struct Database Database;
typedef struct File File;
typedef struct Frame Frame;
typedef struct GNode GNode;
typedef struct HashTable SymbolTable;
typedef struct PNode PNode;
typedef struct PValue PValue;
typedef struct Script Script;
typedef struct Program Program;

#define CC 32 // 'Commutative constant'.

// InterpType enumerates the interpreter functions return types.
typedef enum InterpType {
    InterpError = 0, InterpOkay, InterpBreak, InterpContinue, InterpReturn
} InterpType;

// Report Interpreter.
void initializeInterpreter(Database*);
void initset(void);
Program* parseProgram(String, String);
void runProgram(Program*, Database*, File*);
InterpType interpScript(Context*, File*);
InterpType interpret(PNode*, Context*, PValue*);
InterpType interpChildren(PNode*, Context*, PValue*);
InterpType interpSpouses(PNode*, Context*, PValue*);
InterpType interpFamilies(PNode*, Context*, PValue*);
InterpType interpFathers(PNode*, Context*, PValue*);
InterpType interpMothers(PNode*, Context*, PValue*);
InterpType interpParents(PNode*, Context*, PValue*);
InterpType interpFornotes(PNode*, Context*, PValue*);
InterpType interpForindi(PNode*, Context*, PValue*);
InterpType interpForfam(PNode*, Context*, PValue*);
InterpType interpForsour(PNode*, Context*, PValue*);
InterpType interpForeven(PNode*, Context*, PValue*);
InterpType interpForothr(PNode*, Context*, PValue*);
InterpType interpretSequenceLoop(PNode*, Context*, PValue*);
InterpType interpForList(PNode*, Context*, PValue*);
InterpType interpIfStatement(PNode*, Context*, PValue*);
InterpType interpWhileStatement(PNode*, Context*, PValue*);
InterpType interpProcCall(PNode*, Context*, PValue*);  // User-defined procedure calls.
InterpType interpTraverse(PNode*, Context*, PValue*);
InterpType interp_fornodes(PNode*, Context*, PValue*);

// Prototypes.
PNode *createPNode(int);
PValue evaluate(PNode*, Context*, bool*);
bool evaluateConditional(PNode*, Context*, bool*);
PValue evaluateBuiltin(PNode*, Context*, bool*);
PValue evaluateIdent(PNode*, Context*);
PValue evaluateUserFunc(PNode*, Context*, bool*);
GNode* evaluateGNode(PNode*, Context*, bool*);
String formatDate(String, int, int, int, int, bool);
String get_date(void);
PNode *iden_node(String);
bool iistype(PNode*, int);
int num_params(PNode*);
void scriptError(PNode*, String, ...);
int yylex(void);
int yyparse(void);

// Program running state flags.
extern bool programParsing;
extern bool programRunning;

// Debugging flags.
extern bool callTracing;
extern bool returnTracing;
extern bool symbolTableTracing;

#endif // interp_h
