// DeadEnds
//
// intrpevent.c has the built-in functions for events, dates and places.
//
// Created by Thomas Wetmore on 17 March 2023.
// Last changed on 30 April 2025.

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"

static int daycode = 0;
static int monthcode = 3;
static int datecode = 0;

// __date returns the date of an event.
// usage: date(EVENT) -> STRING
PValue __date(PNode *pnode, Context *context, bool *errflg) {
	PValue pvalue = evaluate(pnode->arguments, context, errflg);
	if (*errflg || !isGNodeType(pvalue.type)) return nullPValue;
	String date = eventToDate(pvalue.value.uGNode, false);
	if (date) return createStringPValue(date);
	return nullPValue;
}

// __place returns the place of an event, the value of the first PLAC GNode in an event.
//  usage: place(EVENT) -> STRING
PValue __place(PNode *pnode, Context *context, bool *errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isGNodeType(pvalue.type)) return nullPValue;
    String place = eventToPlace(pvalue.value.uGNode, false);
    if (place) return createStringPValue(place);
    return nullPValue;
}

// __year return the year of a Gedcom event as a string.
// usage: year(EVENT) -> STRING
PValue __year(PNode *pnode, Context *context, bool* errflg) {
    PValue evnt = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isGNodeType(evnt.type)) return nullPValue;
    return createStringPValue(eventToDate(evnt.value.uGNode, true));
}

//  __long -- Return the long form of an event as a string.
//    usage: long(EVENT) -> STRING
PValue __long(PNode *pnode, Context *context, bool* errflg) {
    PValue event = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isRecordType(event.type)) return nullPValue;
    return createStringPValue(eventToString(event.value.uGNode, false));
}

//  __short -- Return the long form of an event as a string.
//    usage: short(EVENT) -> STRING
PValue __short (PNode *pnode, Context *context, bool* errflg) {
    PValue event = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isRecordType(event.type)) return nullPValue;
    return createStringPValue(eventToString(event.value.uGNode, true));
}

//  __dayformat -- Set day format for standard dates.
//    usage: dayformat(INT) -> NULL
PValue __dayformat (PNode *node, Context *context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 2) daycode = code;
    return nullPValue;
}

//  __monthformat -- Set the month format for showing standard dates.
//    usage: dayformat(INT) -> NULL
PValue __monthformat(PNode *node, Context *context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 6) monthcode = code;
    return nullPValue;
}

//  __dateformat -- Set date format for standard date
//    usage: dateformat(INT) -> NULL
PValue __dateformat(PNode *node, Context *context, bool* eflg) {
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 11) datecode = code;
    return nullPValue;
}

//  __stddate -- Return standard date format of event
//    usage: stddate(EVENT) -> STRING
PValue __stddate(PNode *node, Context *context, bool* eflg) {
    extern String formatDate(String, int, int, int, int, bool);

    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || !isGNodeType(pvalue.type)) return nullPValue;
    GNode* gnode = pvalue.value.uGNode;
    String date = formatDate(eventToDate(gnode, false), daycode, monthcode, 1, datecode, false);
    return createStringPValue(date);
}

//  __gettoday -- Create today's event
//    usage: gettoday() --> EVENT
PValue __gettoday(PNode *expr, Context *context, bool* eflg) {
	GNode *prnt = createGNode(null, "EVEN", null, null);
	GNode *chil = createGNode(null, "DATE", get_date(), prnt);
	prnt->child = chil;
	*eflg = false;
	return PVALUE(PVGNode, uGNode, prnt);
}
