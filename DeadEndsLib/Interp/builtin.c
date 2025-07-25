//
//  DeadEndsLib
//  builtin.c contains many built-in functions of the DeadEnds script language.
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed on 25 July 2025.
//

#include "context.h"
#include "database.h"
#include "date.h"
#include "evaluate.h"
#include "frame.h"
#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "interp.h"
#include "lineage.h"
#include "list.h"
#include "name.h"
#include "path.h"
#include "place.h"
#include "pnode.h"
#include "pvalue.h"
#include "recordindex.h"
#include "sequence.h"
#include "standard.h"
#include "symboltable.h"

// Global constants for useful PValues.
const PValue nullPValue = {PVNull, PV()};
const PValue truePValue = PVALUE(PVBool, uBool, true);
const PValue falsePValue = PVALUE(PVBool, uBool, false);
extern bool symbolTableDebugging;

// isZeroVUnion returns if VUnion value is a numeric zero.
bool isZeroVUnion(PVType type, VUnion vunion) {
	switch (type) {
		case PVInt: return vunion.uInt == 0;
		case PVFloat: return vunion.uFloat == 0;
		default: return false;
	}
}

// * __getint -- Have user provide integer
// *   usage: getint(IDEN [,STRING]) --> VOID
// *   usage: getintmsg(IDEN, STRING) --> VOID
// *========================================*/
//WORD __getint (PNode *expr, Table stab, bool* eflg)
//{
//    INTERP arg = (INTERP) ielist(node);
//    INT val;
//    STRING ttl = (STRING) "Enter integer for report program";
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    val = ask_for_int(ttl);
//    assignIdent(stab, iident(arg), (WORD) val);
//    return NULL;
//}

// * __getstr -- Have user provide string
// *   usage: getstr(IDEN [,STRING]) --> VOID
// *   usage: getstrmsg(IDEN, STRING]) --> VOID
// *=========================================*/
//WORD __getstr (PNode *expr, Table stab, bool* eflg)
//{
//    INTERP arg = (INTERP) ielist(node);
//    STRING val;
//    static STRING ttl = (STRING) "Enter string for report program";
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    val = (STRING) ask_for_string(ttl, "enter string: ");
//    assignIdent(stab, iident(arg), (WORD) val);
//    return NULL;
//}

