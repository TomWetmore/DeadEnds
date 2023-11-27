//
//  DeadEnds
//
//  pnode.c -- Most of the functions in this file create PNode (program nodes) of the many
//    different types.
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed on 3 June 2023.
//

#include "pnode.h"
#include "standard.h"
#include "hashtable.h"
#include "functiontable.h"
#include "gedcom.h"
#include "interp.h"

static bool debugging = false;

//  String names for the program node types. For debugging.
//--------------------------------------------------------------------------------------------------
String pnodeTypes[] = {
    "", "ICons", "FCons", "SCons", "Ident", "If", "While", "Break", "Continue", "Return",
    "ProcDef", "ProcCall", "FuncDef", "FuncCall", "BltinCall", "Traverse", "Nodes", "Families",
    "Spouses", "Children", "Indis", "Fams", "Sources", "Events", "Others", "List", "Set",
    "Fathers", "Mothers", "FamsAsChild", "Notes"
};

// External global variable not declared in header files.
//--------------------------------------------------------------------------------------------------
extern FunctionTable *functionTable;  // parse.c
extern String currentProgramFileName;  // parse.c; used to set the file name of PNodes.
extern int currentProgramLineNumber;;   // parse.c; used to set the line numbers of PNodes.

// showPNode -- Show a PNode. For debugging.
//--------------------------------------------------------------------------------------------------
void showPNode(PNode *pnode)
{
    printf("%s ", pnodeTypes[pnode->type]);
    switch (pnode->type) {
        case PNICons:     printf("%ld\n", pnode->intCons); break;
        case PNFCons:     printf("%g\n", pnode->floatCons); break;
        case PNSCons:     printf("%s\n", pnode->stringCons); break;
        case PNProcCall:  printf("%s()\n", pnode->procName); break;
        case PNFuncCall:  printf("%s()\n", pnode->funcName); break;
        case PNBltinCall: printf("%s()\n", pnode->funcName); break;
        case PNIdent:     printf("%s\n", pnode->identifier); break;
        case PNProcDef:   printf("%s\n", pnode->procName); break;
        case PNFuncDef:   printf("%s\n", pnode->funcName); break;
        case PNIf:
        case PNWhile:
        case PNBreak:
        case PNContinue:
        case PNReturn:
        case PNTraverse:
        case PNNodes:
        case PNFamilies:
        case PNSpouses:
        case PNChildren:
        case PNIndis:
        case PNFams:
        case PNSources:
        case PNEvents:
        case PNOthers:
        case PNList:
        case PNSequence:
        case PNFathers:
        case PNMothers:
        case PNFamsAsChild:
        case PNNotes:
        default: printf("\n"); break;
    }
}

static void setParents(PNode *list, PNode *parent);  // Set the parent to a list of PNodes.

//  allocPNode -- Allocate a program node. The node create functions use this function. This
//    function sets the pType, pFileName and pLineNum fields.
//--------------------------------------------------------------------------------------------------
static PNode *allocPNode(int type)
// type -- Type of the node.
{
    // Allocate the node.
    PNode *node = (PNode*) stdalloc(sizeof(*node));
    if (debugging) {
        printf("allocPNode(%d) %s, %d\n", type, currentProgramFileName, currentProgramLineNumber);
    }
    // Initialize the type, filename, and linenumber fields.
    node->type = type;
    node->fileName = strsave(currentProgramFileName);  // TODO: MEMORY!!!!!!!!!
    node->lineNumber = currentProgramLineNumber;  //  May be overwritten by the yacc m production.
    return node;
}

// icons_node -- Create an integer PNode. Using a C long integer as the underlying type.
//--------------------------------------------------------------------------------------------------
PNode *iconsPNode(long intConstant)
// ival -- Integer (a C long) to embed in a PNode.
{
    PNode *node = allocPNode(PNICons);
    node->intCons = intConstant;
    return node;
}

// fcons_node -- Create a floating point PNode. Using a C double as the underlying type.
//--------------------------------------------------------------------------------------------------
PNode *fconsPNode(double floatConstant)
{
    PNode *pnode = allocPNode(PNFCons);
    pnode->floatCons = floatConstant;
    return pnode;
}

// scons_node -- Create a String PNode.
//--------------------------------------------------------------------------------------------------
PNode *sconsPNode(String string)
{
    PNode *pnode = allocPNode(PNSCons);
    pnode->stringCons = string;
    return pnode;
}

