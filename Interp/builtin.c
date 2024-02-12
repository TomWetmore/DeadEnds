//
//  DeadEnds
//
//  builtin.c -- This file contains many of the built-in functions of the DeadEnds programming
//    language.
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed on 22 November 2023.
//

#include "standard.h"
#include "gnode.h"    // GNode.
#include "lineage.h"
#include "pvalue.h"   // PValue.
#include "pnode.h"    // PNode.
#include "name.h"     // getSurname, manipulateName.
#include "interp.h"
#include "recordindex.h" // searchRecordIndex.
#include "database.h"    // personIndex, familyIndex.
#include "hashtable.h"
//#include "gedcom.h"
#include "evaluate.h"  // evaluate.
#include "path.h"      // fopenPath.
#include "symboltable.h"
#include "date.h"
#include "place.h"

//#define Table SymbolTable

// Global constants for useful PValues.
//--------------------------------------------------------------------------------------------------
const PValue nullPValue = {PVNull, PV()};
const PValue truePValue = PVALUE(PVBool, uBool, true);
const PValue falsePValue = PVALUE(PVBool, uBool, false);
const PValue spacePValue = PVALUE(PVString, uString, " ");
const PValue quotePValue = PVALUE(PVString, uString, "\"");
const PValue newlinePValue = PVALUE(PVString, uString, "\n");

// isZeroVUnion
//--------------------------------------------------------------------------------------------------
bool isZeroVUnion(PVType type, VUnion vunion)
{
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

//  __strsoundex -- SOUNDEX function on strings
//    usage: strsoundex(STRING) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __strsoundex(PNode *expr, Context *context, bool* eflg)
{
	PValue pvalue = evaluate(expr->arguments, context, eflg);
	if (*eflg || pvalue.type != PVString || !pvalue.value.uString) return nullPValue;
	return PVALUE(PVString, uString, strsave(soundex(pvalue.value.uString)));
}

//  __set -- Assignment "statement".
//    usage: set(IDEN, ANY) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __set(PNode *pnode, Context *context, bool* eflg)
{
	PNode *iden = pnode->arguments;
	PNode *expr = iden->next;
	if (iden->type != PNIdent) { *eflg = true; return nullPValue; }
	PValue value = evaluate(expr, context, eflg);
	if (*eflg) return nullPValue;
	assignValueToSymbol(context->symbolTable, iden->identifier, value);
	return nullPValue;
}

//  __d -- Return cardinal integer as a string.
//    usage: d(INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __d(PNode *expr, Context *context, bool* eflg)
{
	char scratch[20];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (value.type == PVBool)
		return PVALUE(PVString, uString, value.value.uBool ? "1" : "0");
	if (*eflg || value.type != PVInt) return nullPValue;
	sprintf(scratch, "%ld", value.value.uInt);
	return PVALUE(PVString, uString, strsave(scratch));
}

//  __f -- Return floating point value as a string.
//---------------------------------------------------------------------------------------------------
PValue __f(PNode *expr, Context *context, bool *errflg)
{
	char scratch[20];
	PValue value = evaluate(expr->arguments, context, errflg);
	if (*errflg || value.type != PVFloat) return nullPValue;
	sprintf(scratch, "%4f", value.value.uFloat);
	return PVALUE(PVString, uString, strsave(scratch));
}

//  __alpha -- Convert small integer (between 1 and 26) to a letter.
//    usage: alpha(INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __alpha(PNode *expr, Context *context, bool* eflg)
{
	char scratch[4];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	if (lvalue < 1 || lvalue > 26) return __d(expr, context, eflg);
	sprintf(scratch, "%c", 'a' + (int) lvalue - 1);
	value.type = PVString;
	return PVALUE(PVString, uString, strsave(scratch));
}

//  __ord -- Convert a small integer to an ordinal string.
//    usage: ord(INT) -> STRING
//--------------------------------------------------------------------------------------------------
static char *ordinals[] = {
	"first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth",
	"tenth", "eleventh", "twelfth"
};
PValue __ord(PNode *expr, Context *context, bool* eflg)
{
	char scratch[12];
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	value.type = PVString;
	if (lvalue < 1) return __d(expr, context, eflg);
	if (lvalue > 12) {
		sprintf(scratch, "%ldth", lvalue);
		value.value.uString = strsave(scratch);
	} else {
		value.value.uString = strsave(ordinals[lvalue - 1]);
	}
	return value;
}

//  __card -- Convert small integer to cardinal string
//    usage: card(INT) -> STRING
//--------------------------------------------------------------------------------------------------
static char *cardinals[] = {
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
	"eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
	"eighteen", "nineteen", "twenty"
};
PValue __card (PNode *expr, Context *context, bool* eflg)
{
	PValue value = evaluate(expr->arguments, context, eflg);
	if (*eflg || value.type != PVInt) return nullPValue;
	long lvalue = value.value.uInt;
	if (lvalue < 0 || lvalue > 20) return __d(expr, context, eflg);
	return PVALUE(PVString, uString, cardinals[lvalue]);
}

//  __roman -- Convert integer to Roman numeral. If out of Roman range return as simple string.
//    usage: roman(INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __roman(PNode *node, Context *context, bool* eflg)
{
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
	return PVALUE(PVString, uString, strsave(scratch));
}

//  __strcmp -- Compare two strings and return their relationship.
//     usage: strcmp(STRING, STRING) -> INT
//     usage: nestr(STRING, STRING) -> BOOL
//--------------------------------------------------------------------------------------------------b
PValue __strcmp (PNode *pnode, Context *context, bool *eflg)
{
	PNode *arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, eflg);
	if (pvalue.type != PVString) {
		prog_error(pnode, "the first argument to strcmp must be a string");
		*eflg = true;
		return nullPValue;
	}
	String str1 = pvalue.value.uString;
	arg = arg->next;
	pvalue = evaluate(arg, context, eflg);
	if (pvalue.type != PVString) {
		prog_error(pnode, "the second argument to strcmp must be a string");
		*eflg = true;
		return nullPValue;
	}
	String str2 = pvalue.value.uString;
	return PVALUE(PVInt, uInt, (long) strcmp(str1, str2));
}

