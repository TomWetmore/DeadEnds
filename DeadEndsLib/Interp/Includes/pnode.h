//
//  DeadEnds
//
//  pnode.h -- Header file for the program node structure.
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed on 16 November 2023.
//

#ifndef pnode_h
#define pnode_h

typedef struct PValue PValue;  // Forward reference.
typedef struct Context Context; 
#include "standard.h"
#include "symboltable.h"
#include "interp.h"  // Context.

//  PNode -- Program Node. The parser builds an abstract syntax tree for each procedure and 
//    function it parses. The nodes of the trees are program nodes. They represent both statements
//    and expressions. The interpreter interprets the trees when running programs.
//--------------------------------------------------------------------------------------------------
typedef struct PNode PNode;
typedef struct HashTable SymbolTable;

// Program node types.
//--------------------------------------------------------------------------------------------------
typedef enum PNType{
	PNICons = 1, PNFCons, PNSCons, PNIdent, PNIf, PNWhile, PNBreak, PNContinue, PNReturn,
	PNProcDef, PNProcCall, PNFuncDef, PNFuncCall, PNBltinCall, PNTraverse, PNNodes, PNFamilies,
	PNSpouses, PNChildren, PNIndis, PNFams, PNSources, PNEvents, PNOthers, PNList, PNSequence,
	PNTable, PNFathers, PNMothers, PNFamsAsChild, PNNotes
} PNType;

//  BIFunc -- Type of a function pointer that takes a program node, symbol table, and boolean
//    and returns a program pvalue.
//--------------------------------------------------------------------------------------------------
typedef PValue (*BIFunc)(PNode *node, Context *context, bool* errflag);

//  struct PNode -- The program node structure.
//--------------------------------------------------------------------------------------------------
struct PNode {
	// Fields found in all program nodes.
	PNType type;        // Type of this program node.
	PNode  *parent;     // Parent of this node; null in root nodes.
	int    lineNumber;  // Line number of this node from its program file.
	String fileName;    // Program file this node is from.

	PNode  *next;       // Next node in a list (e.g., statement blocks, arguments).
	long   intCons;     // Integer (C long) constant.
	double floatCons;   // Float (C double) constant.
	String stringOne;   // String constant: pIdentifier, pProcName, pFuncName, pBuiltinName

	PNode  *expression;

	// PNode expressions in loops.
	PNode  *gnodeExpr;  // PNode expression that should resolve to a GNode.
	PNode  *listExpr;    // PNode expression that must resolve to a List.
	PNode  *setExpr;    // PNode expression that must resolve to a Set.
	PNode  *sequenceExpr;      // PNode expression that must resolve to an IndiSet.

	PNode *pnodeOne;    // pThenState, pLoopState, pFuncBody, pProcBody.
	PNode *pnodeTwo;    // pElseState.

	BIFunc builtinFunc;  // Pointer to a built-in function.

	String idenOne;
	String idenTwo;
	String idenThree;
};

// Mnemonic names for the program node fields.
//--------------------------------------------------------------------------------------------------
#define identifier   stringOne   // Identifier name.
#define procName     stringOne   // Procedure name.
#define funcName     stringOne   // Function (built-in and user-defined) name.
#define builtinName  stringOne   // Built-in function name.

#define thenState  pnodeOne     // First PNode in an if statement then clause.
#define elseState  pnodeTwo     // First PNode in an if statement else clause.
#define loopState  pnodeOne     // First PNode in a loop (many types) body.

#define funcBody   pnodeOne     // First PNode in user-defined function body.
#define procBody   pnodeOne     // First PNode in procedure body.
#define parameters pnodeTwo     // First parameter to function or procedure in list.
#define arguments  pnodeTwo     // First argument to function or procedure in list.

#define condExpr   expression   // Conditional expression used by if and while statements.
#define returnExpr expression   // Expression returned from a return statement.
#define personExpr expression   // Person expression used in some loops.
#define familyExpr expression   // Family expression used in some loops.
#define gnodeExpr  expression   // GNode expression used in some loops.
#define listExpr   expression   // Probably not needed because of pArguments and pParameters.
#define sequenceExpr  expression

#define countIden   idenThree   // Most loops have a counter.
#define levelIden   idenTwo     // Traverse loops have a level.
#define valueIden   idenTwo     // Sequence loops have a value.

#define personIden  idenOne
#define familyIden  idenOne
#define childIden   idenOne
#define spouseIden  idenTwo    // Spouse and family idens occur in two loop types.

#define gnodeIden   idenOne    // Traverse, others??? add to the list here
#define fatherIden  idenTwo    // So now, mothers and fathers can't be done together.
#define motherIden  idenTwo
#define sourceIden  idenOne
#define eventIden   idenOne
#define otherIden   idenOne
#define elementIden idenOne   // Element of a list or set.

#define stringCons  stringOne

PNode *iconsPNode(long);
PNode *fconsPNode(double);
PNode *sconsPNode(String string);
PNode *ifPNode(PNode*, PNode*, PNode*);
PNode *whilePNode(PNode*, PNode*);
PNode *breakPNode(void);
PNode *continuePNode(void);
PNode *returnPNode(PNode*);
PNode *procDefPNode(String, PNode*, PNode*);
PNode *procCallPNode(String, PNode*);
PNode *funcDefPNode(String, PNode*, PNode*);
PNode *funcCallPNode(String, PNode*);
PNode* traversePNode(PNode*, String, String, PNode*);
PNode *fornodesPNode(PNode*, String, PNode*);
PNode *familiesPNode(PNode*, String, String, String, PNode*);
PNode *spousesPNode(PNode*, String, String, String, PNode*);
PNode *childrenPNode(PNode*, String, String, PNode*);
PNode *forindiPNode(String, String, PNode*);
PNode *forfamPNode(String, String, PNode*);
PNode *forsourPNode(String, String, PNode*);
PNode *forevenPNode(String, String, PNode*);
PNode *forothrPNode(String, String, PNode*);
PNode *forlistPNode(PNode*, String, String, PNode*);
PNode *forindisetPNode(PNode*, String, String, String, PNode*);
PNode *fornotesPNode(PNode*, String, PNode*);
PNode *fathersPNode(PNode*, String, String, String, PNode*);
PNode *mothersPNode(PNode*, String, String, String, PNode*);
PNode *parentsPNode(PNode*, String, String, PNode*);

void freePNodes(PNode*);
void showPNode(PNode*);

//  BuiltIn -- Structure that holds built-in functions.
//--------------------------------------------------------------------------------------------------
typedef struct BuiltIn {
	String name;    //  Name of the function.
	int minParams;  //  Minimum number of parameters to the function.
	int maxParams;  //  Maximum number of parameters to the function.
	BIFunc func;    //  Pointer to the C function that implements the function.
} BuiltIn;

extern BuiltIn builtIns[];  //  Array of built-in functions.
extern int numBuiltIns;     //  Number of built-in function.

#endif // pnode_h