// * __getindi -- Have user identify person
// *   usage: getindi(IDEN [,STRING]) --> VOID
// *   usage: getindimsg(IDEN, STRING) --> VOID
// *=========================================*/
//WORD __getindi (PNode *expr, Table stab, bool* eflg)
//{
//    INTERP arg = (INTERP) ielist(node);
//    CACHEEL cel;
//    STRING ttl = (STRING) "Identify person for report program:";
//    STRING key;
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    key = ask_for_indi_key(ttl, FALSE, TRUE);
//    if (!key) return NULL;
//    cel = key_to_indi_cacheel(key);
//    assignIdent(stab, iident(arg), (WORD) cel);
//    return NULL;
//}
//#if 0
///*===================================================
// * __getindidate -- Have user identify person
// *   usage: getindidate(IDEN, INT [,STRING]) --> VOID
// *=================================================*/
//WORD __getindidate (PNode *expr, Table stab, bool* eflg)
//INTERP node; TABLE stab; bool *eflg;
//{
//    INTERP arg = (INTERP) ielist(node);
//    INT year;
//    CACHEEL cel;
//    STRING ttl = "Identify person for report program:";
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    year = (INT) evaluate(arg = inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    cel = key_to_indi_cacheel(ask_for_indi_key(ttl, FALSE, TRUE));
//    assignIdent(stab, iident(arg), (WORD) cel);
//    return NULL;
//}
//#endif
// * __getfam -- Have user identify family
// *   usage: getfam(IDEN [,STRING]) --> VOID
// *=======================================*/
//static char *choosefamily = "Choose family by selecting spouse:";
//WORD __getfam (PNode *expr, Table stab, bool* eflg)
//{
//    INTERP arg = (INTERP) ielist(node);
//    CACHEEL cel = NULL;
//    NODE fam;
//    STRING ttl = (STRING) "Identify family for report program:";
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    fam = ask_for_fam("Enter a spouse from family.",
//        "Enter a sibling from family.");
//    if (fam) cel = fam_to_cacheel(fam);
//    assignIdent(stab, iident(arg), (WORD) cel);
//    return NULL;
//}

// * __getindiset -- Have user identify set of persons
// *   usage: getindiset(IDEN [,STRING]) --> VOID
// *===============================================*/
//WORD __getindiset (PNode *expr, Table stab, bool* eflg)
//{
//    INTERP arg = (INTERP) ielist(node);
//    INDISEQ seq;
//    STRING ttl = (STRING) "Identify person list for report program:";
//    *eflg = TRUE;
//    if (!iistype(arg, IIDENT)) return NULL;
//    *eflg = FALSE;
//    if (inext(arg)) ttl = (STRING) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    seq = (INDISEQ) ask_for_indi_list(ttl, TRUE);
//    assignIdent(stab, iident(arg), (WORD) seq);
//    return NULL;
//}

// __strsoundex computes the DeadEnds version of the SOUNDEX function of a String.
// usage: strsoundex(STRING) -> STRING
PValue __strsoundex(PNode* expr, Context* context, bool* eflg) {
	PValue pvalue = evaluate(expr->arguments, context, eflg);
	if (*eflg || pvalue.type != PVString || !pvalue.value.uString) return nullPValue;
	return createStringPValue(soundex(pvalue.value.uString));
}

// __set performs the DeadEnds script assignment statement.
// usage: set(IDEN, ANY) -> VOID
PValue __set(PNode* pnode, Context* context, bool* errflg) {
	PNode *iden = pnode->arguments;
	PNode *expr = iden->next;
    if (iden->type != PNIdent) {
        scriptError(pnode, "First argument to set() must be an identifier.");
        *errflg = true;
        return nullPValue;
    }
	PValue value = evaluate(expr, context, errflg);
	if (*errflg) return nullPValue;
	assignValueToSymbol(context, iden->identifier, value);
    if (symbolTableDebugging) {
        printf("Symtab after set() builtin with variable %s\n", iden->identifier);
        showSymbolTable(context->frame->table);
    }
	return nullPValue;
}

// __d returns a cardinal integer as a string.
// usage: d(INT) -> STRING
PValue __d(PNode* expr, Context* context, bool* eflg) {
	char scratch[20];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (value.type == PVBool)
		return createStringPValue(value.value.uBool ? "1" : "0");
	if (*eflg || value.type != PVInt) return nullPValue;
	sprintf(scratch, "%ld", value.value.uInt);
	return createStringPValue(scratch);
}

// __f returns a string holding a floating point number.
// usage: f(FLOAT) -> STRING
PValue __f(PNode* expr, Context* context, bool* errflg) {
	char scratch[20];
	PValue value = evaluate(expr->arguments, context, errflg);
	if (*errflg || value.type != PVFloat) return nullPValue;
	sprintf(scratch, "%4f", value.value.uFloat);
	return createStringPValue(scratch);
}

// __alpha converts an integer between 1 and 26 to 'a' to 'z'. If out of range returns d(INT).
// usage: alpha(INT) -> STRING.
PValue __alpha(PNode* expr, Context* context, bool* eflg) {
	char scratch[4];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	if (lvalue < 1 || lvalue > 26) return __d(expr, context, eflg);
	sprintf(scratch, "%c", 'a' + (int) lvalue - 1);
	value.type = PVString;
	return  createStringPValue(scratch);
}

// __ord convert a small integer to an ordinal string. If out of range returns d(INT)
// usage: ord(INT) -> STRING.
static char *ordinals[] = {
	"first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth",
	"tenth", "eleventh", "twelfth"
};
PValue __ord(PNode* expr, Context* context, bool* eflg) {
	char scratch[12];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	value.type = PVString;
	if (lvalue < 1) return __d(expr, context, eflg);
	if (lvalue > 12) {
		sprintf(scratch, "%ldth", lvalue);
        return createStringPValue(scratch);
	}
    return createStringPValue(ordinals[lvalue - 1]);
}

// __card convert a small integer to a cardinal string. If out of range returns d(INT).
// usage: card(INT) -> STRING
static char *cardinals[] = {
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
	"eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
	"eighteen", "nineteen", "twenty"
};
PValue __card (PNode* expr, Context* context, bool* eflg) {
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	if (lvalue < 0 || lvalue > 20) return __d(expr, context, eflg);
	return createStringPValue(cardinals[lvalue]);
}

// __roman convert an integer to Roman numeral. If out of range returns d(INT).
//  usage: roman(INT) -> STRING.
PValue __roman(PNode* node, Context* context, bool* eflg) {
	char scratch[256] = "";
	static char* symbols[] = {"m", "cm", "d", "cd", "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i"};
	static int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};

	PValue value = evaluate(node->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	int num = (int) value.value.uInt;
	if (num > 3999) return (PValue) __d(node, context, eflg);
	for (int i = 0; num > 0; i++) {
		while (num >= values[i]) {
			strcat(scratch, symbols[i]);
			num -= values[i];
		}
	}
	return createStringPValue(scratch);
}

// __strcmp compares two strings and return their relationship using C rules.
// usage: strcmp(STRING, STRING) -> INT
// usage: nestr(STRING, STRING) -> BOOL
PValue __strcmp (PNode* pnode, Context* context, bool* errflg) {
	PNode *arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (pvalue.type != PVString) {
		scriptError(pnode, "first argument to nestr/strcmp() must be a string");
		*errflg = true;
		return nullPValue;
	}
	String str1 = pvalue.value.uString;
	arg = arg->next;
	pvalue = evaluate(arg, context, errflg);
	if (pvalue.type != PVString) {
		scriptError(pnode, "second argument to nestr/strcmp() must be a string");
		*errflg = true;
		return nullPValue;
	}
	String str2 = pvalue.value.uString;
	return PVALUE(PVInt, uInt, (long) strcmp(str1, str2));
}

//  __eqstr compares two strings for equality.
//    usage: eqstr(STRING, STRING) -> BOOL
PValue __eqstr (PNode *pnode, Context *context, bool *errflg) {
	PNode *arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (pvalue.type != PVString) {
		scriptError(pnode, "first argument to eqstr() must be a string");
		*errflg = true;
		return nullPValue;
	}
	String left = pvalue.value.uString;
	arg = arg->next;
	pvalue = evaluate(arg, context, errflg);
	if (pvalue.type != PVString) {
		scriptError(pnode, "second argument to eqstr() must be a string");
		*errflg = true;
		return nullPValue;
	}
	return PVALUE(PVBool, uBool, eqstr(left, pvalue.value.uString));
}

//  __strtoint -- Convert string to integer
//  usage: strtoint(STRING) -> INT and atoi(STRING) -> INT
PValue __strtoint (PNode *node, Context *context, bool *eflg) {
	PValue value = evaluate(node->arguments, context, eflg);
	if (*eflg || value.type != PVString || !value.value.uString) return nullPValue;
	return PVALUE(PVInt, uInt, atoi(value.value.uString));
}

//  __save copies a string on the heap.
PValue __save (PNode *pnode, Context *context, bool *errflg) {
    String value = evaluateString(pnode->arguments, context, errflg);
	if (*errflg || !value || *value == 0) {
		scriptError(pnode, "argument to save() must be a string.");
		*errflg = true;
		return nullPValue;
	}
	return createStringPValue(value);
}

//  __strlen returns the length of string
//  usage: strlen(STRING) -> INT
PValue __strlen (PNode *node, Context *context, bool* eflg) {
    PValue value = evaluate(node->arguments, context, eflg);
    if (*eflg) return nullPValue;
    else if (value.type == PVNull) return PVALUE(PVInt, uInt, (long) 0);
    else if (value.type != PVString) {
        *eflg = true;
        scriptError (node, "argument to strlen must be a string");
        return nullPValue;
    }
    return PVALUE(PVInt, uInt, (long) strlen(value.value.uString));
}

//  __concat catenates potentially two to thirty-two strings.
//  usage: concat(STRING [, STRING]+) -> STRING
PValue __concat(PNode *pnode, Context *context, bool *errflg) {
	PNode *arg = pnode->arguments;
	if (arg == null) return nullPValue;
	int len = 0, nstrs = 0;
	String hold[CC];
	while (arg != null) {
		PValue svalue = evaluate(arg, context, errflg);
		if (*errflg || svalue.type != PVString) return nullPValue;
		len += strlen(svalue.value.uString);
		hold[nstrs++] = svalue.value.uString;
		arg = arg->next;
	}
	String nstring = stdalloc(len + 1);
	String p = nstring;
	for (int i = 0; i < nstrs; i++) {
		strcpy(p, hold[i]);
		p += strlen(p);
		stdfree(hold[i]);
	}
	*p = 0;
    PValue rvalue = createStringPValue(nstring);
    stdfree(nstring);
    return rvalue;
}

//  __lower converts a string to lower case.
//  usage: lower(STRING) -> STRING
PValue __lower(PNode *node, Context *context, bool* eflg) {
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return createStringPValue(lower(val.value.uString));
}

//  __upper converts a string to upper case.
//  usage: upper(STRING) -> STRING
PValue __upper(PNode *node, Context *context, bool* eflg) {
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return createStringPValue(upper(val.value.uString));
}

//  __capitalize capitalizes a string.
// usage: capitalize(STRING) -> STRING
PValue __capitalize(PNode *node, Context *context, bool* eflg) {
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return createStringPValue(capitalize(val.value.uString));
}

// __print prints a list of expresseion values to the stdout window.
// usage: print([STRING]+,) -> VOID
PValue __print(PNode *pnode, Context *context, bool *errflg) {
    for (PNode* arg = pnode->arguments; arg != null; arg = arg->next) {
        PValue svalue = evaluate(arg, context, errflg);
        if (*errflg) return nullPValue;
        if (svalue.type != PVString) continue;
        printf("%s", svalue.value.uString);
    }
    return nullPValue;
}

// __root returns the root node of the record the argument node is in.
// usage: root(NODE) -> NODE
PValue __root (PNode* pnode, Context* context, bool* errflg) {
    PNode *arg = pnode->arguments;
	GNode *gnode = evaluateGNode(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "argument to root() must be a Gedcom node");
		return nullPValue;
	}
	int count = 0;
	while (gnode->parent && count++ < 100) {
		gnode = gnode->parent;
	}
	if (count >= 100) {
		*errflg = true;
		scriptError(pnode, "recursion loop in root()");
		return nullPValue;
	}
	return PVALUE(PVGNode, uGNode, gnode);
}

// __extractdate tries to extract a Date from an event or DATE GNode.
// usage: extractdate(NODE, VARB, VARB, VARB) -> VOID
PValue __extractdate(PNode *pnode, Context *context, bool* errflg) {
    int day = 0, month = 0, year = 0, daormo = 0;
    String str;
    PNode *arg = pnode->arguments; // GNode of event or DATE.
    GNode *gnode = evaluateGNode(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "first argument to extractdate() must be an event or DATE node.");
		return nullPValue;
	}
	PNode *dvar = arg->next; // Symbol to hold day integer.
	PNode *mvar = dvar->next; // Symbol to hold month integer.
	PNode *yvar = mvar->next; // Symbol to hold year integer.
	bool error = false;
	if (dvar->type != PNIdent) error = true;
	if (mvar->type != PNIdent) error = true;
	if (yvar->type != PNIdent) error = true;
	if (error) {
		*errflg = true;
		scriptError(pnode, "The day, month and year arguments must be identifiers.");
		return nullPValue;
	}
	// gnode should be either a DATE node or an event node.
    if (nestr("DATE", gnode->tag))
        str = eventToDate(gnode, false);
    else
        str = gnode->value;
	if (!str || *str == 0) return nullPValue;  // Not considered an error.
	String stryear;
    extractDate(str, &daormo, &day, &month, &year, &stryear);
    assignValueToSymbol(context, dvar->identifier, PVALUE(PVInt, uInt, day));
	assignValueToSymbol(context, mvar->identifier, PVALUE(PVInt, uInt, month));
	assignValueToSymbol(context, yvar->identifier, PVALUE(PVInt, uInt, year));
    *errflg = false;
    return nullPValue;
}