//  __eqstr -- Compare two strings
//    usage: eqstr(STRING, STRING) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __eqstr (PNode *node, Context *context, bool *eflg)
{
	PNode *arg = node->arguments;
	PValue pvalue = evaluate(arg, context, eflg);
	if (pvalue.type != PVString) {
		prog_error(node, "the first argument to eqstr must be a string");
		*eflg = true;
		return nullPValue;
	}
	String left = pvalue.value.uString;
	arg = arg->next;
	pvalue = evaluate(arg, context, eflg);
	if (pvalue.type != PVString) {
		prog_error(node, "the second argument to eqstr must be a string");
		*eflg = true;
		return nullPValue;
	}
	return PVALUE(PVBool, uBool, eqstr(left, pvalue.value.uString));
}

//  __strtoint -- Convert string to integer
//    usage: strtoint(STRING) -> INT
//--------------------------------------------------------------------------------------------------
PValue __strtoint (PNode *node, Context *context, bool *eflg)
{
	PValue value = evaluate(node->arguments, context, eflg);
	if (*eflg || value.type != PVString || !value.value.uString) return nullPValue;
	return PVALUE(PVInt, uInt, atoi(value.value.uString));
}

//  __not -- Not operation
//    usage: not(INT) -> INT
//--------------------------------------------------------------------------------------------------
PValue __not (PNode *node, Context *context, bool *errflg)
{
	PValue value = evaluateBoolean(node->arguments, context, errflg);
	if (*errflg || value.type != PVBool) return nullPValue;
	return value.value.uBool ? falsePValue : truePValue;
}

//  __save -- Copy string
//--------------------------------------------------------------------------------------------------
PValue __save (PNode *pnode, Context *context, bool *errflg)
{
    String value = evaluateString(pnode->arguments, context, errflg);
	if (*errflg || !value || *value == 0) {
		prog_error(pnode, "The argument to save must be a string.");
		*errflg = true;
		return nullPValue;
	}
	return PVALUE(PVString, uString, strsave(value));
}

//  __strlen -- Find length of string
//    usage: strlen(STRING) -> INT
//--------------------------------------------------------------------------------------------------
PValue __strlen (PNode *node, Context *context, bool* eflg)
{
	PValue value = evaluate(node->arguments, context, eflg);
	if (*eflg || value.type != PVString) return nullPValue;
	return PVALUE(PVInt, uInt, (long) strlen(value.value.uString));
}

//  __concat -- Catenate potentially many strings.
//    usage: concat(STRING [, STRING]+) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __concat (PNode *pnode, Context *context, bool *errflg)
{
	PNode *arg = pnode->arguments;
	if (arg == null) return nullPValue;
	int len = 0, nstrs = 0;
	String hold[100];
	PValue svalue = evaluate(arg, context, errflg);
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
	return PVALUE(PVString, uString, nstring);
}

