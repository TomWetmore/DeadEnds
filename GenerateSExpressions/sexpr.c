//
//  sexpr.c
//  gensexprs
//  This file contains the printSExp recursive procedure that transforms PNode trees into S-Expressions that are
//  written to standard output.
//
//  This step avoids writing a DeadEnds script parser in Swift DeadEnds.
//
//  In this implementation there is no internal SExpression data type. The PNode trees are written to standard
//  output in S-Expression form.
//
//  Created by Thomas Wetmore on 4 March 2025.
//  Last changed on 9 April 2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pnode.h"

static bool addingLineNumbers = true;
char lnBuffer[16];

// printSExpr prints a PNode as an S-Expression. This feature allows DeadEnds scripts to be parsed by the "DeadEnds
// C world" and converted into S-expressions that are interpreted by the "DeadEnds Swift" world.
void printSExpr(FILE *out, PNode *node, int *line, int depth) {
    if (!node) { return; }
	int curline = node->lineNumber;
	if (curline != *line) {
		fprintf(out, "\n");
		*line = curline;
		for (int i = 0; i < depth; i++) { fprintf(out, "   "); }
	}

    if (addingLineNumbers) { sprintf(lnBuffer, "[%d]", curline); }

	switch (node->type) {
	case PNICons:
		fprintf(out, "%ld%s", node->intCons, lnBuffer);
		break;
	case PNFCons:
		fprintf(out, "%f%s", node->floatCons, lnBuffer);
		break;
	case PNSCons:
		fprintf(out, "\"%s\"%s", node->stringCons, lnBuffer);
		break;
	case PNIdent:
		fprintf(out, "%s%s", node->identifier, lnBuffer);
		break;
	case PNIf:
		fprintf(out, "(if%s (", lnBuffer);
		printSExpr(out, node->condExpr, line, depth);
		fprintf(out, ") {");
		printSExpr(out, node->thenState, line, depth + 1);
		fprintf(out, "}");
		if (node->elseState) {
			fprintf(out, "{");
			printSExpr(out, node->elseState, line, depth + 1);
			fprintf(out, "}");
		}
		fprintf(out, ")");
		break;
	case PNWhile:
        fprintf(out, "(while%s (", lnBuffer);
		printSExpr(out, node->condExpr, line, depth);
		fprintf(out, ") {");
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, "})");
		break;
	case PNBreak:
		fprintf(out, "(break%s)", lnBuffer);
		break;
	case PNContinue:
		fprintf(out, "(continue%s)", lnBuffer);
		break;
	case PNReturn:
		fprintf(out, "(return%s", lnBuffer);
		if (node->returnExpr) {
			fprintf(out, " ");
			printSExpr(out, node->returnExpr, line, depth);
		}
		fprintf(out, ")");
		break;
	case PNProcDef:
		fprintf(out, "(proc%s %s%s (", lnBuffer, node->procName, lnBuffer);
		printSExpr(out, node->parameters, line, depth + 1);
		fprintf(out, ") {\n"); // Close parameters; open body.
		printSExpr(out, node->procBody, line, depth + 1);
		fprintf(out, "} )");
		break;
	case PNProcCall:
		fprintf(out, "(call%s %s%s (", lnBuffer, node->procName, lnBuffer);
		printSExpr(out, node->arguments, line, depth + 1);
		fprintf(out, "))");
		break;
	case PNFuncDef:
		fprintf(out, "(func%s %s%s", lnBuffer, node->funcName, lnBuffer);
		printSExpr(out, node->parameters, line, depth + 1);
		printSExpr(out, node->funcBody, line, depth + 1);
		fprintf(out, ")");
		break;
	case PNFuncCall:
		fprintf(out, "(fcall%s %s%s (", lnBuffer, node->funcName, lnBuffer);
		printSExpr(out, node->arguments, line, depth + 1);
		fprintf(out, "))");
		break;
	case PNBltinCall:
		fprintf(out, "(bltin%s %s%s (", lnBuffer, node->builtinName, lnBuffer);
		printSExpr(out, node->arguments, line, depth + 1);
		fprintf(out, "))");
		break;
	case PNChildren:
		fprintf(out, "(children%s (", lnBuffer);
		printSExpr(out, node->familyExpr, line, depth);
		fprintf(out, " %s %s) {", node->childIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " })\n");
		break;
	case PNIndis:
		fprintf(out, "(forindi %s %s {", node->personIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNFams:
		fprintf(out, "(forfam %s %s {", node->familyIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNSources:
		fprintf(out, "(forsource %s %s {", node->sourceIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNEvents:
		fprintf(out, "(forevent %s %s {", node->eventIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNOthers:
		fprintf(out, "(forother %s %s {", node->otherIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNList:
		fprintf(out, "(forlist ");
		printSExpr(out, node->listExpr, line, depth);
		fprintf(out, " %s %s { ", node->elementIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNSequence:
		fprintf(out, "(sequence (");
		printSExpr(out, node->sequenceExpr, line, depth);
		fprintf(out, " %s %s %s) { ", node->elementIden, node->valueIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNFathers:
		fprintf(out, "(fathers ");
		printSExpr(out, node->personExpr, line, depth);
		fprintf(out, " %s %s %s { ", node->fatherIden, node->familyIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNMothers:
		fprintf(out, "(mothers ");
		printSExpr(out, node->personExpr, line, depth);
		fprintf(out, " %s %s %s { ", node->motherIden, node->familyIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNFamsAsChild:
		fprintf(out, "(parents ");
		printSExpr(out, node->personExpr, line, depth);
		fprintf(out, " %s %s { ", node->familyIden, node->countIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	case PNTraverse:
		fprintf(out, "(traverse ");
		printSExpr(out, node->gnodeExpr, line, depth);
		fprintf(out, " %s %s { ", node->gnodeIden, node->levelIden);
		printSExpr(out, node->loopState, line, depth + 1);
		fprintf(out, " } )\n");
		break;
	default:
		fprintf(out, "(UNKNOWN)");
		break;
	}

	// Print next statement in a sequence
	if (node->next) {
		fprintf(out, " ");
		printSExpr(out, node->next, line, depth);
	}
}

// **Update Wrapper Function**
void printPNodeTreeAsSExpr(FILE *out, PNode *root) {
    extern char lnBuffer[];
    lnBuffer[0] = 0;
	int line = root->lineNumber;
	//fprintf(out, "\n");
	printSExpr(out, root, &line, 1);
}
