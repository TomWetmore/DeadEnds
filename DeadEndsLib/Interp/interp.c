// DeadEnds
//
// interp.c holds functions that interpret DeadEnds scripts. The main function is interpret,
// which is called on a PNode. Depending on the PNode's type, interpret may handle it directly,
// or it may call a specific function.
//
// Created by Thomas Wetmore on 9 December 2022.
// Last changed on 17 May 2025.

#include <stdarg.h>
#include "symboltable.h"
#include "functiontable.h"
#include "interp.h"
#include "pnode.h"
#include "evaluate.h"
#include "lineage.h"
#include "pvalue.h"
#include "database.h"
#include "list.h"
#include "utils.h"
#include "sequence.h"

static bool debugging = false;

extern FunctionTable *procedureTable;  // User-defined procedures.
extern FunctionTable *functionTable;   // User-defined functions.
extern SymbolTable *globalTable;       // Global symbol table.
extern String pnodeTypes[];

bool programParsing = false;
bool programRunning = false;
bool programDebugging = false;

// Interface between the lexer, parser, and interpreter.
int Perrors = 0;      // Number of errors.

// initializeInterpreter initializes the interpreter.
void initializeInterpreter(Database* database) {
    Perrors = 0;
}

// createContext creates a Context from a SymbolTable and Database.
Context* createContext(SymbolTable *symbolTable, Database *database) {
    Context* context = (Context*) malloc(sizeof(Context));
    context->symbolTable = symbolTable;
    context->database = database;
    context->file = stdOutputFile();
    return context;
}

// deleteContext deletes a Context; deletes the Symboltable but not the Database.
void deleteContext(Context *context) {
    deleteHashTable(context->symbolTable);
    free(context);
}

// finishInterpreter is called when the interpreter is done; currently a no-op.
void finishInterpreter(void) { }

//  remove_tables - Remove the interpreter's tables when no longer needed.
//--------------------------------------------------------------------------------------------------
//static void remove_tables(void)
//{
//    // The node block cleaner will free pnodes in procedureTable. TODO: FIGURE OUT WHAT THAT MEANS.
//    deleteHashTable(procedureTable);
//    // TODO: The lexer saved global names so they need to be freed.
//    deleteHashTable(globalTable);
//    // TODO: Are local variable IDENs leaking.
//    deleteHashTable(functionTable);
//}