//  __lower -- Convert string to lower case.
//    usage: lower(STRING) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __lower(PNode *node, Context *context, bool* eflg)
{
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return PVALUE(PVString, uString, lower(val.value.uString));
}

//  __upper -- Convert string to upper case.
//    usage: upper(STRING) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __upper(PNode *node, Context *context, bool* eflg)
{
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return PVALUE(PVString, uString, upper(val.value.uString));
}

//  __capitalize -- Capitalize a string.
//    usage: capitalize(STRING) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __capitalize(PNode *node, Context *context, bool* eflg)
{
	PValue val = evaluate(node->arguments, context, eflg);
	if (*eflg || val.type != PVString) return nullPValue;
	return PVALUE(PVString, uString, capitalize(val.value.uString));
}

///*===================================
// * __print -- Print to stdout window
// *   usage: print([STRING]+,) -> VOID
// *=================================*/
//WORD __print (node, stab, eflg)
//INTERP node; TABLE stab; bool *eflg;
//{
//    INTERP arg = (INTERP) ielist(node);
//    WORD val;
//    while (arg) {
//        val = evaluate(arg, stab, eflg);
//        if (*eflg) return NULL;
//        if (val) wprintf("%s", (STRING) val);
//        arg = inext(arg);
//    }
//    return NULL;
//}

//  __root -- Return root of cached record
//    usage: root(INDI|FAM|EVEN|SOUR|OTHR) -> NODE
//--------------------------------------------------------------------------------------------------
//PValue __rot (PNode *pnode, Context *context, bool *errflg)
//{
//    String key;
//     CACHEEL cel = (CACHEEL) evaluate(ielist(node), stab, eflg);
//        if (*eflg || !cel) return NULL;
//        if (cnode(cel))
//        return (WORD) cnode(cel);
//    key = ckey(cel);
//    switch (*key) {
//    case 'I': return (WORD) key_to_indi(key); break;
//    case 'F': return (WORD) key_to_fam(key);  break;
//    case 'E': return (WORD) key_to_even(key); break;
//    case 'S': return (WORD) key_to_sour(key); break;
//    case 'X': return (WORD) key_to_othr(key); break;
//    default:  FATAL();
//    }
//}

///*=====================================
// * __trim -- Trim string if too long
// *   usage: trim(STRING, INT) -> STRING
// *===================================*/
//WORD __trim (node, stab, eflg)
//INTERP node; TABLE stab; bool *eflg;
//{
//    INTERP arg = (INTERP) ielist(node);
//    STRING str = (STRING) evaluate(arg, stab, eflg);
//    INT len;
//    if (*eflg) return NULL;
//    len = (INT) evaluate(inext(arg), stab, eflg);
//    if (*eflg) return NULL;
//    return (WORD) trim(str, len);
//}

//  __extractdate -- Extract date from EVENT or DATE NODE
//    usage: extractdate(NODE, VARB, VARB, VARB) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __extractdate (PNode *pnode, Context *context, bool* errflg)
{
    int da = 0, mo = 0, yr = 0, daormo = 0;
    String str;
    PNode *arg = pnode->arguments;
    PNode *dvar = arg->next;
    PNode *mvar = dvar->next;
    PNode *yvar = mvar->next;
    GNode *gnode = evaluateGNode(arg, context, errflg);
	if (*errflg) {
		prog_error(pnode, "The first argument to extractdate must be a event or DATE node.");
		return nullPValue;
	}
    *errflg = true;
	bool error = false;
	if (dvar->type != PNIdent) error = true;
	if (mvar->type != PNIdent) error = true;
	if (yvar->type != PNIdent) error = true;
	if (error) {
		*errflg = true;
		prog_error(pnode, "The day, month and year arguments must be identifiers.");
		return nullPValue;
	}
	// gnode should be either a DATE node or an event node.
    if (nestr("DATE", gnode->tag))
        str = eventToDate(gnode, false);
    else
        str = gnode->value;
	if (!str || *str == 0) return nullPValue;  // Not considered an error.
	String stryear;
    extractDate(str, &daormo, &da, &mo, &yr, &stryear);
    assignValueToSymbol(context->symbolTable, dvar->identifier, PVALUE(PVInt, uInt, da));
	assignValueToSymbol(context->symbolTable, mvar->identifier, PVALUE(PVInt, uInt, mo));
	assignValueToSymbol(context->symbolTable, yvar->identifier, PVALUE(PVInt, uInt, yr));
    *errflg = false;
    return nullPValue;
}

