//
//  DeadEnds Library
//
//  interp.c holds functions that interpret DeadEnds scripts. The main function is interpret,
//  which is called on a PNode. Depending on the PNode's type, interpret will handle it directly,
//  or call a more specific function.
//
//  Created by Thomas Wetmore on 9 December 2022.
//  Last changed on 15 August 2025.
//

#include <stdarg.h>
#include "context.h"
#include "database.h"
#include "evaluate.h"
#include "file.h"
#include "frame.h"
#include "functiontable.h"
#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "interp.h"
#include "lineage.h"
#include "list.h"
#include "pnode.h"
#include "pvalue.h"
#include "sequence.h"
#include "set.h"
#include "stringset.h"
#include "symboltable.h"
#include "utils.h"

bool callTracing = false;
bool returnTracing = false;
bool symbolTableTracing = false;
bool frameTracing = false;

extern String pnodeTypes[];
extern String curFileName;
extern int curLine;

void showRuntimeStack(Context*, PNode*); // Move elsewhere?
extern void poutput(String, Context*);

bool programParsing = false;
bool programRunning = false;
bool programDebugging = false;

// Number of script errors.
int Perrors = 0;

// interpScript interprets a DeadEnds script. A script must contain procedure named "main". interpScript calls
// that procedure. This function creates a procedure call PNode to call main procecure, updates the script
// output file in the Context if the caller provides one, and then calls the PNode with the Context.
void interpScript(Context* context, File* outfile) {
    // Create a PNProcCall PNode to call the main procedure.
    curFileName = "deadends";
    curLine = 1;
    PNode* pnode = procCallPNode("main", null);
    // If there is an output file use it.
    if (outfile && context->file != outfile) {
        closeFile(context->file);
        context->file = outfile;
    }
    // Run the script by interpreting the main procedure.
    // TODO: Should look at the return code.
    (void) interpret(pnode, context, null);
}