//  __extractnames tries to extract name parts from person or NAME node.
//  usage: extractnames(NODE, LIST, VARB, VARB) -> VOID
//static void sdelete(void* element) { stdfree(element); } // The elements are Strings that are freed.
PValue __extractnames (PNode *pnode, Context *context, bool *errflg) {
	PNode *nexp = pnode->arguments;
	PNode *lexp = nexp->next;
	PNode *lvar = lexp->next;
	PNode *svar = lvar->next;
	GNode *node = evaluateGNode(nexp, context, errflg); // First arg is a Gedcom node.
    if (*errflg) return nullPValue; // Error will have been reported.
	if (nestr(node->tag, "NAME")) {
        node = NAME(node);
        if (!node) {
            *errflg = true;
            scriptError(pnode, "the first arg to extractnames must be a NAME node or a parent of one");
            return nullPValue;
        }
	}
	// Get the list to put the name parts in.
	PValue pvalue = evaluate(lexp, context, errflg);
	if (*errflg || pvalue.type != PVList) {
		*errflg = true;
		scriptError(pnode, "the second arg to extractnames must be a list");
		return nullPValue;
	}
    // list will be filled with the name parts as PVStrings.
	List *list = pvalue.value.uList;
    if (list) emptyList(list); // Frees any contents but keeps list.
	bool error = false;
	if (!iistype(lvar, PNIdent)) error = true;
	if (!iistype(svar, PNIdent)) error = true;
	if (error) {
		*errflg = true;
		scriptError(pnode, "the third and fourth args to extract names must be identifiers.");
		return nullPValue;
	}
	String str = node->value;
	if (!str || *str == 0) { // Return an empty list.
		assignValueToSymbol(context, lvar->identifier, PVALUE(PVInt, uInt, 0));
		assignValueToSymbol(context, svar->identifier, PVALUE(PVInt, uInt, 0));
		return nullPValue;
	}
	int len, sind;
	*errflg = false;
    // Use a temporaray list to get the name parts.
    List* parts = createList(null, null, null, false); // No delete function.
    nameToList(str, parts, &len, &sind);
    // Convert the Strings in the temporary parts list to PValues and append them to list.
    for (int i = 0; i < len; i++) {
        String part = (String) getListElement(parts, i);
        PValue pvalue = PVALUE(PVString, uString, part); // Transfer String ownership.
        PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue)); // Create PValue* in the heap.
        *ppvalue = pvalue;
        appendToList(list, ppvalue);  // Add the PValue* to the list.
    }
    deleteList(parts); // Free temporary list.
	assignValueToSymbol(context, lvar->identifier, PVALUE(PVInt, uInt, len));
	assignValueToSymbol(context, svar->identifier, PVALUE(PVInt, uInt, sind));
	return nullPValue;
}