//  __extractnames -- Extract name parts from person or NAME node.
//    usage: extractnames(NODE, LIST, VARB, VARB) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __extractnames (PNode *pnode, Context *context, bool *errflg)
{
	PNode *nexp = pnode->arguments;
	PNode *lexp = nexp->next;
	PNode *lvar = lexp->next;
	PNode *svar = lvar->next;
	GNode *name = evaluateGNode(nexp, context, errflg);
	if (*errflg || nestr(name->tag, "NAME")) {
		*errflg = true;
		prog_error(pnode, "The first argument to extractnames must be a NAME node.");
		return nullPValue;
	}
	// Get the list to put the names in.
	PValue pvalue = evaluate(lexp, context, errflg);
	if (*errflg || pvalue.type != PVList) {
		*errflg = true;
		prog_error(pnode, "The second argument to extractnames must be a list.");
		return nullPValue;
	}
	List *list = pvalue.value.uList;
	bool error = false;
	if (!iistype(lvar, PNIdent)) error = true;
	if (!iistype(svar, PNIdent)) error = true;
	if (error) {
		*errflg = true;
		prog_error(pnode, "The third and fourth arguments to extract names must be identifiers.");
		return nullPValue;
	}
	String str = name->value;
	if (!str || *str == 0) {
		assignValueToSymbol(context->symbolTable, lvar->identifier, PVALUE(PVInt, uInt, 0));
		assignValueToSymbol(context->symbolTable, svar->identifier, PVALUE(PVInt, uInt, 0));
		return nullPValue;
	}
	int len, sind;
	*errflg = false;
	nameToList(str, list, &len, &sind);
	assignValueToSymbol(context->symbolTable, lvar->identifier, PVALUE(PVInt, uInt, len));
	assignValueToSymbol(context->symbolTable, svar->identifier, PVALUE(PVInt, uInt, sind));
	return nullPValue;
}

/*===============================================================
 * _extractplaces -- Extract place parts from event or PLAC NODE.
 *   usage: extractplaces(NODE, LIST, VARB) -> VOID
 *=============================================================*/
PValue __extractplaces (PNode *pnode, Context *context, bool *errflg)
{
	// Get the PLAC GNode to extract the places from.
	PNode *nexp = pnode->arguments;
	GNode *place = evaluateGNode(nexp, context, errflg);
	if (*errflg) {
		prog_error(pnode, "The first argument to extractplaces must be a PLAC or event node.");
		return nullPValue;
	}
	if (nestr(place->tag, "PLAC")) place = PLAC(place);
	if (!place) {
		*errflg = true;
		prog_error(pnode, "The first argument to extractplaces must be a PLAC or event node.");
		return nullPValue;
	}
	// Get the List to put the places in.
	PNode *lexp = nexp->next;
	PValue pvalue = evaluate(lexp, context, errflg);
	if (*errflg || pvalue.type != PVList) {
		prog_error(pnode, "The second argument to extractplaces must be a List.");
		return nullPValue;
	}
	List *list = pvalue.value.uList;
	// Get the variable to put the number of places in.
	PNode *varb = lexp->next;
	if (varb->type != PNIdent) {
		prog_error(pnode, "The third argument to extractplaces must be an identifier.");
		*errflg = true;
		return nullPValue;
	}
	String pstr = place->value;
	int len;
	placeToList(pstr, list, &len);
	assignValueToSymbol(context->symbolTable, varb->identifier, PVALUE(PVInt, uInt, len));
	return nullPValue;
}

//  __copyfile -- Copy the contents of a file to the output stream.
//    usage: copyfile(STRING) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __copyfile (PNode *node, Context *context, bool *eflg)
{
	String fileName = evaluateString(node->arguments, context, eflg);
	if (*eflg || fileName == null || strlen(fileName) == 0) {
		*eflg = true;
		prog_error(node, "The argument to copyfile must be a string.");
		return nullPValue;
	}
	FILE *cfp = fopenPath(fileName, "r", ".:$HOME");
	if (cfp == null) {
		prog_error(node, "Could not open file for copying.");
		*eflg = true;
		return nullPValue;
	}
	char buffer[1024];
	while (fgets(buffer, 1024, cfp)) {
		printf("%s", buffer);  // TODO: GOT TO CHANGE TO A MORE POUTPUT APPROACH.
	}
	fclose(cfp);
	return nullPValue;
}