// interpret interprets a list of PNodes. If a return statement is found it returns the return value through the
// last parameter. The language allows expressions at the statement level, so top level expressions are
// interpreted. Output is written when a statement or top level expression evaluates to a String.
InterpType interpret(PNode* pnode, Context* context, PValue* returnValue) {
    ASSERT(pnode && context);
    bool errorFlag = false;
    InterpType returnCode;
    PValue pvalue;
    while (pnode) { // Interpret the list of PNodes in the current block.
        if (programDebugging) {
            printf("interpret:%d: ", pnode->lineNumber);
            showPNode(pnode);
        }
        switch (pnode->type) {
        case PNSCons: // Strings are written.
            poutput(pnode->stringCons, context);
            break;
        case PNICons: // Numbers are ignored.
        case PNFCons:
            break;
        case PNIdent: // Idents with String values are written.
            pvalue = evaluateIdent(pnode, context);
            if (pvalue.type == PVString && pvalue.value.uString) {
                poutput(pvalue.value.uString, context);
                stdfree(pvalue.value.uString);
            }
            break;
        case PNBltinCall: // Call builtin and write return value if a String.
            pvalue = evaluateBuiltin(pnode, context, &errorFlag);
            if (errorFlag) {
                scriptError(pnode, "error calling built-in function: %s", pnode->funcName);
                return InterpError;
            }
            if (pvalue.type == PVString && pvalue.value.uString) {
                poutput(pvalue.value.uString, context);
                stdfree(pvalue.value.uString);
            }
            break;
        case PNProcCall: { // Call a builtin procedure. If for some reason it returns a String write it.
            switch (returnCode = interpProcCall(pnode, context, returnValue)) {
            case InterpOkay:
                if (returnValue && returnValue->type == PVString && returnValue->value.uString) {
                    poutput(returnValue->value.uString, context);
                    stdfree(returnValue->value.uString);
                }
                break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        }
        case PNFuncCall: // Call a user-defined function as a statement. If it returns a String write it.
            pvalue = evaluateUserFunc(pnode, context, &errorFlag);
            if (errorFlag) return InterpError;
            if (pvalue.type == PVString && pvalue.value.uString) {
                poutput(pvalue.value.uString, context);
                stdfree(pvalue.value.uString);  // The pvalue's string is in the heap.
            }
            break;
        case PNNotes: // Iterate NOTEs.
            switch (returnCode = interpFornotes(pnode, context, returnValue)) {
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
            switch (returnCode = interpChildren(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNSpouses: // Interpret spouse loop.
            switch (returnCode = interpSpouses(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFamilies: // Interpret a family loop.
            switch (returnCode = interpFamilies(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFathers: // Interpret fathers loop.
            switch (returnCode = interpFathers(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNMothers: // Interpret mothers loop.
            switch (returnCode = interpMothers(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFamsAsChild: // Interpret a familes loop.
            switch (returnCode = interpParents(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNSequence: // Interpret a Sequence loop.
            switch (returnCode = interpretSequenceLoop(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNIndis: // All persons loop.
            switch (returnCode = interpForindi(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNFams: // All families loop.
            switch (returnCode = interpForfam(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default:
                return returnCode;
            }
            break;
        case PNSources: // Source loop.
            switch (returnCode = interpForsour(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNEvents: // Event loop.
            switch (returnCode = interpForeven(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNOthers: // All other records loop.
            switch (returnCode = interpForothr(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNList: // All list elements.
            switch (returnCode = interpForList(pnode, context, returnValue)) {
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
            switch (returnCode = interp_fornodes(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNTraverse:
            switch (returnCode = interpTraverse(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNIf: // If statement.
            switch (returnCode = interpIfStatement(pnode, context, returnValue)) {
            case InterpOkay: break;
            case InterpError: return InterpError;
            default: return returnCode;
            }
            break;
        case PNWhile: // While loop.
            switch (returnCode = interpWhileStatement(pnode, context, returnValue)) {
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
            if (pnode->returnExpr)
                *returnValue = evaluate(pnode->returnExpr, context, &errorFlag);
            return InterpReturn;
        }
        pnode = pnode->next; // Move to next statement.
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
        assignValueToSymbol(context, pnode->childIden, PVALUE(PVPerson, uGNode, chil));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, nchil));
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
        assignValueToSymbol(context, pnode->spouseIden, PVALUE(PVPerson, uGNode, spouse));
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, nspouses));

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
InterpType interpFamilies(PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(pnode, "the first argument to families must be a person");
        return InterpError;
    }
    GNode *spouse = null;
    int count = 0;
    RecordIndex* index = context->database->recordIndex;
    FORFAMSS(indi, fam, key, index) {
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
        SexType sex = SEXV(indi);
        if (sex == sexMale) spouse = familyToWife(fam, index);
        else if (sex == sexFemale) spouse = familyToHusband(fam, index);
        else spouse = null;
        assignValueToSymbol(context, pnode->spouseIden, spouse ? PVALUE(PVPerson, uGNode, spouse) : nullPValue);
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, ++count));
        InterpType irc = interpret(pnode->loopState, context, pval);
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
InterpType interpFathers(PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(pnode, "the first argument to fathers must be a person");
        return InterpError;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex)
    GNode *husb = familyToHusband(fam, context->database->recordIndex);
    if (husb == null) goto d;
    assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
    assignValueToSymbol(context, pnode->fatherIden, PVALUE(PVFamily, uGNode, husb));
    assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, ++nfams));
    InterpType irc = interpret(pnode->loopState, context, pval);
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
InterpType interpMothers (PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(pnode, "the first argument to mothers must be a person");
        return InterpError;;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex) {
        GNode *wife = familyToWife(fam, context->database->recordIndex);
        if (wife == null) goto d;
        //  Assign the current loop identifier valujes to the symbol table.
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context, pnode->motherIden, PVALUE(PVFamily, uGNode, wife));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, ++nfams));

        // Intepret the body of the loop.
        InterpType irc = interpret(pnode->loopState, context, pval);
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
InterpType interpParents(PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    InterpType irc;
    GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
    if (eflg || !indi || nestr(indi->tag, "INDI")) {
        scriptError(pnode, "the first argument to parents must be a person");
        return InterpError;
    }
    int nfams = 0;
    FORFAMCS(indi, fam, key, context->database->recordIndex) {
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, fam));
        assignValueToSymbol(context, pnode->countIden,  PVALUE(PVInt, uInt, ++nfams));
        irc = interpret(pnode->loopState, context, pval);
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
InterpType interpFornotes(PNode* pnode, Context* context, PValue *pval) {
    ASSERT(pnode && context);
    bool eflg = false;
    InterpType irc;
    GNode *root = evaluateGNode(pnode->gnodeExpr, context, &eflg);
    if (eflg) {
        scriptError(pnode, "first arg of fornotes() must evaluate to a gedcom node.");
        return InterpError;
    }
    if (!root) return InterpOkay;
    FORTAGVALUES(root, "NOTE", sub, vstring) {
        assignValueToSymbol(context, pnode->noteIden, createStringPValue(vstring));
        irc = interpret(pnode->loopState, context, pval);
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
InterpType interp_fornodes(PNode* pnode, Context* context, PValue *pval) {
    bool eflg = false;
    GNode *root = evaluateGNode(pnode->gnodeExpr, context, &eflg);
    if (eflg || !root) {
        scriptError(pnode, "the first argument to fornodes must be a Gedcom node/line");
        return InterpError;
    }
    GNode *sub = root->child;
    while (sub) {
        assignValueToSymbol(context, pnode->gnodeIden, PVALUE(PVGNode, uGNode, sub));
        InterpType irc = interpret(pnode->loopState, context, pval);
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
    SymbolTable* table = context->frame->table;
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* person = getListElement(roots, i);
        assignValueToSymbol(context, pnode->personIden, PVALUE(PVPerson, uGNode, person));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(table, pnode->personIden);
    removeFromHashTable(table, pnode->countIden);
    return InterpOkay;
}

// interpForfam interprets the forfam statement that iterates over all families in the Database.
// usage: forfam(FAM_V,INT_V) {...}
InterpType interpForfam(PNode* pnode, Context* context, PValue* pvalue) {
    RootList *roots = context->database->familyRoots;
    sortList(roots);
    SymbolTable* table = context->frame->table;
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* family = getListElement(roots, i);
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, family));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(table, pnode->familyIden);
    removeFromHashTable(table, pnode->countIden);
    return InterpOkay;
}

// interpForSour interprets the forsour statement that iterates over all sources in the database.
// usage: forsour(SOUR_V, INT_V) {...}
InterpType interpForsour(PNode *pnode, Context *context, PValue *pvalue) {
    RootList *roots = context->database->sourceRoots;
    sortList(roots);
    SymbolTable* table = context->frame->table;
    for (int i = 0; i < lengthList(roots); i++) {
        GNode* source = getListElement(roots, i);
        assignValueToSymbol(context, pnode->familyIden, PVALUE(PVFamily, uGNode, source));
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(pnode->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(table, pnode->familyIden);
    removeFromHashTable(table, pnode->countIden);
    return InterpOkay;
}

// interpForeven interpret the foreven statement looping through all events in the Database.
// usage: foreven(EVEN_V,INT_V) {...}
InterpType interpForeven (PNode* node, Context* context, PValue *pvalue) {
    RootList* roots = context->database->eventRoots;
    sortList(roots);
    SymbolTable* table = context->frame->table;
    for (int i = 0; i < lengthList(roots); i++) {
        GNode *event = getListElement(roots, i);
        assignValueToSymbol(context, node->eventIden, PVALUE(PVEvent, uGNode, event));
        assignValueToSymbol(context, node->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(node->loopState, context, pvalue);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(table, node->personIden);
    removeFromHashTable(table, node->countIden);
    return InterpOkay;
}

// interpForothr Interprets the forothr statement looping through all events in the Database.
// usage: forothr(OTHR_V,INT_V) {...}
InterpType interpForothr(PNode *node, Context *context, PValue *pval) {
    SymbolTable* table = context->frame->table;
    RootList* roots = context->database->otherRoots;
    for (int i = 0; i <= lengthList(roots); i++) {
        GNode* othr = getListElement(roots, i);
        assignValueToSymbol(context, node->otherIden, PVALUE(PVEvent, uGNode, othr));
        assignValueToSymbol(context, node->countIden, PVALUE(PVInt, uInt, i));
        InterpType irc = interpret(node->loopState, context, pval);
        switch (irc) {
        case InterpContinue:
        case InterpOkay: continue;
        case InterpBreak:
        case InterpReturn: goto e;
        case InterpError: return InterpError;
        }
    }
    e:  removeFromHashTable(table, node->personIden);
    removeFromHashTable(table, node->countIden);
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
        assignValueToSymbol(context, pnode->elementIden, PVALUE(PVPerson, uGNode, indi));
        //PValue pvalue = (PValue) {PVInt, el->value}; // Update person's value in symbol table.
        //PValue pvalue = (PValue) {el->value->type, el->value->value};
        PValue pvalue = *(el->value);
        assignValueToSymbol(context, pnode->valueIden, pvalue);
        assignValueToSymbol(context, pnode->countIden, PVALUE(PVInt, uInt, ncount));
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
extern void showFrame(Frame*);
InterpType interpProcCall(PNode* pnode, Context* context, PValue* pval) {
    // Get the procedure from the procedure table.
    String name = pnode->procName;
    if (callTracing) printf("calling: %s (line %d)\n", name, pnode->lineNumber);
    PNode* proc = searchFunctionTable(context->procedures, name);
    if (!proc) {
        scriptError(pnode, "procedure %s is undefined", name);
        return InterpError;
    }

    // Create the symbol table for the called procedure.
    SymbolTable* table = createSymbolTable();

    // Bind the arguments to the parameters. Important: the arguments are evaluated using the caller's symbol
    // table, while the parameters and their values are put in the called procedure's symbol table.
    PNode* arg = pnode->arguments;
    PNode* parm = proc->parameters;
    int argcount = 1;
    while (arg && parm) {
        bool errflg = false;
        // Evaluate the argument values in the caller's context.
        PValue value = evaluate(arg, context, &errflg);
        if (errflg) {
            scriptError(pnode, "could not evaluate argument %d of %s", argcount++, name);
            return InterpError;
        }
        // Assign values to the parameters in the called procedure's symbol table.
        assignValueToSymbolTable(table, parm->identifier, value);
        arg = arg->next;
        parm = parm->next;
    }
    if (arg || parm) { // Check for argument and parameter mismatch.
        scriptError(pnode, "different numbers of arguments and parameters to %s", name);
        return InterpError;
    }

    // Create the frame for the called procedure. Add the frame to the context. Call the procedure.
    // Remove the frame from the context. Delete the frame which deletes the symbol table.
    Frame* frame = createFrame(pnode, proc, table, context->frame);
    context->frame = frame;
    InterpType returnCode = interpret(proc->procBody, context, pval);
    context->frame = frame->caller;
    deleteFrame(frame);

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
InterpType interpTraverse(PNode* pnode, Context* context, PValue* returnValue) {
    ASSERT(pnode && context);
    bool errorFlag = false;
    // Get the root node of the traversal.
    GNode* root = evaluateGNode(pnode->gnodeExpr, context, &errorFlag);
    if (errorFlag || !root) {
        scriptError(pnode, "the first argument to traverse must be a Gedcom line");
        return InterpError;
    }
    // Set up the traversal stack.
    GNode* nodeStack[MAXTRAVERSEDEPTH];
    int lev = 0;
    nodeStack[lev] = root;
    // Traverse the tree doing something.
    InterpType returnIrc = InterpOkay;
    SymbolTable* table = context->frame->table;
    while (true) {
        // Assign loop variables.
        assignValueToSymbol(context, pnode->levelIden, PVALUE(PVInt, uInt, lev));
        assignValueToSymbol(context, pnode->gnodeIden, PVALUE(PVGNode, uGNode, nodeStack[lev]));
        // Interpret loop body
        InterpType irc = interpret(pnode->loopState, context, returnValue);
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
                scriptError(pnode, "maximum traversal depth exceeded");
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
    removeFromHashTable(table, pnode->levelIden);
    removeFromHashTable(table, pnode->gnodeIden);
    return returnIrc;
}

// scriptError reports a run time script error.
void scriptError(PNode* pnode, String fmt, ...) {
    va_list args;
    printf("\nError in \"%s\" at line %d: ", pnode->fileName, pnode->lineNumber);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(".\n");
}

// showRuntimeStack shows the contents of the run time stack. If pnode is not null its line number is shown;
static StringSet* getParameterSet(PNode*);
void showFrame(Frame*);
void showRuntimeStack(Context* context, PNode* pnode) {
    // Get the bottom frame.
    Frame* frame = context->frame;
    if (!frame) return;
    printf("Run Time Stack ");
    if (pnode) {
        printf("from line %d in %s", pnode->lineNumber, frame->call->procName);
    }
    printf("\n");
    for (; frame; frame = frame->caller) {
        showFrame(frame);
    }
    printf("Global symbols:\n");
    FORHASHTABLE(context->globals, element)
        Symbol* symbol = (Symbol*) element;
        String svalue = valueOfPValue(*(symbol->value));
        String type = typeOfPValue(*(symbol->value));
        printf("    %s: %s: %s\n", symbol->ident, svalue, type);
        stdfree(svalue);
    ENDHASHTABLE
}


