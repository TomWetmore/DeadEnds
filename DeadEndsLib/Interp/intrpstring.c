//
//  DeadEnds Library
//  intrpstring.c
//
//  Created by Thomas Wetmore on 14 February 2024.
//  Last changed on 3 June 2025.
//

#include <stdio.h>
#include "evaluate.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "standard.h"
#include "utils.h"

// __substring -- Find substring of string.
//  usage: substring(STRING, INT, INT) -> STRING
PValue __substring (PNode *pnode, Context *context, bool *errflg) {
	PNode *arg = pnode->arguments;
	String full = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to substring must be a string.");
		return nullPValue;
	}
	arg = arg->next;
	int lo = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to substring must be an integer.");
		return nullPValue;
	}
	arg = arg->next;
	int hi = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The third argument to substring must be an integer.");
		return nullPValue;
	}
	return createStringPValue(substring(full, lo, hi));
}

//  __trim -- Trim string if too long
//    usage: trim(STRING, INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __trim (PNode *pnode, Context *context, bool *errflg)
{
	// Get the String to be trimmed.
	PNode *arg = pnode->arguments;
	String string = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to trim must be a string.");
		return nullPValue;
	}
	// Get the maximum size of the String.
	arg = arg->next;
	int length = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to time must be an integer.");
		return nullPValue;
	}
	return createStringPValue(trim(string, length));
}

//  __rjustify -- Right justify a string value
//    usage: rjustify(STRING, INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __rjustify (PNode *pnode, Context *context, bool *errflg)
{
	// Get the String to be right justified.
	PNode *arg = pnode->arguments;
	String string = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to rjustify must be a string.");
		return nullPValue;
	}
	// Get the size of field to right justify the String into.
	arg = arg->next;
	int length = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to rjustify must be an integer.");
		return nullPValue;
	}
	return createStringPValue(rightjustify(string, length));
}