//  __nl -- Newline function
//    usage: nl() -> STRING
//--------------------------------------------------------------------------------------------------
PValue __nl(PNode *pnode, Context *context, bool* errflg)
{
	return newlinePValue;
}

//  __space -- Space function
//    usage: sp() -> STRING
//--------------------------------------------------------------------------------------------------
PValue __space(PNode *pnode, Context *context, bool* errflg)
{
	return spacePValue;
}

//  __qt -- Double quote function
//    usage: qt() -> STRING
//--------------------------------------------------------------------------------------------------
PValue __qt(PNode *pnode, Context *context, bool* errflg)
{
	return quotePValue;
}

//  __children -- Return the sequence of children of a family
//--------------------------------------------------------------------------------------------------
PValue __children(PNode *pnode, Context *context, bool* errflg)
{
	GNode *family = evaluateFamily(pnode->arguments, context, errflg);
	if (*errflg || !family) return nullPValue;
	Sequence *children = familyToChildren(family, context->database);
	if (!children) return nullPValue;
	return PVALUE(PVSequence, uSequence, children);
}

//  __version -- Return the version of the DeadEnds program.
//--------------------------------------------------------------------------------------------------
PValue __version(PNode *pnode, Context *context, bool* errflg)
{
	extern String version;
	return PVALUE(PVString, uString, version);
}

//  __noop -- Used for builtins that have been removed (e.g., lock, unlock).
//--------------------------------------------------------------------------------------------------
PValue __noop(PNode *pnode, Context *context, bool* errflg) { return nullPValue; }

//  FUNCTIONS ORIGINALLY FOUND IN INTERP/WRITE.C //

//  __createnode -- Create a Gedcom node.
//    usage: createnode(STRING, STRING) -> NODE
//--------------------------------------------------------------------------------------------------
PValue __createnode (PNode *node, Context *context, bool *eflg)
{
	PNode *tagNode = node->arguments, *valNode = node->arguments->next;
	PValue tagValue = evaluate(tagNode, context, eflg);
	if (tagValue.type != PVString) {
		prog_error(node, "first argument to createnode must be a key string");
		*eflg = true;
		return nullPValue;
	}
	String tag = tagValue.value.uString;
	PValue valValue = evaluate(valNode, context, eflg);
	if (valValue.type != PVNull && valValue.type != PVString) {
		prog_error(node, "the second argument to create node must be an optional string");
		*eflg = true;
		return nullPValue;
	}
	String val  = valValue.value.uString;
	return PVALUE(PVGNode, uGNode, createGNode(null, tag, val, null));
}

//  __addnode -- Add a node to a Gedcom tree
//    usage: addnode(NODE this, NODE parent, NODE prevsib) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __addnode (PNode *node, Context *context, bool *eflg)
{
	PNode *arg1 = node->arguments, *arg2 = arg1->next, *arg3 = arg2->next;;
	PValue this = evaluate(arg1, context, eflg);
	if (*eflg || !isGNodeType(this.type)) {
		*eflg = true;
		prog_error(node, "the first argument to addnode must be an existing node");
		return nullPValue;
	}
	GNode *thisNode = this.value.uGNode;
	PValue parent = evaluate(arg2, context, eflg);
	if (*eflg || !isGNodeType(parent.type)) {
		*eflg = true;
		prog_error(node, "the second argument to addnode must be an existing node");
		return nullPValue;
	}
	GNode *parentNode = parent.value.uGNode;
	PValue prev = evaluate(arg3, context, eflg);
	if (*eflg || !isGNodeType(prev.type)) {
		*eflg = true;
		prog_error(node, "the third argument to addnode must be an existing node");
		return nullPValue;
	}
	GNode *prevNode = prev.value.uGNode;
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

//  __deletenode -- Remove node from GEDCOM tree
//    usage: deletenode(NODE) -> VOID
//    MNOTE: MEMORY LEAK?
//--------------------------------------------------------------------------------------------------
PValue __deletenode (PNode *node, Context *context, bool *eflg)
{
	PValue pvalue = evaluate(node->arguments, context, eflg);
	if (*eflg || !isGNodeType(pvalue.type)) {
		*eflg = true;
		prog_error(node, "the argument to deletenode must be an existing node");
		return nullPValue;
	}
	GNode *this = pvalue.value.uGNode;
	// If this node has no parent, it is a root node, and they cannot be deleted.
	if (!this->parent) {
		*eflg = true;
		prog_error(node, "the argument node is a root and cannot be deleted.");
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
	return nullPValue;
}
