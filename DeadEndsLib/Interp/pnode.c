// DeadEnds
//
// pnode.c holds the functions that manage PNodes (program nodes).
//
// Created by Thomas Wetmore on 14 December 2022.
// Last changed on 10 May 2025.

#include "pnode.h"
#include "standard.h"
#include "hashtable.h"
#include "functiontable.h"
#include "gedcom.h"
#include "interp.h"

static bool debugging = false;

// pnodeTypes are String names for the program node types useful for debugging.
String pnodeTypes[] = {
    "", "ICons", "FCons", "SCons", "Ident", "If", "While", "Break", "Continue", "Return",
    "ProcDef", "ProcCall", "FuncDef", "FuncCall", "BltinCall", "Traverse", "Nodes", "Families",
    "Spouses", "Children", "Indis", "Fams", "Sources", "Events", "Others", "List", "Set",
    "Fathers", "Mothers", "FamsAsChild", "Notes"
};

extern FunctionTable *functionTable;  // parse.c
extern String curFileName;  // parse.c; used to set the file name of PNodes.
extern int curLine;   // parse.c; used to set the line numbers of PNodes.

// showPNode shows a PNode useful for debugging.
void showPNode(PNode* pnode) {
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

static void setParents(PNode* list, PNode* parent); // Set the parents of a PNode list.

// allocPNode allocates a PNode and sets the pType, pFileName and pLineNum fields.
static PNode* allocPNode(int type) {
    PNode* node = (PNode*) stdalloc(sizeof(*node));
    if (debugging) {
        printf("allocPNode(%d) %s, %d\n", type, curFileName, curLine);
    }
    node->type = type;
    node->fileName = strsave(curFileName); // TODO: MEMORY!!!!!!!!!
    node->lineNumber = curLine; // Overwritten by the yacc m production?
    return node;
}

// iconsPNode creates an integer PNode with a C long.
PNode* iconsPNode(long intConstant) {
    PNode *node = allocPNode(PNICons);
    node->intCons = intConstant;
    return node;
}

// fconsPNode creates a float PNode with a C double.
PNode* fconsPNode(double floatConstant) {
    PNode *pnode = allocPNode(PNFCons);
    pnode->floatCons = floatConstant;
    return pnode;
}

// sconsPNode creates a String PNode.
PNode* sconsPNode(String string) {
    PNode *pnode = allocPNode(PNSCons);
    pnode->stringCons = string;
    return pnode;
}

// ifPNode creates an if statement PNode.
PNode* ifPNode (PNode* cond, PNode* tnode, PNode* enode) {
    PNode *pnode = allocPNode(PNIf);
    pnode->condExpr = cond;
    pnode->thenState = tnode;
    pnode->elseState = enode;
    setParents(tnode, pnode);
    setParents(enode, pnode);
    return pnode;
}

// whilePNode creates a while loop PNode.
PNode* whilePNode(PNode* cond, PNode* body) {
    PNode *node = allocPNode(PNWhile);
    node->condExpr = cond;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// breakPNode creates a break PNode.
PNode* breakPNode(void) { return allocPNode(PNBreak); }

// continuePNode creates a continue PNode.
PNode *continuePNode(void) { return allocPNode(PNContinue); }

// returnPNode create a return PNode with optional return expression.
PNode* returnPNode(PNode *args) {
    PNode *node = allocPNode(PNReturn);
    if (args) { node->returnExpr = args; }
    return node;
}

// procDefPNode creates a user-defined proc definition node.
PNode* procDefPNode(String name, PNode* parms, PNode* body) {
    PNode *node = allocPNode(PNProcDef);
    node->procName = name;
    node->parameters = parms;
    node->procBody = body;
    setParents(body, node);
    return node;
}

// procCallPNode creates a user-defined proc call node.
PNode* procCallPNode(String name, PNode *args) {
    PNode *node = allocPNode(PNProcCall);
    node->procName = strsave(name);
    node->arguments = args;
    return node;
}

// funcDefPNode creates a user-defined function definition node.
PNode* funcDefPNode(String name, PNode* parms, PNode* body) {
    PNode *node = allocPNode(PNFuncDef);
    node->funcName = name;
    node->parameters = parms;
    node->funcBody = body;
    setParents(body, node);
    return node;
}

// funcCallPNode creates a builtin or user-defined function call program node.
PNode* funcCallPNode(String name, PNode* alist) {
    if (isInHashTable(functionTable, name)) { // User-defined.
        PNode *node = allocPNode(PNFuncCall);
        node->funcName = name;
        node->arguments = alist;
        node->funcBody = searchFunctionTable(functionTable, name);
        return node;
    }

    // Not user-defined; should be a builtin.
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
    if (found) { // Is a builtin
        int n;
        if ((n = num_params(alist)) < builtIns[md].minParams || n > builtIns[md].maxParams) {
            printf("%s: must have %d to %d parameters.\n", name,
                   builtIns[md].minParams, builtIns[md].maxParams);
            Perrors++;
        }
        PNode *node = allocPNode(PNBltinCall);
        node->funcName = name;
        node->arguments = alist;
        node->builtinFunc = builtIns[md].func;
        return node;
    }
    // Function is undefined; assume it will be later.
    PNode *node = allocPNode(PNFuncCall);
    node->funcName = name;
    node->parameters = alist;
    node->funcBody = null;
    return node;
}

// traversePNode creates a traverse loop PNode to traverses a tree of GNodes. snode and levv are
// the names of the idents holding the current node's name and the current level.
PNode* traversePNode (PNode* nexpr, String snode, String levv, PNode* body) {
    PNode *node = allocPNode(PNTraverse);
    node->gnodeExpr = nexpr;
    node->gnodeIden = snode;
    node->levelIden = levv;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// fornodesPNode creates a fornodes loop PNode to iterate the children of a GNode. nvar is the
// name of the ident holding the current child's name.
PNode* fornodesPNode (PNode* nexpr, String nvar, PNode* body) {
    PNode *node = allocPNode(PNNodes);
    node->gnodeExpr = nexpr;
    node->gnodeIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// familiesPNode creates a family loop PNode to loop through the families a person is a spouse in.
// fvar is the family ident; svar is the spouse ident; nvar is the counter ident.
PNode* familiesPNode(PNode* pexpr, String fvar, String svar, String count, PNode* body) {
    PNode *node = allocPNode(PNFamilies);
    node->personExpr = pexpr;
    node->familyIden = fvar;
    node->spouseIden = svar;
    node->countIden = count;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// spousesPNode create a spouse loop PNode that loops through the spouses of a person. svar is
// spouse ident; fvar is the family ident; nvar is the counter ident.
PNode* spousesPNode (PNode* pexpr, String svar, String fvar, String count, PNode* body) {
    PNode *node = allocPNode(PNSpouses);
    node->personExpr = pexpr;
    node->spouseIden = svar;
    node->familyIden = fvar;
    node->countIden = count;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// childrenPNode creates a children loop PNode that loops through the chidren of a family.
// Called by yyparse() on rule reduction; fexpr is the family expression; cvar is the child
// loop var; nvar is the counter; body is the root PNode of the loop body.
PNode *childrenPNode (PNode *fexpr, String cvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNChildren);
    node->familyExpr = fexpr;
    node->childIden = cvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forindiPNode create a forindi PNode loop to iterate every person in a database. Called by yyparse on rule
// reduction; ivar is the person identifier; nvar is the counter identifier; and body is the PNode loop body.
PNode *forindiPNode (String ivar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNIndis);
    node->personIden = ivar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forfamPNode creates a forfam PNode loop to iterate every family in a database. Called by yyparse on rule
// reduction; fvar is the family identifier; nvar is the counter identifier, and body is the PNode loop body.
PNode *forfamPNode (String fvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNFams);
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forsourPNode create a forsour PNode loop that iterates every source in a database.
PNode *forsourPNode (String svar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNSources);
    node->sourceIden = svar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forevenPNode creates a foreven PNode loop that iterates every event in a database.
PNode *forevenPNode(String evar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNEvents);
    node->eventIden = evar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forothrPNode creates a forothr PNode loop that iterates every other record in a database.
PNode *forothrPNode (String ovar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNOthers);
    node->otherIden = ovar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forlistPNode creates a list PNode loop that iterates every element in a list.
PNode *forlistPNode (PNode *lexpr, String evar, String nvar, PNode *body)
// iexpr -- PNode expression that evaluates to a List.
{
    PNode *node = allocPNode(PNList);
    node->listExpr = lexpr;
    node->elementIden = (void*) evar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// forindisetPNode creates a sequence loop.
PNode *forindisetPNode(PNode *iexpr, String ivar, String vvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNSequence);
    node->sequenceExpr = iexpr;
    node->elementIden = ivar;
    node->valueIden = vvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// fornotesPNode creates fornotes loop node. nexpr evaluates to a GNode. vvar is the variable that will hold NOTE
// values; body is the loop.
PNode *fornotesPNode(PNode *nexpr, String vvar, PNode *body) {
    PNode *pnode = allocPNode(PNNotes);
    pnode->gnodeExpr = nexpr;
    pnode->noteIden = vvar;
    pnode->loopState = body;
    setParents(body, pnode);
    return pnode;
}

// iden_node -- Create an identifier PNode.
// TODO: FIGURE OUTWHAT TO DO WITH THIS.
PNode *iden_node(String identifier) {
    PNode *pnode = allocPNode(PNIdent);
    pnode->identifier = identifier;
    return pnode;
}

// fathersPNode creates fathers loop node.
PNode *fathersPNode(PNode *pexpr, String pvar, String fvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNFathers);
    node->personExpr = pexpr;
    node->fatherIden = pvar;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// mothersPNode creates a mothers loop node.
PNode *mothersPNode(PNode *pexpr, String pvar, String fvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNMothers);
    node->personExpr = pexpr;
    node->motherIden = pvar;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// parentsPNode creates a parents loop node.
PNode *parentsPNode(PNode *pexpr, String fvar, String nvar, PNode *body) {
    PNode *node = allocPNode(PNFamsAsChild);
    node->personExpr = pexpr;
    node->familyIden = fvar;
    node->countIden = nvar;
    node->loopState = body;
    setParents(body, node);
    return node;
}

// setParents sets the parent node for a list of nodes.
static void setParents (PNode *list, PNode *parent) {
    while (list) {
        list->parent = parent;
        list = list->next;
    }
}

// freePNodes frees the program nodes rooted at the given program node.
void freePNodes(PNode *pnode) {
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
        case PNTable: break; // TODO: FINISH TABLE IMPLEMENTATION.
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
            break; // TODO: NOT FULLY IMPLEMENTED.
        default:
            fprintf(stderr, "Warning: Unknown PNode type %d in freePNodes\n", pnode->type);
            ASSERT(false);
        }
        PNode* save = pnode;
        pnode = pnode->next;
        stdfree(save);
    }
}