// __extractplaces tries to extract the place parts from an event or PLAC GNode.
// usage: extractplaces(NODE, LIST, VARB) -> VOID
PValue __extractplaces(PNode *pnode, Context *context, bool *errflg) {
    PNode *nexp = pnode->arguments;
    PNode *lexp = nexp->next;
    PNode *cvar = lexp->next;
    GNode *node = evaluateGNode(nexp, context, errflg); // First arg is a Gedcom node.
    if (*errflg) return nullPValue; // Error will have been reported.

    // There are a number of edge cases to handle. There may or may not be a string to process.
    String str = null;
    if (node) {
        if (eqstr(node->tag, "PLAC")) {
            str = node->value;
        } else {
            GNode* plac = PLAC(node);
            if (plac) str = plac->value;
        }
    }
    // Get the list to put the place parts in
    PValue pvalue = evaluate(lexp, context, errflg);
    if (*errflg || pvalue.type != PVList) {
        *errflg = true;
        scriptError(pnode, "the second arg to extractplaces must be a list");
        return nullPValue;
    }
    // list will be filled with the place parts as PVStrings.
    List* list = pvalue.value.uList;
    if (list) emptyList(list); // Frees contents but keeps the list.
    bool error = false;
    if (!iistype(cvar, PNIdent)) error = true;
    if (error) {
        *errflg = true;
        scriptError(pnode, "the third arg to extractplaces must be an identifier");
        return nullPValue;
    }
    if (!str || *str == 0) { // Return an empty list.
        assignValueToSymbol(context, cvar->identifier, PVALUE(PVInt, uInt, 0));
        return nullPValue;
    }
    // Use placeToList to separate a PLAC value into list of string parts.
    List* parts = createList(null, null, null, false); // No delete function.
    placeToList(str, parts);
    // Transform the list of Strings into a list of PValues.
    FORLIST(parts, element)
        String string = (String) element;
        PValue* svalue = stdalloc(sizeof(PValue));
        svalue->type = PVString;
        svalue->value.uString = string; // Transfers memory ownership.
        appendToList(list, svalue);
    ENDLIST
    deleteList(parts); // Does not free the strings.

    // Assign the count to the symbol table
    assignValueToSymbol(context, cvar->identifier, PVALUE(PVInt, uInt, lengthList(list)));
    return nullPValue;
}

