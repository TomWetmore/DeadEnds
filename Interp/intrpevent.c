//
//  intrpevent.c -- The built-in functions for events, dates and places.
//  JustParsing
//
//  Created by Thomas Wetmore on 17 March 2023.
//  Last changed on 16 November 2023.
//

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"

static int daycode = 0;
static int monthcode = 3;
static int datecode = 0;

///*===============================
// * __date -- Return date of event
// *   usage: date(EVENT) -> STRING
// *=============================*/
//WORD __date (node, stab, eflg)
//INTERP node; TABLE stab; bool *eflg;
//{
//    NODE evnt = (NODE) evaluate(ielist(node), stab, eflg);
//    TRANTABLE ttr = tran_tables[MINRP];
//    if (*eflg || !evnt) return NULL;
//    return (WORD) eventToDate(evnt, ttr, FALSE);
//}

//  __place -- Return the place of event, the value of the first PLAC GNode in an event.
//    usage: place(EVENT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __place(PNode *pnode, Context *context, bool* errflg)
{
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isGNodeType(pvalue.type)) return nullPValue;
    String place = event_to_plac(pvalue.value.uGNode, false);
    if (place) return PVALUE(PVString, uString, strsave(place));
    return nullPValue;
}

//  __year -- Return the year of a Gedcom event as a string.
//    usage: year(EVENT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __year(PNode *pnode, Context *context, bool* errflg)
{
    PValue evnt = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isGNodeType(evnt.type)) return nullPValue;
    return PVALUE(PVString, uString, strsave(eventToDate(evnt.value.uGNode, true)));
}

//  __long -- Return the long form of an event as a string.
//    usage: long(EVENT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __long(PNode *pnode, Context *context, bool* errflg)
{
    PValue event = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isRecordType(event.type)) return nullPValue;
    return PVALUE(PVString, uString, eventToString(event.value.uGNode, false));
}

//  __short -- Return the long form of an event as a string.
//    usage: short(EVENT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __short (PNode *pnode, Context *context, bool* errflg)
{
    PValue event = evaluate(pnode->arguments, context, errflg);
    if (*errflg || !isRecordType(event.type)) return nullPValue;
    return PVALUE(PVString, uString, eventToString(event.value.uGNode, true));
}

//  __dayformat -- Set day format for standard dates.
//    usage: dayformat(INT) -> NULL
//--------------------------------------------------------------------------------------------------
PValue __dayformat (PNode *node, Context *context, bool* eflg)
{
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 2) daycode = code;
    return nullPValue;
}

//  __monthformat -- Set the month format for showing standard dates.
//    usage: dayformat(INT) -> NULL
//--------------------------------------------------------------------------------------------------
PValue __monthformat(PNode *node, Context *context, bool* eflg)
{
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 6) monthcode = code;
    return nullPValue;
}

//  __dateformat -- Set date format for standard date
//    usage: dateformat(INT) -> NULL
//--------------------------------------------------------------------------------------------------
PValue __dateformat(PNode *node, Context *context, bool* eflg)
{
    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || pvalue.type != PVInt) return nullPValue;
    int code = (int) pvalue.value.uInt;
    if (code >= 0 && code <= 11) datecode = code;
    return nullPValue;
}

//  __stddate -- Return standard date format of event
//    usage: stddate(EVENT) -> STRING
//--------------------------------------------------------------------------------------------------


PValue __stddate(PNode *node, Context *context, bool* eflg)
{
    extern String format_date(String, int, int, int, int, bool);

    PValue pvalue = evaluate(node->arguments, context, eflg);
    if (*eflg || !isGNodeType(pvalue.type)) return nullPValue;
    GNode* gnode = pvalue.value.uGNode;
    String date = format_date(eventToDate(gnode, false), daycode, monthcode, 1, datecode, false);
    return PVALUE(PVString, uString, strsave(date));
}

//  __gettoday -- Create today's event
//    usage: gettoday() --> EVENT
//--------------------------------------------------------------------------------------------------
//PValue __gettoday(PNode expr, Context *context, bool* eflg)
//{
//    GNode prnt = createGNode(null, "EVEN", null, null);
//    GNode chil = createGNode(null, "DATE", get_date(), prnt);
//    prnt->gChild = chil;
//    *eflg = false;
//    return (PValue) {.pvType = PVGNode, .pvValue = pv(.uGNode = prnt)};
//}