// ifPNode -- Create an if statement PNode.
//--------------------------------------------------------------------------------------------------
PNode *ifPNode (PNode *cond, PNode *tnode, PNode *enode)
// cond -- Conditional PNode expression.
// tnode -- First PNode in then then clause.
// enode -- First PNode in the else clause.
{
    PNode *pnode = allocPNode(PNIf);
    pnode->condExpr = cond;
    pnode->thenState = tnode;
    pnode->elseState = enode;
    setParents(tnode, pnode);
    setParents(enode, pnode);
    return pnode;
}

// whilePNode -- Create a while loop PNode.
//--------------------------------------------------------------------------------------------------
PNode *whilePNode(PNode *cond, PNode *body)
// cond -- Conditional PNode expression.
// body -- First PNode in the loop body.
{
    PNode *node = allocPNode(PNWhile);
    node->condExpr = cond;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// breakPNode -- Create a break PNode.  Break nodes have no non-common fields.
//--------------------------------------------------------------------------------------------------
PNode *breakPNode(void) { return allocPNode(PNBreak); }

// continuePNode -- Create a continue PNode. Continue nodes have no non-common fields.
//--------------------------------------------------------------------------------------------------
PNode *continuePNode(void) { return allocPNode(PNContinue); }

// returnPNode -- Create a return PNode with an optional return expression.
//--------------------------------------------------------------------------------------------------
PNode *returnPNode(PNode *args)
// args -- return Pnode expression.
{
    PNode *node = allocPNode(PNReturn);
    if (args) { node->returnExpr = args; }
    return node;
}

//  procDefPNode -- Create a user-defined procedure definition node. This is called by the parser
//    when a procedure definition is recognized.
//--------------------------------------------------------------------------------------------------
PNode *procDefPNode(String name, PNode *parms, PNode *body)
// name -- Name of the procedure.
// parms -- List of parameters (identifiers) to the procedure.
// body -- Root PNode of the procedure's PNode syntax tree.
{
    PNode *node = allocPNode(PNProcDef);
    node->procName = name;
    node->parameters = parms;
    node->procBody = body;
    setParents(body, node);
    return node;
}

//  procCallPNode -- Create a procedure call node. This is called by the parser when a procedure
//    call is recognized.
//--------------------------------------------------------------------------------------------------
PNode *procCallPNode(String name, PNode *args)
//  name -- Name of the procedure.
//  args -- List of argument expressions to pass to the procedure.
{
    PNode *node = allocPNode(PNProcCall);
    node->procName = strsave(name);
    node->arguments = args;
    return node;
}

//  funcDefPNode -- Create user function definition node. This is called by the parser when a
//    function definition is recognized.
//--------------------------------------------------------------------------------------------------
PNode *funcDefPNode(String name, PNode *parms, PNode *body)
//  name -- Name of the function.
//  parms -- List of parameters to the function.
//  body -- Root PNode of the function's PNode syntax tree.
{
    PNode *node = allocPNode(PNFuncDef);
    node->funcName = name;
    node->parameters = parms;
    node->funcBody = body;
    setParents(body, node);
    return node;
}

//  funcCallPNode -- Create a builtin or user-defined function call program node. We find which
//    one by looking the name up in the user-defined function table.
//--------------------------------------------------------------------------------------------------
PNode *funcCallPNode(String name, PNode *alist)
// name -- Name of the function.
// alist -- List of the function's argument program node expressions.
{
    //  See if the function is built-in or user-defined. Check the function table first. If it
    //    is there it is user-defined. It is possible for a user-defined function to override a
    //    built-in function.
    if (isInHashTable(functionTable, name)) {

        //  The function is user-definded so create a user-defined function call node.
        PNode *node = allocPNode(PNFuncCall);
        node->funcName = name;
        node->arguments = alist;

        // Get the function's body from the function table.
        node->funcBody = searchFunctionTable(functionTable, name);
        return node;
    }

    //  The function is not user-defined. Look for it in the built-in function table.
    int lo = 0;
    int hi = nobuiltins - 1;
    bool found = false;
    int r;
    int md = 0;
    while (lo <= hi) {
        md = (lo + hi) >> 1;
        if ((r = nestr(name, builtIns[md].name)) < 0)
            hi = md - 1;
        else if (r > 0)
            lo = md + 1;
        else {
            found = true;
            break;
        }
    }

    //  If a built-in function with the name is found.
    if (found) {
        // Check that there is the right number of arguments.
        int n;
        if ((n = num_params(alist)) < builtIns[md].minParams || n > builtIns[md].maxParams) {
            printf("%s: must have %d to %d parameters.\n", name,
                   builtIns[md].minParams, builtIns[md].maxParams);
            Perrors++;
        }

        //  Create and return a built-in call program node.
        PNode *node = allocPNode(PNBltinCall);
        node->funcName = name;
        node->arguments = alist;
        node->builtinFunc = builtIns[md].func;
        return node;
    }

    // If the name was in neither table, create a user-defined function with null body.
    printf("%s: undefined function.\n", name);
    Perrors++;
    PNode *node = allocPNode(PNFuncCall);
    node->funcName = name;
    node->parameters = alist;
    node->funcBody = null;
    return node;
}

//  traversePNode -- Create traverse loop PNode to traverse a tree of Gedcom Nodes.
//--------------------------------------------------------------------------------------------------
PNode *traversePNode (PNode *nexpr, String snode, String levv, PNode *body)
//  nexpr -- Expression that evaluates to a gedcom node to traverse.
//  snode -- Variable name of the current sub-node.
//  levv -- Variable name of the current level.
//  body -- Root of the loop's program node tree.
{
    PNode *node = allocPNode(PNTraverse);
    node->gnodeExpr = nexpr;
    node->gnodeIden = snode;
    node->levelIden = levv;
    node->loopState = body;
    setParents(body, node);
    return node;
}

//  fornodesPNode -- Create fornodes loop PNode to iterate through the children of a Gedcom Node.
//--------------------------------------------------------------------------------------------------
PNode *fornodesPNode (PNode *nexpr, String nvar, PNode *body)
//  nexpr -- Expression that evaluates to a gedcom node.
//  nvar -- Name of variable that iterates through the children of the node.
//  body -- Root of the loop's program node tree.
{
    PNode *node = allocPNode(PNNodes);
    node->gnodeExpr = nexpr;
    node->gnodeIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

//  familiesPNode -- Create a family loop program node that loops through all families that
//    a person is a spouse in. Called by yyparse() on rule reduction.
//--------------------------------------------------------------------------------------------------
PNode *familiesPNode(PNode *pexpr, String fvar, String svar, String count, PNode *body)
//  pexpr -- Program node expression that evaluates to a person gedcom node.
//  fvar -- Name of the family identifier.
//  svar -- Name of the spouse identifier.
//  nvar -- Name of the counter variable.
//  body -- First program node of the loop body..
{
    PNode *node = allocPNode(PNFamilies);
    node->personExpr = pexpr;
    node->familyIden = fvar;
    node->spouseIden = svar;
    node->countIden = count;
    node->loopState = body;
    setParents(body, node);
    return node;
}

//  spousesPNode -- Create a spouse loop program node that loops through all spouses that the
//    given person is a spouse of. Called by yyparse() on rule reduction.
//--------------------------------------------------------------------------------------------------
PNode *spousesPNode (PNode *pexpr, String svar, String fvar, String count, PNode *body)
//  pexpr -- Program node expression that evaluate to a person gedcom node.
//  svar -- Name of the spouse loop variable.
//  fvar -- Name of the family loop variable.
//  nvar -- Name of the loop counter variable.
//  body -- First program node of the loop body.
{
    PNode *node = allocPNode(PNSpouses);
    node->personExpr = pexpr;
    node->spouseIden = svar;
    node->familyIden = fvar;
    node->countIden = count;
    node->loopState = body;  // Body of the loop.
    setParents(body, node);
    return node;
}

//  childrenPNode -- Create a children loop program node that loops through all chidren in a
//    family. Called by yyparse() on rule reduction.
//--------------------------------------------------------------------------------------------------
PNode *childrenPNode (PNode *fexpr, String cvar, String nvar, PNode *body)
//  fexpr -- Program node expression that evaluates to a family gedcom node.
//  cvar -- Name of the child loop variable.
//  nvar -- Name of the loop counter variable.
//  body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNChildren);
    node->familyExpr = fexpr;
    node->childIden = cvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

//  forindiPNode -- Create a forindi PNode to iterate every Person in the Database.
//    usage: forindi(INDI_V, INT_V) { body }
//    fields: pPersonIden, pCountIden, pLoopState
//--------------------------------------------------------------------------------------------------
PNode *forindiPNode (String ivar, String nvar, PNode *body)
//  ivar -- Person identifier.
//  nvar -- Counter identifier.
//  body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNIndis);
    node->personIden = ivar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

//  forfamPNode -- Create a forfam program node loop to iterate every family in the database.
//--------------------------------------------------------------------------------------------------
PNode *forfamPNode (String fvar, String nvar, PNode *body)
//  fvar -- Family identifier.
//  nvar -- Counter identifier.
//  body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNFams);
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forsourPNode -- Create a forsour PNode loop to iterate every Source in the Database.
//--------------------------------------------------------------------------------------------------
PNode *forsourPNode (String svar, String nvar, PNode *body)
// svar -- Source identifier.
// nvar -- Counter identifier.
// body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNSources);
    node->sourceIden = svar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forevenPNode -- Create foreven loop node
//--------------------------------------------------------------------------------------------------
PNode *forevenPNode(String evar, String nvar, PNode *body)
// evar -- Event identifier.
// nvar -- Counter identifier.
// body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNEvents);
    node->eventIden = evar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forothrPNode -- Create forothr loop node
//--------------------------------------------------------------------------------------------------
PNode *forothrPNode (String ovar, String nvar, PNode *body)
// ovar -- Other identifier.
// nvar -- Counter identifier.
// body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNOthers);
    node->otherIden = ovar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forlistPNode -- Create a list loop PNode.
// Usage:
//--------------------------------------------------------------------------------------------------
PNode *forlistPNode (PNode *lexpr, String evar, String nvar, PNode *body)
// iexpr -- PNode expression that evaluates to a List.
// evar -- Element identifier.
// nvar -- Counter identifier.
// body -- First PNode of the loop body.
{
    PNode *node = allocPNode(PNList);
    node->listExpr = lexpr;
    node->elementIden = (Word) evar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forindisetPNode -- Create an index loop PNode.
//--------------------------------------------------------------------------------------------------
PNode *forindisetPNode(PNode *iexpr, String ivar, String vvar, String nvar, PNode *body)
// iexpr expr
// ivar    person
// vvar    value
// nvar    counter
// body    body
{
    PNode *node = allocPNode(PNSequence);
    node->sequenceExpr = iexpr;
    node->elementIden = ivar;
    node->valueIden = vvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// fornotesPNode -- Create fornotes loop node
//--------------------------------------------------------------------------------------------------
PNode *fornotesPNode (PNode *nexpr, String vvar, PNode *body)
// nexpr -- PNode expression that evaluates to a GNode.
// vvar -- value
// body -- First PNode of loop body.
{
//    PNode *node = allocPNode(INOTES);
//    node->pnexpr = nexpr;
//    istrng(node) = (Word) vvar;
//    node->pbody = body;
//    setParents(body, node);
//    return node;
    return null;
}

// iden_node -- Create an identifier PNode.
// TODO: FIGURE OUTWHAT TO DO WITH THIS.
//--------------------------------------------------------------------------------------------------
PNode *iden_node(String identifier)
// iden -- String to embed in a PNode.
{
    PNode *pnode = allocPNode(PNIdent);
    pnode->identifier = identifier;
    return pnode;
}

// fathersPNode -- Create fathers loop node.
//--------------------------------------------------------------------------------------------------
PNode *fathersPNode(PNode *pexpr, String pvar, String fvar, String nvar, PNode *body)
//  pexpr -- Person PNode expression.
//  pvar -- Father identifier.
//  fvar -- Family identifier.
//  nvar -- Count identifier.
//  body -- First PNode of body.
{
    PNode *node = allocPNode(PNFathers);
    node->personExpr = pexpr;
    node->fatherIden = pvar;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// mothersPNode -- Create mothers loop node.
//--------------------------------------------------------------------------------------------------
PNode *mothersPNode(PNode *pexpr, String pvar, String fvar, String nvar, PNode *body)
//  pexpr -- Person PNode expression.
//  pvar -- Mother identifier.
//  fvar -- Family identifier.
//  nvar -- Count identifier.
//  body -- First PNode of loop.
{
    PNode *node = allocPNode(PNMothers);
    node->personExpr = pexpr;
    node->motherIden = pvar;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// parentsPNode -- Create parents loop node.
//--------------------------------------------------------------------------------------------------
PNode *parentsPNode(PNode *pexpr, String fvar, String nvar, PNode *body)
//  pexpr -- Person PNode expression.
//  fvar -- Family identifier.
//  nvar -- Count identifier.
//  body -- First PNode of loop.
{
    PNode *node = allocPNode(PNFamsAsChild);
    node->personExpr = pexpr;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// setParents -- Set the parent node for a list of nodes.
//--------------------------------------------------------------------------------------------------
static void setParents (PNode *list, PNode *parent)
//  list -- List of program nodes to be assigned a parent.
//  parent -- Node to be made the parent.
{
    while (list) {
        list->parent = parent;
        list = list->next;
    }
}

//typedef enum {
//    PNICons = 1, PNFCons, PNSCons, PNIdent, PNIf, PNWhile, PNBreak, PNContinue, PNReturn,
//    PNProcDef, PNProcCall, PNFuncDef, PNFuncCall, PNBltinCall, PNTraverse, PNNodes, PNFamilies,
//    PNSpouses, PNChildren, PNIndis, PNFams, PNSources, PNEvents, PNOthers, PNList, PNSequence,
//    PNTable, PNFathers, PNMothers, PNFamsAsChild, PNNotes
//} PNType;

//  freePNodes -- Free the program nodes rooted at the given program node.
//    TODO: Write me.
//    NOTE: This is more complicated than it seem at first. Just like there is a separate
//      function for allocating each type of program node, there should be a separate function
//      for freeing each tyhpe of program node.
//--------------------------------------------------------------------------------------------------
void freePNodes(PNode *pnode)
{
    while (pnode) {
        switch (pnode->type) {
            case PNICons: break;
            case PNFCons: break;
            case PNSCons:
            case PNIdent:
                stdfree(pnode->stringCons);
                break;
            case PNIf:
                freePNodes(pnode->condExpr);
                freePNodes(pnode->thenState);
                freePNodes(pnode->elseState);
                break;
            case PNWhile:
                freePNodes(pnode->condExpr);
                freePNodes(pnode->loopState);
                break;
            case PNBreak:
            case PNContinue:
                break;
            case PNReturn:
                if (pnode->returnExpr)
                    freePNodes(pnode->returnExpr);
                break;
            case PNProcDef:
                stdfree(pnode->procName);
                freePNodes(pnode->parameters);
                freePNodes(pnode->procBody);
                break;
            case PNProcCall:
                stdfree(pnode->procName);
                freePNodes(pnode->arguments);
                break;
            case PNFuncDef:
                stdfree(pnode->funcName);
                freePNodes(pnode->parameters);
                freePNodes(pnode->funcBody);
                break;
            case PNFuncCall:
            case PNBltinCall:
                stdfree(pnode->funcName);
                freePNodes(pnode->arguments);
                break;
            case PNTraverse:
                freePNodes(pnode->gnodeExpr);
                stdfree(pnode->gnodeIden);
                stdfree(pnode->levelIden);
                freePNodes(pnode->loopState);
                break;
            case PNNodes:
                freePNodes(pnode->gnodeExpr);
                stdfree(pnode->gnodeIden);
                freePNodes(pnode->loopState);
                break;
            case PNFamilies:
                freePNodes(pnode->personExpr);
                stdfree(pnode->familyIden);
                stdfree(pnode->spouseIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNSpouses:
                freePNodes(pnode->personExpr);
                stdfree(pnode->spouseIden);
                stdfree(pnode->familyIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNChildren:
                freePNodes(pnode->familyExpr);
                stdfree(pnode->childIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNIndis:
                stdfree(pnode->personIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNFams:
                stdfree(pnode->familyIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNSources:
                stdfree(pnode->sourceIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNEvents:
                stdfree(pnode->eventIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNOthers:
                stdfree(pnode->otherIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNList:
                freePNodes(pnode->listExpr);
                stdfree(pnode->elementIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNSequence:
                freePNodes(pnode->sequenceExpr);
                stdfree(pnode->elementIden);
                stdfree(pnode->valueIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNTable:
            case PNFathers:
                freePNodes(pnode->personExpr);
                stdfree(pnode->fatherIden);
                stdfree(pnode->familyIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNMothers:
                freePNodes(pnode->personExpr);
                stdfree(pnode->motherIden);
                stdfree(pnode->familyIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNFamsAsChild:
                freePNodes(pnode->personExpr);
                stdfree(pnode->familyIden);
                stdfree(pnode->countIden);
                freePNodes(pnode->loopState);
                break;
            case PNNotes:
                break;
        }
        pnode = pnode->next;
    }
}