// __copyfile copies the contents of a file to the output stream.
// usage: copyfile(STRING) -> VOID
PValue __copyfile (PNode *pnode, Context *context, bool *errflg) {
	String fileName = evaluateString(pnode->arguments, context, errflg);
	if (*errflg || fileName == null || strlen(fileName) == 0) {
		*errflg = true;
		scriptError(pnode, "The argument to copyfile must be a string.");
		return nullPValue;
	}
	FILE *cfp = fopenPath(fileName, "r", null);
	if (cfp == null) {
		scriptError(pnode, "Could not open file for copying.");
		*errflg = true;
		return nullPValue;
	}
	char buffer[1024];
	while (fgets(buffer, 1024, cfp)) {
		printf("%s", buffer);  // TODO: CHANGE TO A MORE POUTPUT APPROACH.
	}
	fclose(cfp);
	return nullPValue;
}

// __nl is the newline function.
// usage: nl() -> STRING
PValue __nl(PNode* pnode, Context* context, bool* errflg) { return createStringPValue("\n"); }

// __space is the space function.
// usage: sp() -> STRING
PValue __space(PNode* pnode, Context* context, bool* errflg) { return createStringPValue(" "); }

// __qt is the double quote function
// usage: qt() -> STRING
PValue __qt(PNode *pnode, Context *context, bool* errflg) { return createStringPValue("\""); }

