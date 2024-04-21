// DeadEnds
//
// interp.c holds the functions that interpret DeadEnds scripts. The top function is interpret,
// which is called on a sequence of PNodes. Depending on the type of PNode, interpret may handle
// the node directly, or it may call a specific function.
//
// Created by Thomas Wetmore on 9 December 2022.
// Last changed on 18 April 2024.

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
FILE *Poutfp = null;  // Output file.
int Perrors = 0;      // Number of errors.

//String ierror = (String) "Error: file \"%s\": line %d: ";

// initializeInterpreter initializes the interpreter.
void initializeInterpreter(Database *database) {
	Perrors = 0;
}

// createContext creates a Context from a SymbolTable and Database.
Context *createContext(SymbolTable *symbolTable, Database *database) {
	Context *context = (Context*) malloc(sizeof(Context));
	context->symbolTable = symbolTable;
	context->database = database;
	return context;
}

// deleteContext deletes a Context.
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

// interpret interprets a list of program nodes. If a return statement is encountered function
// returns with the return value as last parameter. The language allows expressions at the
// statement level, so top level expressions are interpreted. Output goes to the output file when
// any statement or top level expression evaluates to a String. Recurses. programNode is the first
// PNode in the list of nodes to interpret.
InterpType interpret(PNode *programNode, Context *context, PValue *returnValue) {
	ASSERT(programNode && context);
	bool errorFlag = false;
	InterpType returnCode;
	PValue pvalue;

	while (programNode) { // Iterate the PNodes in list to interpret.
		if (programDebugging) {
			printf("interpret:%d: ", programNode->lineNumber);
			showPNode(programNode);
		}
		switch (programNode->type) {
			case PNSCons: // Strings are written to output.
				printf("%s", (String) programNode->stringCons);
				break;
			case PNICons: // Numeric constants are ignored.
			case PNFCons:
				break;
			case PNIdent: // Idents with String values are written to output.
				pvalue = evaluateIdent(programNode, context, &errorFlag);
				if (errorFlag) {
					scriptError(programNode, "error evaluating an identifier");
					return InterpError;
				}
				if (pvalue.type == PVString && pvalue.value.uString)
					printf("%s", pvalue.value.uString);
				break;
			case PNBltinCall: // Call builtins and print return value if it is a String.
				pvalue = evaluateBuiltin(programNode, context, &errorFlag);
				if (errorFlag) {
					scriptError(programNode, "error calling built-in function: %s", programNode->funcName);
					return InterpError;
				}
				if (pvalue.type == PVString && pvalue.value.uString)
					printf("%s", pvalue.value.uString);
				break;
			case PNProcCall: // Call user-defined procedure.
			   switch (returnCode = interpProcCall(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNFuncCall: // Call a user-defined function at the statement level.
				pvalue = evaluateUserFunc(programNode, context, &errorFlag);
				if (errorFlag) return InterpError;
				if (pvalue.type == PVString && pvalue.value.uString) {
					printf("%s", pvalue.value.uString);
					stdfree(pvalue.value.uString);  // The pvalue's string is in the heap.
				}
				break;
			case PNFuncDef: // These types are illegal during interpretation.
			case PNProcDef:
			case PNTable:
			case PNNotes:  //  Maybe this is legitimate?
				FATAL();
			case PNChildren: // Interpret a children loop.
				switch (returnCode = interpChildren(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNSpouses: // Interpret a spouses loop.
				switch (returnCode = interpSpouses(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNFamilies: // Interpret a families loop.
				switch (returnCode = interpFamilies(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNFathers: // Interpret a fathers loop.
				switch (returnCode = interpFathers(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNMothers: // Interpret a mothers loop.
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
			case PNIndis: // Interpret an all persons loop.
				switch (returnCode = interpForindi(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNFams: // Interpret an all families loop.
				switch (returnCode = interpForFam(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default:
						return returnCode;
				}
				break;

			//  Iterate through all sources in the database.
			case PNSources:
				switch (returnCode = interp_forsour(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;

			//  Iterate through all events in the database.
			case PNEvents:
				switch (returnCode = interp_foreven(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNOthers: // Iterate through all sources in the database.
				switch (returnCode = interp_forothr(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;

			//  Loop through all elements of a list. All elements will be PValue*'s.
			case PNList:
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
			case PNIf: // Interpret an if statement.
				switch (returnCode = interpIfStatement(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNWhile: // Interpret a while loop.
				switch (returnCode = interpWhileStatement(programNode, context, returnValue)) {
					case InterpOkay: break;
					case InterpError: return InterpError;
					default: return returnCode;
				}
				break;
			case PNBreak: // Interpret a break statement.
				return InterpBreak;
			case PNContinue: // Interpret a continue statement.
				return InterpContinue;
			case PNReturn: // Interpret a return statement.
				if (programNode->returnExpr)
					*returnValue = evaluate(programNode->returnExpr, context, &errorFlag);
				return InterpReturn;
		}
		programNode = programNode->next; // Loop to next statement.
	}
	return InterpOkay;
}

//  interpChildren -- Interpret child loop. Loops through the children of a family.
//    usage: children(FAM, INDI_V, INT_V) {...}
//    fields: pFamilyExpr, pChildIden, pCountIden, pLoopState
//--------------------------------------------------------------------------------------------------
InterpType interpChildren (PNode *pnode, Context *context, PValue* pval)
//  node -- The children loop program node to interpret.
//  stab -- The current symbol table.
//  pval -- Possible return value.
{
	bool eflg = false;
	GNode *fam =  evaluateFamily(pnode->familyExpr, context, &eflg);
	if (eflg || !fam || nestr(fam->tag, "FAM")) {
		scriptError(pnode, "the first argument to children must be a family");
		return InterpError;
	}
	FORCHILDREN(fam, chil, key, nchil, context->database) {
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

//  interpSpouses -- Interpret spouse loop. Loops through the spouses of a person.
//    usage: spouses(INDI, INDI_V, FAM_V, INT_V) {...}
//    fields: pPersonExpr, pSpouseIden, pFamilyIden, pCountIden, pLoopState
//--------------------------------------------------------------------------------------------------
InterpType interpSpouses(PNode *pnode, Context *context, PValue *pval)
//  node -- The spouses program node.
//  stab -- Local symbol table.
//  pval -- Possible return value.
{
	bool eflg = false;
	GNode *indi = evaluatePerson(pnode->personExpr, context, &eflg);
	if (eflg || !indi || nestr(indi->tag, "INDI")) {
		scriptError(pnode, "the first argument to spouses must be a person");
		return InterpError;
	}
	FORSPOUSES(indi, spouse, fam, nspouses, context->database) {
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

//  interpFamilies -- Interpret family loop (families a person is in as a spouse).
//    usage: families(INDI, FAM_V, INDI_V, INT_V) {...}
//    fields: pPersonExpr, pFamilyIden, pSpouseIden, pCountIden, pLoopState
//--------------------------------------------------------------------------------------------------
InterpType interpFamilies(PNode *node, Context *context, PValue *pval)
{
	bool eflg = false;
	GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
	if (eflg || !indi || nestr(indi->tag, "INDI")) {
		scriptError(node, "the first argument to families must be a person");
		return InterpError;
	}
	GNode *spouse = null;
	int count = 0;
	Database *database = context->database;
	FORFAMSS(indi, fam, key, database) {
		assignValueToSymbol(context->symbolTable, node->familyIden, PVALUE(PVFamily, uGNode, fam));
		SexType sex = SEXV(indi);
		if (sex == sexMale) spouse = familyToWife(fam, database);
		else if (sex == sexFemale) spouse = familyToHusband(fam, database);
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
InterpType interpFathers(PNode *node, Context *context, PValue *pval) {
	bool eflg = false;
	GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
	if (eflg || !indi || nestr(indi->tag, "INDI")) {
		scriptError(node, "the first argument to fathers must be a person");
		return InterpError;
	}
	int nfams = 0;
	FORFAMCS(indi, fam, key, context->database)
		GNode *husb = familyToHusband(fam, context->database);
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
InterpType interpMothers (PNode *node, Context *context, PValue *pval) {
	bool eflg = false;
	GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
	if (eflg || !indi || nestr(indi->tag, "INDI")) {
		scriptError(node, "the first argument to mothers must be a person");
		return InterpError;;
	}
	int nfams = 0;
	FORFAMCS(indi, fam, key, context->database) {
		GNode *wife = familyToWife(fam, context->database);
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
InterpType interpParents(PNode *node, Context *context, PValue *pval)
{
	bool eflg = false;
	InterpType irc;
	GNode *indi = evaluatePerson(node->personExpr, context, &eflg);
	if (eflg || !indi || nestr(indi->tag, "INDI")) {
		scriptError(node, "the first argument to parents must be a person");
		return InterpError;
	}
	int nfams = 0;
	FORFAMCS(indi, fam, key, context->database) {
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
//
//// interp_fornotes -- Interpret NOTE loop
////--------------------------------------------------------------------------------------------------
InterpType interp_fornotes(PNode *node, Context *context, PValue *pval)
{
	ASSERT(node && context);
	bool eflg = false;
	InterpType irc;
	GNode *root = evaluateGNode(node, context, &eflg);
	if (eflg) {
		scriptError(node, "1st arg to fornotes must be a record line");
		return InterpError;
	}
	if (!root) return InterpOkay;
	FORTAGVALUES(root, "NOTE", sub, vstring) {
		assignValueToSymbol(context->symbolTable, node->gnodeIden, PVALUE(PVString, uString, vstring));
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

//  interp_fornodes -- Interpret fornodes loop. Loops through the children of a Gedcom node.
//    usage: fornodes(NODE, NODE_V) {...}
//    fields: pGNodeExpr, pNodeIden, pLoopState
//--------------------------------------------------------------------------------------------------
InterpType interp_fornodes(PNode *node, Context *context, PValue *pval)
{
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

// interpForindi interprets the forindi loop statement.
// Usage: forindi(INDI_V, INT_V) {...}; Fields: personIden, countIden, loopState.
InterpType interpForindi (PNode *pnode, Context *context, PValue *pvalue) {
	RootList *rootList = context->database->personRoots;
	sortList(rootList);  // Sort the list by key??
	int numPersons = lengthList(rootList);

	for (int i = 0; i < numPersons; i++) {
		// FOR DEBUGGING.
		GNode *testGNode = (GNode*) getListElement(rootList, i);  // testWord is a root GNode.
		String keyFromRoot = rootList->getKey(getListElement(rootList, i));
		// FOR DEBUGGING.
		printf("key of the %dth person is %s\n", i, keyFromRoot);
		GNode *person = keyToPerson(keyFromRoot, context->database);
		if (person) {
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
		} else {
			printf("HIT THE ELSE IN INTERPFORINDI--PROBABLY NOT GOOD\n");
		}
	}

	//  Remove the loop variables from the symbol table before returning.
	//  MNOTE: The elements get removed from the table only in one case.
e:  removeFromHashTable(context->symbolTable, pnode->personIden);
	removeFromHashTable(context->symbolTable, pnode->countIden);
	return InterpOkay;
}
/////*========================================+
//// * interp_forsour -- Interpret forsour loop
//// *  usage: forsour(SOUR_V,INT_V) {...}
//// *=======================================*/
InterpType interp_forsour (PNode *node, Context *context, PValue *pval)
{
////    NODE sour;
////    static char key[MAXKEYWIDTH];
////    STRING record;
////    INTERPTYPE irc;
////    INT len, count = 0;
////    INT scount = 0;
////    insert_pvtable(stab, inum(node), PINT, 0);
////    while (TRUE) {
////        printkey(key, 'S', ++count);
////        if (!(record = retrieve_record(key, &len))) {
////            if(scount < num_sours()) continue;
////            break;
////        }
////        if (!(sour = stringToNodeTree(record))) continue;
////        scount++;
////        insert_pvtable(stab, ielement(node), PSOUR,
////                       sour_to_cacheel(sour));
////        insert_pvtable(stab, inum(node), PINT, (VPTR)count);
////        irc = interpret((PNODE) ibody(node), stab, pval);
////        free_nodes(sour);
////        stdfree(record);
////        switch (irc) {
////            case INTCONTINUE:
////            case INTOKAY:
////                continue;
////            case INTBREAK:
////                return INTOKAY;
////            default:
////                return irc;
////        }
////    }
////    return INTOKAY;
	return InterpOkay;
}
//

//  interp_foreven -- Interpret the foreven loop.
//    usage: foreven(EVEN_V,INT_V) {...}
//--------------------------------------------------------------------------------------------------
InterpType interp_foreven (PNode *node, Context *context, PValue *pval)
{
	int numEvents = numberEvents(context->database);
	int numMisses = 0;
	char scratch[10];

	for (int i = 1; i <= numEvents; i++) {
		sprintf(scratch, "E%d", i);
		GNode *event = keyToEvent(scratch, context->database);
		if (event) {
			assignValueToSymbol(context->symbolTable, node->eventIden, PVALUE(PVEvent, uGNode, event));
			assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, i));
			InterpType irc = interpret(node->loopState, context, pval);
			switch (irc) {
				case InterpContinue:
				case InterpOkay: continue;
				case InterpBreak:
				case InterpReturn: goto e;
				case InterpError: return InterpError;
			}
		} else {
			numMisses++;
		}
	}

	//  Remove the loop variales from the symbol table before returning.
	//  MNOTE: The elements get removed from the table only in one case.
e:  removeFromHashTable(context->symbolTable, node->personIden);
	removeFromHashTable(context->symbolTable, node->countIden);
	return InterpOkay;
}
/////*========================================+
//// * interp_forothr -- Interpret forothr loop
//// *  usage: forothr(OTHR_V,INT_V) {...}
//// *=======================================*/
InterpType interp_forothr(PNode *node, Context *context, PValue *pval)
{
	int numOthers = numberOthers(context->database);
	int numMisses = 0;
	char scratch[10];

	for (int i = 1; i <= numOthers; i++) {
		sprintf(scratch, "X%d", i);
		GNode *event = keyToEvent(scratch, context->database);
		if (event) {
			assignValueToSymbol(context->symbolTable, node->otherIden, PVALUE(PVEvent, uGNode, event));
			assignValueToSymbol(context->symbolTable, node->countIden, PVALUE(PVInt, uInt, i));
			InterpType irc = interpret(node->loopState, context, pval);
			switch (irc) {
				case InterpContinue:
				case InterpOkay: continue;
				case InterpBreak:
				case InterpReturn: goto e;
				case InterpError: return InterpError;
			}
		} else {
			numMisses++;
		}
	}

	//  Remove the loop variales from the symbol table before returning.
	//  MNOTE: The elements get removed from the table only in one case.
e:  removeFromHashTable(context->symbolTable, node->personIden);
	removeFromHashTable(context->symbolTable, node->countIden);
	return InterpOkay;
	return InterpOkay;
}
/////*======================================+
//// * interpForFam -- Interpret forfam loop
//// *  usage: forfam(FAM_V,INT_V) {...}
//// *=====================================*/
InterpType interpForFam (PNode *node, Context *context, PValue *pval)
{
////    NODE fam;
////    static char key[MAXKEYWIDTH];
////    STRING record;
////    INTERPTYPE irc;
////    INT len, count = 0;
////    INT fcount = 0;
////    insert_pvtable(stab, inum(node), PINT, (VPTR)count);
////    while (TRUE) {
////        printkey(key, 'F', ++count);
////        if (!(record = retrieve_record(key, &len))) {
////            if(fcount < num_fams()) continue;
////            break;
////        }
////        if (!(fam = stringToNodeTree(record))) continue;
////        fcount++;
////        insert_pvtable(stab, ielement(node), PFAM,
////                       (VPTR) fam_to_cacheel(fam));
////        insert_pvtable(stab, inum(node), PINT, (VPTR)count);
////        irc = interpret((PNODE) ibody(node), stab, pval);
////        free_nodes(fam);
////        stdfree(record);
////        switch (irc) {
////            case INTCONTINUE:
////            case INTOKAY:
////                continue;
////            case INTBREAK:
////                return INTOKAY;
////            default:
////                return irc;
////        }
////    }
////    return INTOKAY;
	return InterpOkay;
}

// interpretSequenceLoop interprets a script sequence loop.
// usage: forindiset(SET, INDI_V, ANY_V, INT_V) { }
// fields: sequenceExpr, pElementIden, pCountIden, pLoopState
InterpType interpretSequenceLoop(PNode *pnode, Context *context, PValue *pval)
{
	bool eflg = false;
	InterpType irc;
	PValue val = evaluate(pnode->sequenceExpr, context, &eflg); // Must be a Sequence.
	if (eflg || val.type != PVSequence) {
		scriptError(pnode, "the first argument to forindiset must be a set");
		return InterpError;
	}
	Sequence *seq = val.value.uSequence;
	FORSEQUENCE(seq, el, ncount) { // Iterate Sequence elements.
		GNode *indi = keyToPerson(el->key, context->database); // Update person in symbol table.
		assignValueToSymbol(context->symbolTable, pnode->elementIden, PVALUE(PVPerson, uGNode, indi));
		PValue pvalue = (PValue) {PVInt, el->value}; // Update person's value in symbol table.
		assignValueToSymbol(context->symbolTable, pnode->valueIden, pvalue);
		assignValueToSymbol(context->symbolTable, pnode->countIden, PVALUE(PVInt, uInt, ncount));

		switch (irc = interpret(pnode->loopState, context, pval)) { // Interpret body of loop.
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
InterpType interpIfStatement(PNode *pnode, Context *context, PValue *rvalue) {
	ASSERT(pnode && pnode->type == PNIf && context);
	bool eflg = false;
	bool cond = evaluateConditional(pnode->condExpr, context, &eflg); // Evaluate condition.
	if (eflg) return InterpError;
	if (cond) return interpret(pnode->thenState, context, rvalue); // Interpret then.
	if (pnode->elseState) return interpret(pnode->elseState, context, rvalue); // Interpret else.
	return InterpOkay; // No else statement.
}

// interpWhileStatement interprets a while statement.
// usage: while ([VAR,] COND) { BODY }; fields: condExpr. loopstaate
InterpType interpWhileStatement (PNode *node, Context *context, PValue *pval) {
	ASSERT(node && node->type == PNWhile && context);
	bool eflg = false;
	while (true) {
		bool cond = evaluateConditional(node->condExpr, context, &eflg); // Evaluate condition.
		if (eflg) return InterpError;
		if (!cond) return InterpOkay; // If condition is false loop is over.
		InterpType irc;
		switch (irc = interpret(node->loopState, context, pval)) { // Interpret body of loop.
			case InterpContinue: // Do another loop.
			case InterpOkay:
				continue;
			case InterpBreak: // Break out of loop.
				return InterpOkay;
			default: // For InterpReturn return.
				return irc;
		}
	}
}

// interpProcCall interpret a procedure call.
// Usage: CALL PROC(ARG,...)/ Fields: procName, arguments, parameters, procbody.
InterpType interpProcCall(PNode *programNode, Context *context, PValue *pval) {
	ASSERT(programNode && programNode->type == PNProcCall && context);
	//if (programDebugging) {
	if (true) {
		printf("interpProcCall: %d: %s: %2.3f\n", programNode->lineNumber, programNode->procName,
			   getMilliseconds());
	}
	PNode *procedure = searchFunctionTable(procedureTable, programNode->procName); // Get procedure.
	if (!procedure) {
		printf("``%s'': undefined procedure\n", programNode->procName);
		return InterpError;
	}
	SymbolTable *newSymbolTable = createSymbolTable(); // Create context to run procedure in.
	Context *newContext = createContext(newSymbolTable, context->database);
	if (debugging)
		printf("interpProcCall: creating symbol table %p for %s\n", newSymbolTable,
			   programNode->procName);
	PNode *argument = programNode->arguments;  // First argument to the procedure.
	PNode *parameter = procedure->parameters;  // First parameter of the procedure.
	while (argument && parameter) { // Bind arguments to parameters.
		bool eflg = false;
		PValue value = evaluate(argument, context, &eflg); // Evaluate an argument.
		if (eflg) return InterpError;
		assignValueToSymbol(newSymbolTable, parameter->identifier, value); // Assign arg to param.
		argument = argument->next;
		parameter = parameter->next;
	}
	if (argument || parameter) { // Check for argument parameter mismatch.
		printf("``%s'': mismatched args and params\n", programNode->procName);
		deleteHashTable(newSymbolTable);
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

// interpTraverse interprets the traverse statement that traverses a Gedcom node tree. It adds two
// entries to the local symbol table for the loop variables.
// Usage: traverse(GNode expr, GNode ident, int ident) {...}.
// Fields: gnodeExpr, levelIden, gNodeIden.
#define MAXTRAVERSEDEPTH 100
InterpType interpTraverse(PNode *traverseNode, Context *context, PValue *returnValue) {
	ASSERT(traverseNode && context);
	bool errorFlag = false;
	GNode *root = evaluateGNode(traverseNode->gnodeExpr, context, &errorFlag); // Root of traverse.
	if (errorFlag || !root) {
		scriptError(traverseNode, "the first argument to traverse must be a Gedcom line");
		return InterpError;
	}
	assignValueToSymbol(context->symbolTable, traverseNode->levelIden, PVALUE(PVInt, uInt, 0));
	assignValueToSymbol(context->symbolTable, traverseNode->gnodeIden, PVALUE(PVGNode, uGNode, root));

	// Normally getValueOfIden gets the value of an iden from a SymbolTable. In this case we want
	// direct access to the PValues, because each iteration updates the values of two idents. The
	// 'proper' way to do this is with assignments, but keeping pointers to the PValues allow them
	// to be changed directly. So searchHashTable is used instead of getValueOfIden.
	PValue *level = ((Symbol*) searchHashTable(context->symbolTable, traverseNode->levelIden))->value;
	PValue *node = ((Symbol*) searchHashTable(context->symbolTable, traverseNode->gnodeIden))->value;
	ASSERT(node && level);

	GNode *snode, *nodeStack[MAXTRAVERSEDEPTH]; // Stack of GNodes.
	InterpType irc;
	InterpType returnIrc = InterpOkay;

	int lev = 0;
	nodeStack[lev] = snode = root; // Init the stack and start traverse.
	while (true) {
		node->value.uGNode = snode; // Update symbol table.
		level->value.uInt = lev;
		switch (irc = interpret(traverseNode->loopState, context, returnValue)) { // Interpret loop.
			case InterpContinue:
			case InterpOkay: break;
			case InterpBreak:
				returnIrc = InterpOkay;
				goto a;
			default:
				returnIrc = irc;
				goto a;
		}

		// Modify lev and the stack for the next traverse.
		// If the current node has a child go down.
		if (snode->child) {
			snode = nodeStack[++lev] = snode->child;
			continue;
		}
		// If current node has a sibling, go there.
		if (snode->sibling) {
			snode = nodeStack[lev] = snode->sibling;
			continue;
		}
		// If current node has no child or sibling, pop until either reach top or find a sibling.
		while (--lev >= 0 && !(nodeStack[lev])->sibling)
			;
		if (lev < 0) break;
		snode = nodeStack[lev] = (nodeStack[lev])->sibling;
	}
a:  removeFromHashTable(context->symbolTable, traverseNode->levelIden);
	removeFromHashTable(context->symbolTable, traverseNode->gnodeIden);
	return returnIrc;
}

//  scriptError reports a run time script error.
void scriptError(PNode *gnode, String fmt, ...) {
	va_list args;
	printf("\nError in \"%s\" at line %d: ", gnode->fileName, gnode->lineNumber);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf(".\n");
}