// interpret interprets a list of PNodes. If a return statement is found it returns the return
// value through the last parameter. The language allows expressions at the statement level, so
// top level expressions are interpreted. Output is written when a statement or top level
// expression evaluates to a String.
InterpType interpret(PNode* programNode, Context* context, PValue* returnValue) {
    ASSERT(programNode && context);
    bool errorFlag = false;
    InterpType returnCode;
    PValue pvalue;
    //FILE* fp = context->file->fp;
    while (programNode) { // Iterate the PNodes in list to interpret.
        if (programDebugging) {
            printf("interpret:%d: ", programNode->lineNumber);
            showPNode(programNode);
        }
        switch (programNode->type) {
        case PNSCons: // Strings are written.
            fprintf(context->file->fp, "%s", (String) programNode->stringCons);
            break;
        case PNICons: // Numbers are ignored.
        case PNFCons:
            break;
        case PNIdent: // Idents with String values are written.
            pvalue = evaluateIdent(programNode, context);
            if (pvalue.type == PVString && pvalue.value.uString)
                fprintf(context->file->fp, "%s", pvalue.value.uString);
            break;
        case PNBltinCall: // Call builtin and write return value if a String.
            pvalue = evaluateBuiltin(programNode, context, &errorFlag);
            if (errorFlag) {
                scriptError(programNode, "error calling built-in function: %s", programNode->funcName);
                return InterpError;
            }
            if (pvalue.type == PVString && pvalue.value.uString) {
                fprintf(context->file->fp, "%s", pvalue.value.uString);
                stdfree(pvalue.value.uString);
            }
            break;
        case PNProcCall: {
            switch (returnCode = interpProcCall(programNode, context, returnValue)) {
            case InterpOkay:
                if (returnValue && returnValue->type == PVString && returnValue->value.uString) {
                    fprintf(context->file->fp, "%s", returnValue->value.uString);
                    stdfree(returnValue->value.uString);
                }
                break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        }
        case PNFuncCall: // Call a user-defined function at statement level.
            pvalue = evaluateUserFunc(programNode, context, &errorFlag);
            if (errorFlag) return InterpError;
            if (pvalue.type == PVString && pvalue.value.uString) {
                fprintf(context->file->fp, "%s", pvalue.value.uString);
                stdfree(pvalue.value.uString);  // The pvalue's string is in the heap.
            }
            break;
        case PNNotes: // Iterate NOTEs.
            switch (returnCode = interpFornotes(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFuncDef: // Illegal during interpretation.
        case PNProcDef:
        case PNTable:
            FATAL();
        case PNChildren: // Interpret children loop.
            switch (returnCode = interpChildren(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNSpouses: // Interpret spouse loop.
            switch (returnCode = interpSpouses(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFamilies: // Interpret a family loop.
            switch (returnCode = interpFamilies(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFathers: // Interpret fathers loop.
            switch (returnCode = interpFathers(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNMothers: // Interpret mothers loop.
            switch (returnCode = interpMothers(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFamsAsChild: // Interpret a familes loop.
            switch (returnCode = interpParents(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNSequence: // Interpret a Sequence loop.
            switch (returnCode = interpretSequenceLoop(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNIndis: // All persons loop.
            switch (returnCode = interpForindi(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFams: // All families loop.
            switch (returnCode = interpForfam(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default:
                return returnCode;
            }
            break;
        case PNSources: // Source loop.
            switch (returnCode = interpForsour(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNEvents: // Event loop.
            switch (returnCode = interpForeven(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNOthers: // All other records loop.
            switch (returnCode = interpForothr(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNList: // All list elements.
            switch (returnCode = interpForList(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
            //            case INOTES:
            //                switch (irc = interp_fornotes(node, stab, pval)) {
            //                    case INTOKAY:
            //                        break;
            //                    case INTERROR:

            //                        return INTERROR;
            //                    default:
            //                        return irc;
            //                }
            //                break;
        case PNNodes:
            switch (returnCode = interp_fornodes(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNTraverse:
            switch (returnCode = interpTraverse(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNIf: // If statement.
            switch (returnCode = interpIfStatement(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNWhile: // While loop.
            switch (returnCode = interpWhileStatement(programNode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNBreak: // Break statement.
            return InterpBreak;
        case PNContinue: // Continue statement.
            return InterpContinue;
        case PNReturn: // Return statement.
            if (programNode->returnExpr)
                *returnValue = evaluate(programNode->returnExpr, context, &errorFlag);
            return InterpReturn;
        }
        programNode = programNode->next; // Move to next statement.
    }
    return InterpOkay;
}

// interpChildren interprets the children loop statement, looping through the children of a family.
// usage: children(FAM, INDI_V, INT_V) {...}
// fields: pFamilyExpr, pChildIden, pCountIden, pLoopState
InterpType interpChildren (PNode* pnode, Context* context, PValue* pval) {
    bool eflg = false;
    GNode *fam =  evaluateFamily(pnode->familyExpr, context, &eflg);
    if (eflg || !fam || nestr(fam->tag, "FAM")) {
        scriptError(pnode, "the first argument to children must be a family");
        return InterpError;
    }
    FORCHILDREN(fam, chil, key, nchil, context->database->recordIndex) {
        assignValueToSymbol(context->symbolTable, pnode->childIden, PVALUE(PVPerson, uGNode, chil));
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, nchil));
        InterpType irc = interpret(pnode->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: goto a;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        a:	;
    } ENDCHILDREN
    return InterpOkay;
}

// interpSpouses interpret the spouses statement looping through the spouses of a person.
// usage: spouses(INDI, INDI_V, FAM_V, INT_V) {...}
// fields: pPersonExpr, pSpouseIden, pFamilyIden, pCountIden, pLoopState
InterpType interpSpouses(PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(pnode, "the first argument to spouses must be a person");
        return InterpError;
    }
    FORSPOUSES(indi, spouse, fam, nspouses, context->database->recordIndex) {
        assignValueToSymbol(context->symbolTable, pnode->spouseIden, PVALUE(PVPerson, uGNode, spouse));
        assignValueToSymbol(context->symbolTable, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, nspouses));

        InterpType irc = interpret(pnode->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: goto b;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        b:	;
    } ENDSPOUSES
    return InterpOkay;
}

// interpFamilies interprets the families statement looping through the families a person is a spouse in.
// usage: families(INDI, FAM_V, INDI_V, INT_V) {...}
// fields: pPersonExpr, pFamilyIden, pSpouseIden, pCountIden, pLoopState
InterpType interpFamilies(PNode* node, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(node, "the first argument to families must be a person");
        return InterpError;
    }
    GNode *spouse = null;
    int count = 0;
    //Database *database = context->database;
    RecordIndex* index = context->database->recordIndex;
    FORFAMSS(indi, fam, key, index) {
        assignValueToSymbol(context->symbolTable, node->familyIden, PVALUE(PVFamily, uGNode, fam));
        SexType sex = SEXV(indi);
        if (sex == sexMale) spouse = familyToWife(fam, index);
        else if (sex == sexFemale) spouse = familyToHusband(fam, index);
        else spouse = null;
        assignValueToSymbol(context->symbolTable, node->spouseIden, PVALUE(PVPerson, uGNode, spouse));
        assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, ++count));
        InterpType irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: goto c;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        c:	;
    }
    ENDFAMSS
    return InterpOkay;
}

// interpFathers interprets the father loop statement. Most persons will only have one father in a
// database, so most of the time the loop body is interpreted once.
InterpType interpFathers(PNode* node, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(node, "the first argument to fathers must be a person");
        return InterpError;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex)
    GNode *husb = familyToHusband(fam, context->database->recordIndex);
    if (husb == null) goto d;
    assignValueToSymbol(context->symbolTable, node->familyIden, PVALUE(PVFamily, uGNode, fam));
    assignValueToSymbol(context->symbolTable, node->fatherIden, PVALUE(PVFamily, uGNode, husb));
    assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, ++nfams));
    InterpType irc = interpret(node->loopState, context, pval);
    switch (irc) {
    case InterpContinue:
    case InterpOkay: goto d;
    case InterpBreak: return InterpOkay;
    default: return irc;
    }
    d:	    ;
    ENDFAMCS
    return InterpOkay;
}

// interpMothers interprets the mother loop statement. Most persons will only have one mother in a
// database, so most of the time the loop body is interpreted once.
InterpType interpMothers (PNode* node, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(node, "the first argument to mothers must be a person");
        return InterpError;;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex) {
        GNode *wife = familyToWife(fam, context->database->recordIndex);
        if (wife == null) goto d;
        //  Assign the current loop identifier valujes to the symbol table.
        assignValueToSymbol(context->symbolTable, node->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context->symbolTable, node->motherIden, PVALUE(PVFamily, uGNode, wife));
        assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, ++nfams));

        // Intepret the body of the loop.
        InterpType irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: goto d;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        d:        ;
    }  ENDFAMCS
    return InterpOkay;
}

// interpParents -- Interpret parents loop; this loops over all families a person is a child in.
// TODO: Does this exist in LifeLines?
InterpType interpParents(PNode* node, Context* context, PValue *pval) {
    bool eflg = false;
    InterpType irc;
    GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(node, "the first argument to parents must be a person");
        return InterpError;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex) {
        assignValueToSymbol(context->symbolTable, node->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context->symbolTable, node->countIden,  PVALUE(PVInt, uInt, ++nfams));
        irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: goto f;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        f:	;
    }
    ENDFAMCS
    return InterpOkay;
}

// interp_fornotes interprets the fornote loop.
InterpType interpFornotes(PNode* node, Context* context, PValue *pval) {
    ASSERT(node && context);
    bool eflg = false;
    InterpType irc;
    GNode *root = evaluateGNode(node->gnodeExpr, context, &eflg);
    if (eflg) {
        scriptError(node, "first arg of fornotes() must evaluate to a gedcom node.");
        return InterpError;
    }
    if (!root) return InterpOkay;
    FORTAGVALUES(root, "NOTE", sub, vstring) {
        assignValueToSymbol(context->symbolTable, node->gnodeIden, createStringPValue(vstring));
        irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay:
            goto g;
        case InterpBreak:
            return InterpOkay;
        default:
            return irc;
        }
        g:      ;
    } ENDTAGVALUES
    return InterpOkay;
}

// interp_fornodes interpret the fornodes statement looping though the children of a GNode.
// usage: fornodes(NODE, NODE_V) {...}; fields: pGNodeExpr, pNodeIden, pLoopState
InterpType interp_fornodes(PNode* node, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *root = evaluateGNode(node->gnodeExpr, context, &eflg);
    if (eflg || !root) {
        scriptError(node, "the first argument to fornodes must be a Gedcom node/line");
        return InterpError;
    }
    GNode *sub = root->child;
    while (sub) {
        assignValueToSymbol(context->symbolTable, node->gnodeIden, PVALUE(PVGNode, uGNode, sub));
        InterpType irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay:
            sub = sub->sibling;
            continue;
        case InterpBreak: return InterpOkay;
        default:
            return irc;
        }
    }
    return InterpOkay;
}

// interpForindi interprets the forindi statement looping through all persons in the Database.
// Usage: forindi(INDI_V, INT_V) {...}; Fields: personIden, countIden, loopState.
InterpType interpForindi (PNode* pnode, Context* context, PValue* pvalue) {
    RootList *roots = context->database->personRoots;
    sortList(roots);
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* person = getListElement(roots, i);
        assignValueToSymbol(context->symbolTable, pnode->personIden, PVALUE(PVPerson, uGNode, person));
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(context->symbolTable, pnode->personIden);
    removeFromHashTable(context->symbolTable, pnode->countIden);
    return InterpOkay;
}

// interpForfam interprets the forfam statement that iterates over all families in the Database.
// usage: forfam(FAM_V,INT_V) {...}
InterpType interpForfam(PNode* pnode, Context* context, PValue* pvalue) {
    RootList *roots = context->database->familyRoots;
    sortList(roots);
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* family = getListElement(roots, i);
        assignValueToSymbol(context->symbolTable, pnode->familyIden, PVALUE(PVFamily, uGNode, family));
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(context->symbolTable, pnode->familyIden);
    removeFromHashTable(context->symbolTable, pnode->countIden);
    return InterpOkay;
}

// interpForSour interprets the forsour statement that iterates over all sources in the database.
// usage: forsour(SOUR_V, INT_V) {...}
InterpType interpForsour(PNode *pnode, Context *context, PValue *pvalue) {
    RootList *roots = context->database->sourceRoots;
    sortList(roots);
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* source = getListElement(roots, i);
        assignValueToSymbol(context->symbolTable, pnode->familyIden, PVALUE(PVFamily, uGNode, source));
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(context->symbolTable, pnode->familyIden);
    removeFromHashTable(context->symbolTable, pnode->countIden);
    return InterpOkay;
}

// interpForeven interpret the foreven statement looping through all events in the Database.
// usage: foreven(EVEN_V,INT_V) {...}
InterpType interpForeven (PNode* node, Context* context, PValue *pvalue) {
    RootList* roots = context->database->eventRoots;
    sortList(roots);
    for (int i = 0; i < lengthList(roots); i++) {
        GNode *event = getListElement(roots, i);
        assignValueToSymbol(context->symbolTable, node->eventIden, PVALUE(PVEvent, uGNode, event));
        assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(node->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(context->symbolTable, node->personIden);
    removeFromHashTable(context->symbolTable, node->countIden);
    return InterpOkay;
}

// interpForothr Interprets the forothr statement looping through all events in the Database.
// usage: forothr(OTHR_V,INT_V) {...}
InterpType interpForothr(PNode *node, Context *context, PValue *pval) {
    RootList* roots = context->database->otherRoots;
    for (int i = 0; i <= lengthList(roots); i++) {
        GNode* othr = getListElement(roots, i);
        assignValueToSymbol(context->symbolTable, node->otherIden, PVALUE(PVEvent, uGNode, othr));
        assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(context->symbolTable, node->personIden);
    removeFromHashTable(context->symbolTable, node->countIden);
    return InterpOkay;
    return InterpOkay;
}

// interpretSequenceLoop interprets a script sequence loop.
// usage: forindiset(SET, INDI_V, ANY_V, INT_V) { }
// fields: sequenceExpr, pElementIden, pCountIden, pLoopState
InterpType interpretSequenceLoop(PNode* pnode, Context* context, PValue* pval) {
    bool eflg = false;
    InterpType irc;
    PValue val = evaluate(pnode->sequenceExpr, context, &eflg);
    if (eflg || val.type != PVSequence) {
        scriptError(pnode, "the first argument to forindiset must be a set");
        return InterpError;
    }
    Sequence *seq = val.value.uSequence;
    RecordIndex* index = context->database->recordIndex;
    FORSEQUENCE(seq, el, ncount) {
        GNode *indi = keyToPerson(el->root->key, index); // Update person in symbol table.
        assignValueToSymbol(context->symbolTable, pnode->elementIden, PVALUE(PVPerson, uGNode, indi));
        PValue pvalue = (PValue) {PVInt, el->value}; // Update person's value in symbol table.
        assignValueToSymbol(context->symbolTable, pnode->valueIden, pvalue);
        assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, ncount));
        switch (irc = interpret(pnode->loopState, context, pval)) {
        case InterpContinue:
        case InterpOkay: goto h;
        case InterpBreak: return InterpOkay;
        default: return irc;
        }
        h:	;
    }
    ENDSEQUENCE
    return InterpOkay;
}

// interpIfStatement interprets an if statement.
// usage: if ([VAR,] COND) { THEN } [{ else ELSE }]; fields: condExpr, thenState, elseState
InterpType interpIfStatement(PNode* pnode, Context* context, PValue* rvalue) {
    ASSERT(pnode && pnode->type == PNIf && context);
    bool eflg = false;
    bool cond = evaluateConditional(pnode->condExpr, context, &eflg);
    if (eflg) return InterpError;
    if (cond) return interpret(pnode->thenState, context, rvalue);
    if (pnode->elseState) return interpret(pnode->elseState, context, rvalue);
    return InterpOkay;
}

// interpWhileStatement interprets a while statement.
// usage: while ([VAR,] COND) { BODY }; fields: condExpr. loopstaate
InterpType interpWhileStatement (PNode* pnode, Context* context, PValue* pval) {
    ASSERT(pnode && pnode->type == PNWhile && context);
    bool eflg = false;
    while (true) {
        bool cond = evaluateConditional(pnode->condExpr, context, &eflg);
        if (eflg) return InterpError;
        if (!cond) return InterpOkay;
        InterpType irc;
        switch (irc = interpret(pnode->loopState, context, pval)) {
        case InterpContinue:
        case InterpOkay:
            continue;
        case InterpBreak:
            return InterpOkay;
        default:
            return irc;
        }
    }
}

// interpProcCall interprets a user-defined procedure call.
InterpType interpProcCall(PNode* pnode, Context* context, PValue* pval) {
    ASSERT(pnode && pnode->type == PNProcCall && context);
    if (programDebugging) {
        printf("interpProcCall: %d: %s: %2.3f\n", pnode->lineNumber, pnode->procName,
               getMseconds());
    }
    PNode* procedure = searchFunctionTable(procedureTable, pnode->procName);
    if (!procedure) {
        printf("``%s'': undefined procedure\n", pnode->procName);
        return InterpError;
    }
    SymbolTable* newSymbolTable = createSymbolTable(); // Context to run proc in.
    Context* newContext = createContext(newSymbolTable, context->database);
    newContext->file = context->file; // Propogate output file.
    PNode* argument = pnode->arguments; // First arg.
    PNode* parameter = procedure->parameters; // First param.
    while (argument && parameter) { // Bind args and params.
        bool eflg = false;
        PValue value = evaluate(argument, context, &eflg); // Eval arg.
        if (eflg) return InterpError;
        assignValueToSymbol(newSymbolTable, parameter->identifier, value); // Assign arg to param.
        argument = argument->next;
        parameter = parameter->next;
    }
    if (argument || parameter) { // Check for arg/param mismatch.
        printf("``%s'': mismatched args and params\n", pnode->procName);
        deleteContext(newContext);
        return InterpError;
    }
    if (debugging) {
        printf("Symbol Table after binding args and parms:\n");
        showSymbolTable(newSymbolTable);
    }
    InterpType returnCode = interpret(procedure->procBody, newContext, pval); // Interpret body.
    deleteContext(newContext);
    switch (returnCode) {
    case InterpReturn:
    case InterpOkay: return InterpOkay;
    case InterpBreak:
    case InterpContinue:
    case InterpError:
    default: return InterpError;
    }
    return InterpError;
}

// interpTraverse interprets the traverse statement. It adds two entries to the symbol table.
// Usage: traverse(GNode expr, GNode ident, int ident) {...}.
// Fields: gnodeExpr, levelIden, gNodeIden.
#define MAXTRAVERSEDEPTH 100
InterpType interpTraverse(PNode* traverseNode, Context* context, PValue* returnValue) {
    ASSERT(traverseNode && context);
    bool errorFlag = false;
    // Get the root node of the traversal.
    GNode* root = evaluateGNode(traverseNode->gnodeExpr, context, &errorFlag);
    if (errorFlag || !root) {
        scriptError(traverseNode, "the first argument to traverse must be a Gedcom line");
        return InterpError;
    }
    // Set up the traversal stack.
    GNode* nodeStack[MAXTRAVERSEDEPTH];
    int lev = 0;
    nodeStack[lev] = root;
    // Traverse the tree doing something.
    InterpType returnIrc = InterpOkay;
    while (true) {
        // Assign loop variables.
        assignValueToSymbol(context->symbolTable, traverseNode->levelIden, PVALUE(PVInt, uInt, lev));
        assignValueToSymbol(context->symbolTable, traverseNode->gnodeIden, PVALUE(PVGNode, uGNode, nodeStack[lev]));
        // Interpret loop body
        InterpType irc = interpret(traverseNode->loopState, context, returnValue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay:
            break;
        case InterpBreak:
            returnIrc = InterpOkay;
            goto cleanup;
        default:
            returnIrc = irc;
            goto cleanup;
        }
        // Traverse to first child.
        if (nodeStack[lev]->child) {
            if (lev + 1 >= MAXTRAVERSEDEPTH) {
                scriptError(traverseNode, "maximum traversal depth exceeded");
                returnIrc = InterpError;
                goto cleanup;
            }
            nodeStack[lev + 1] = nodeStack[lev]->child;
            lev++;
            continue;
        }
        // Traverse to sibling.
        if (nodeStack[lev]->sibling) {
            nodeStack[lev] = nodeStack[lev]->sibling;
            continue;
        }
        // Backup to ancestor with unvisited sibling.
        while (--lev >= 0 && !(nodeStack[lev]->sibling)) { }
        if (lev < 0) break;
        nodeStack[lev] = nodeStack[lev]->sibling;
    }

cleanup:
    removeFromHashTable(context->symbolTable, traverseNode->levelIden);
    removeFromHashTable(context->symbolTable, traverseNode->gnodeIden);
    return returnIrc;
}

// scriptError reports a run time script error.
void scriptError(PNode* gnode, String fmt, ...) {
    va_list args;
    printf("\nError in \"%s\" at line %d: ", gnode->fileName, gnode->lineNumber);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(".\n");
}