// __children returns the Sequence of children in a family
// usage: children(FAM) -> SET
PValue __children(PNode *pnode, Context *context, bool* errflg) {
	GNode *family = evaluateFamily(pnode->arguments, context, errflg);
	if (*errflg || !family) return nullPValue;
	Sequence *children = familyToChildren(family, context->database->recordIndex);
	if (!children) return nullPValue;
	return PVALUE(PVSequence, uSequence, children);
}

// __version returns the version of the DeadEnds program.
// usage: version() -> STRING
PValue __version(PNode*vpnode, Context* context, bool* errflg) {
	extern String version;
	return createStringPValue(version);
}

// __noop is used for builtins that have been removed (e.g., lock, unlock).
PValue __noop(PNode *pnode, Context *context, bool* errflg) { return nullPValue; }

// __createnode creates a Gedcom node.
// usage: createnode(STRING key[, STRING value]) -> NODE, value can be omitted.
PValue __createnode(PNode* pnode, Context* context, bool* errflg) {
	PNode *arg1 = pnode->arguments, *arg2 = arg1->next;
	// Get the tag.
	PValue tagValue = evaluate(arg1, context, errflg);
	if (tagValue.type != PVString) {
		scriptError(pnode, "first argument to createnode must be a key string");
		*errflg = true;
		return nullPValue;
	}
	String tag = tagValue.value.uString;
	String value = null;
	// Get the value if there.
	if (arg2) {
		PValue valValue = evaluate(arg2, context, errflg);
		if (valValue.type != PVNull && valValue.type != PVString) {
			scriptError(pnode, "the second argument to create node must be an optional string");
			*errflg = true;
			return nullPValue;
		}
		value = valValue.value.uString;
	}
	return PVALUE(PVGNode, uGNode, createGNode(null, tag, value, null));
}

// __addnode adds a node to a Gedcom tree.
// usage: addnode(NODE this, NODE parent[, NODE prevsib]) -> VOID, where prevsib may omitted.
PValue __addnode(PNode* pnode, Context* context, bool* errflg) {
	// Get node to add.
	PNode* arg1 = pnode->arguments, *arg2 = arg1->next, *arg3 = arg2->next;;
	PValue this = evaluate(arg1, context, errflg);
	if (*errflg || !isGNodeType(this.type)) {
        scriptError(pnode, "the first argument to addnode must be an existing node");
		*errflg = true;
		return nullPValue;
	}
	GNode *thisNode = this.value.uGNode;
	// Get parent of node.
	PValue parent = evaluate(arg2, context, errflg);
	if (*errflg || !isGNodeType(parent.type)) {
        scriptError(pnode, "the second argument to addnode must be an existing node");
		*errflg = true;
		return nullPValue;
	}
	GNode *parentNode = parent.value.uGNode;
	// Get previous sibling if there is one.
	GNode* prevNode = null;
	if (arg3) { // Previous sibling may be omitted.
		PValue prev = evaluate(arg3, context, errflg);
		if (*errflg || !isGNodeType(prev.type)) {
			*errflg = true;
			scriptError(pnode, "the third argument to addnode must be an existing node");
			return nullPValue;
		}
		prevNode = prev.value.uGNode;
	}
	thisNode->parent = parentNode;
	GNode *nextNode = null;
	if (prevNode == null) {
		nextNode = parentNode->child;
		parentNode->child = thisNode;
	} else {
		nextNode = prevNode->sibling;
		prevNode->sibling = thisNode;
	}
	thisNode->sibling = nextNode;
	return nullPValue;
}

// __deletenode removes subtree from a Gedcom tree/record.
// usage: deletenode(NODE) -> VOID
// mnote: memory leak.
PValue __deletenode(PNode *node, Context *context, bool *errflg) {
	PValue pvalue = evaluate(node->arguments, context, errflg);
	if (*errflg || !isGNodeType(pvalue.type)) {
		*errflg = true;
		scriptError(node, "the argument to deletenode must be an existing node");
		return nullPValue;
	}
	GNode *this = pvalue.value.uGNode;
	// If this node has no parent, it is a root node, and they cannot be deleted.
	if (!this->parent) {
		*errflg = true;
		scriptError(node, "the argument node is a root and cannot be deleted");
		return nullPValue;
	}
	GNode *parent = this->parent;
	GNode *prev = null;
	GNode *curs = parent->child;
	while (curs && curs != this) {
		prev = curs;
		curs = curs->sibling;
	}
	if (curs == null) return nullPValue;
	GNode *next = this->sibling;
	if (prev == null)
		parent->child = next;
	else
		prev->sibling = next;
	this->parent = null;
	this->sibling = null;
	return nullPValue;
}

// __getrecord reads a GNode record from a database.
// usage: getrecord(STRING) -> NODE and dereference(STRING) -> NODE
PValue __getrecord (PNode *pnode, Context *context, bool *errflg) {
	String key = evaluateString(pnode->arguments, context, errflg);
	if (*errflg || !key || *key == 0) {
		scriptError(pnode, "first parameter to getrecord/dereference() must be a record key");
		*errflg = true;
		return nullPValue;
	}
	GNode* root = getRecord(key, context->database->recordIndex);
	return root ? PVALUE(PVGNode, uGNode, root) : nullPValue;
}

// __freerecord is a no-op because of the in-Ram database.
// usage: freerecord(NODE) -> VOID
PValue __freerecord (PNode* pnode, Context* context, bool* errflg) {
	return nullPValue;
}

// __reference checks if a String is has the format of a record key.
// usage: reference(STRING) -> BOOLEAN
PValue __reference (PNode* pnode, Context* context, bool* errflg) {
	String key = evaluateString(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The argument to reference must be formatted as a record key.");
		return nullPValue;
	}
	bool rvalue = (*key && (strlen(key)) > 2 && (*key == '@') &&
		(key[strlen(key)-1] == '@'));
	return PVALUE(PVBool, uBool, rvalue);
}

// __extracttokens extracts tokens from a STRING value
// usage: extracttokens(STRING, LIST, VARB, STRING) -> VOID
// String to tokenize; List to hold tokens; variable to hold count; delimiter.
PValue __extracttokens (PNode *pnode, Context *context, bool *errflg) {
	// Get the String to be tokenized.
	PNode *sexp = pnode->arguments;
	String str = evaluateString(sexp, context, errflg);
	if (*errflg) {
		scriptError(pnode, "the first arg to extracttokens must be a string");
		return nullPValue;
	}
	// Get the List to hold the tokens.
	PNode *lexp = sexp->next;
	PValue pvalue = evaluate(lexp, context, errflg);
    if (*errflg) return nullPValue;
    if (pvalue.type != PVList) {
		scriptError(pnode, "the second arg to extracttokens must be a list");
		*errflg = true;
		return nullPValue;
	}
	List *list = pvalue.value.uList;
    if (list) emptyList(list);
	// Get the variable to hold the number of tokens found.
	PNode *lvar = lexp->next;
	if (lvar->type != PNIdent) {
		scriptError(pnode, "the third arg to extracttokens must be an identifier");
		*errflg = true;
		return nullPValue;
	}
	// Get the delimiter between tokens.
	String dlm = evaluateString(lvar->next, context, errflg);
    if (*errflg) return nullPValue;
	if (!dlm || *dlm == 0) {
		scriptError(pnode, "The fourth arg to extracttokens must be a string delimiter");
		*errflg = true;
		return nullPValue;
	}
    // Break the string into a temp              List of String tokens.
    List* tokens = createList(null, null, null, false);
	valueToList(str, tokens, dlm);
    // Transform the Strings into PValues and put them in list.
    FORLIST(tokens, element)
        String string = (String) element;
        PValue* svalue = stdalloc(sizeof(PValue));
        svalue->type = PVString;
        svalue->value.uString = string; // Transfer memory ownership.
        appendToList(list, svalue);
    ENDLIST
	assignValueToSymbol(context, lvar->identifier, PVALUE(PVInt, uInt, lengthList(list)));
    deleteList(tokens);
	return nullPValue;
}

//  __savenode -- Save Gedcom tree permanently.
//  usage: savenode(NODE) -> NODE
PValue __savenode (PNode *pnode, Context *context, bool *errflg) {
	GNode *node = evaluateGNode(pnode->arguments, context, errflg);
	if (*errflg || !node) {
		scriptError(pnode, "argument to savenode() must be a Gedcom node");
		*errflg = true;
		return nullPValue;
	}
	return PVALUE(PVGNode, uGNode, copyGNodes(node, true, true));
}
