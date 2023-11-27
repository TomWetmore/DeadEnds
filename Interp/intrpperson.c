//
//  DeadEnds
//
//  intrpperson.c -- Built-in functions dealing with persons.
//
//  Created by Thomas Wetmore on 17 March 2023.
//  Last changed on 16 November 2023.
//

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"
#include "name.h"
#include "lineage.h"
#include "interp.h"
#include "recordindex.h"
#include "database.h"

// __name -- Get a person's name.
//   usage: name(INDI [,BOOL]) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __name(PNode *pnode, Context *context, bool* errflg)
{
    // The first argument must evaluate to a person.
    PNode* arg = pnode->arguments;
    GNode *indi = evaluatePerson(arg, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(pnode, "the first argument to name must be a person");
        return nullPValue;
    }

    // If there is a second argument, it must evaluate to a boolean.
    bool useCaps = false;  // Default is not to make surname all caps.
    if ((arg = arg->next)) {
        PValue caps = evaluateBoolean(arg, context, errflg);
        if (caps.type != PVBool) {
            *errflg = true;
            prog_error(pnode, "the second argument to name must be a boolean");
            return nullPValue;
        }
        useCaps = caps.value.uBool;
    }

    // Get the person's first NAME gedcom node.
    GNode *nameNode = NAME(indi);
    if (!nameNode) {
        *errflg = true;
        prog_error(pnode, "the person argument to name does not have a name");
        return nullPValue;
    }

    //  Use manipulateName to return a string in data space.
    String name = manipulateName(nameNode->value, useCaps, true, 68);
    //  MNOTE: The program value below has a pointer to data space, not heap space.
    if (name) return PVALUE(PVString, uString, name);
    else return nullPValue;
}

//  __fullname -- Process person's name
//    usage: fullname(INDI, BOOL, BOOL, INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __fullname(PNode *pnode, Context *context, bool* eflg)
{
    // Evaluate the first argument, the person whose name is sought.
    PNode *arg = pnode->arguments;
    GNode* indi = evaluatePerson(arg, context, eflg);
    if (*eflg || !indi) {
        prog_error(pnode, "the first argument to fullname must be a person");
        return nullPValue;
    }
    // Evaluate the second argument, whether to use all caps.
    PValue pvalue = evaluate(arg = arg->next, context, eflg);
    if (*eflg || pvalue.type != PVBool) {
        prog_error(pnode, "the second argument to fullname must be a boolean");
        return nullPValue;
    }
    bool caps = pvalue.value.uBool;

    // Evaluate the third argument, whether to use surname first with comma if false.
    pvalue = evaluate(arg = arg->next, context, eflg);
    if (*eflg || pvalue.type != PVBool) {
        prog_error(pnode, "the third argument to fullname must be a boolean");
        return nullPValue;
    }
    bool reg = pvalue.value.uBool;

    // Evaluate the fourth argument, the max number of characters available.
    pvalue = evaluate(arg = arg->next, context, eflg);
    if (*eflg || pvalue.type != PVInt) {
        prog_error(pnode, "the fourth argument to fullname must be an integer");
        return nullPValue;
    }
    int len = (int) pvalue.value.uInt;

    // Get the name of the person.
    GNode* name;
    if (!(name = NAME(indi)) || !name->value) {
        *eflg = true;
        prog_error(pnode, "the person must have a name");
        return nullPValue;
    }
    return PVALUE(PVString, uString, manipulateName(name->value, caps, reg, len));
}

//  __surname -- Get a person's surname.
//    usage: surname(INDI) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __surname (PNode *pnode, Context *context, bool* errflg)
{
    GNode* gnode = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !gnode) {
        *errflg = true;
        prog_error(pnode, "the argument to surname must be a person");
        return nullPValue;
    }
    if (!(gnode = NAME(gnode)) || !gnode->value) {
        *errflg = true;
        prog_error(pnode, "the person must have a name");
        return nullPValue;
    }
    return PVALUE(PVString, uString, getSurname(gnode->value));
}

//  __givens -- Get the given names of a person. They are returned as a single string.
//    usage: givens(INDI) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __givens(PNode *pnode, Context *context, bool* errflg)
{
    GNode* this = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !this) {
        *errflg = true;
        prog_error(pnode, "the argument to givens must be a person");
        return nullPValue;
    }
    if (!(this = NAME(this)) || !this->value) {
        *errflg = true;
        prog_error(pnode, "the person must have a name");
        return nullPValue;
    }
    return PVALUE(PVString, uString, getGivenNames(this->value));
}

//  __trimname -- Trim name if too long
//    usage: trimname(INDI, INT) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __trimname (PNode *node, Context *context, bool *eflg)
{
    PNode *arg = node->arguments;
    GNode *indi = evaluatePerson(arg, context, eflg);
    if (*eflg || !indi) {
        prog_error(node, "the first argument to trimname must be a person");
        return nullPValue;
    }
    *eflg = true;
    if (!(indi = NAME(indi)) || !indi->value) {
        prog_error(node, "the person must have a name");
        return nullPValue;
    }
    *eflg = false;
    PValue length = evaluate(arg->next, context, eflg);
    if (*eflg || length.type != PVInt) {
        prog_error(node, "the second argument to trimname must be an integer");
        return nullPValue;
    }
    return PVALUE(PVString, uString, nameString(trimName(indi->value, (int) length.value.uInt)));
}

//  __birth -- Return first birth event of a person.
//    usage: birth(INDI) -> EVENT
//--------------------------------------------------------------------------------------------------
PValue __birth(PNode *pnode, Context *context, bool* errflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(pnode, "the argument to birth must be a person");
        return nullPValue;
    }
    GNode* event = BIRT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

//  __death -- Return the first death event of a person.
//    usage: death(INDI) -> EVENT
//--------------------------------------------------------------------------------------------------
PValue __death(PNode *pnode, Context *context, bool* errflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(pnode, "the argument to death must be a person");
        return nullPValue;
    }
    GNode* event = DEAT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

//  __baptism -- Return the first baptism event of a person.
//    usage: baptism(INDI) -> EVENT
//--------------------------------------------------------------------------------------------------
PValue __baptism(PNode *pnode, Context *context, bool* errflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(pnode, "the argument to baptism must be a person");
        return nullPValue;
    }
    GNode* event = BAPT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

//  __burial -- Return the first burial event of a person.
//    usage: burial(INDI) -> EVENT
//--------------------------------------------------------------------------------------------------
PValue __burial(PNode *pnode, Context *context, bool* errflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) {
        *errflg = true;
        prog_error(pnode, "the argument to burial must be a person");
        return nullPValue;
    }
    GNode* event = BURI(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

//  __father -- Return the father of a person.
//    usage: father(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __father(PNode *pnode, Context *context, bool* errflg)
{
    GNode* person = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !person) return nullPValue;
    GNode* father = personToFather(person, context->database);
    return father ? PVALUE(PVPerson, uGNode, father) : nullPValue;
}

//  __mother -- Return the mother of a person.
//    usage: mother(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __mother(PNode *pnode, Context *context, bool* errflg)
{
    GNode* person = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !person) return nullPValue;
    GNode* mother = personToMother(person, context->database);
    return mother ? PVALUE(PVPerson, uGNode, mother) : nullPValue;
}

//  __nextsib -- Find a person's next (younger) sibling, if any,
//    usage: nextsib(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __nextsib(PNode *pnode, Context *context, bool* errflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) return nullPValue;
    GNode* sib = personToNextSibling(indi, context->database);
    if (!sib) return nullPValue;
    return PVALUE(PVPerson, uGNode, sib);
}

//  __prevsib -- Find a person's previous (older) sibling.
//    usage: prevsib(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __prevsib(PNode *pnode, Context *context, bool* eflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) return nullPValue;
    GNode* sib = personToPreviousSibling(indi, context->database);
    if (!sib) return nullPValue;
    return PVALUE(PVPerson, uGNode, sib);
}

//  __sex -- Find sex, as string M, F or U, of person
//    usage: sex(INDI) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __sex (PNode *pnode, Context *context, bool* eflg)
{
    const static PValue malePValue = PVALUE(PVString, uString, "M");
    const static PValue femalePValue = PVALUE(PVString, uString, "F");
    const static PValue unknownPValue = PVALUE(PVString, uString, "U");

    GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) return nullPValue;
    GNode* sex = SEX(indi);
    if (sex && sex->value) {
        if (eqstr(sex->value, "M")) return malePValue;
        else if (eqstr(sex->value, "F")) return femalePValue;
    }
    return unknownPValue;
}

//  __male -- Check if person is male.
//    usage: male(INDI) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __male(PNode *pnode, Context *context, bool* eflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) return nullPValue;
    GNode* sex = SEX(indi);
    return (sex && sex->value && eqstr(sex->value, "M")) ? truePValue : falsePValue;
}

//  __female -- Check if a person is female.
//    usage: female(INDI) -> BOOL
//--------------------------------------------------------------------------------------------------
PValue __female(PNode *pnode, Context *context, bool* eflg)
{
    GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) return nullPValue;
    GNode* sex = SEX(indi);
    return (sex && sex->value && eqstr(sex->value, "F")) ? truePValue : falsePValue;
}

//  __pn -- Generate pronoun
//    usage: pn(INDI, INT) -> STRING
//--------------------------------------------------------------------------------------------------
static char *mpns[] = {  "He",  "he", "His", "his", "him" };
static char *fpns[] = { "She", "she", "Her", "her", "her" };
PValue __pn(PNode *node, Context *context, bool* eflg)
{
    PNode *arg = node->arguments;
    GNode* indi = evaluatePerson(arg, context, eflg);
    if (*eflg || !indi) return nullPValue;
    PValue typ = evaluate(arg->next, context, eflg);
    if (*eflg || typ.type != PVInt) return nullPValue;
    int pncode = (int) typ.value.uInt;
    if (SEXV(indi) == sexFemale) return PVALUE(PVString, uString, fpns[pncode]);
    else return PVALUE(PVString, uString, mpns[pncode]);
}

//  __nfamilies -- Find the number of families a person is a spouse in.
//    usage: nfamilies(INDI) -> INT
//--------------------------------------------------------------------------------------------------
PValue __nfamilies(PNode *node, Context *context, bool* eflg)
{
    //    GNode indi = evaluatePerson(node->pArguments, context, eflg);
    //    if (*eflg || !indi) return nullPValue;
    //    return (PValue) {PVInt, pv(.uInt = node_list_length(FAMS(indi)))};
    return nullPValue;
}

//  __nspouses -- Find the number of spouses a person has.
//    usage: nspouses(INDI) -> INT
//--------------------------------------------------------------------------------------------------
PValue __nspouses(PNode *node, Context *context, bool* eflg)
{
    GNode *indi = evaluatePerson(node->arguments, context, eflg);
    if (*eflg || !indi) return PVALUE(PVInt, uInt, 0);
    return PVALUE(PVInt, uInt, numberOfSpouses(indi, context->database));
}

// * __parents -- Find parents' family of person
// *   usage: parents(INDI) -> FAM
// *=========================================*/
//WORD __parents (PNode *expr, Table context, bool* eflg)
//{
//    NODE indi = eval_indi(ielist(node), context, eflg, NULL);
//    if (*eflg || !indi) return NULL;
//    return (WORD) fam_to_cacheel(indi_to_famc(indi));
//}

//  __title -- Get the first title of a person, if there.
//    usage: title(INDI) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __title(PNode *pnode, Context *context, bool* errflg)
{
    GNode* gnode = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !gnode) return nullPValue;
    gnode = findTag(gnode->child, "TITL");
    if (gnode && gnode->value) return PVALUE(PVString, uString, gnode->value);
    return nullPValue;
}

//  __soundex -- Return the soundex code of a person's name.
//    usage: soundex(INDI) -> STRING
//--------------------------------------------------------------------------------------------------
PValue __soundex (PNode *expr, Context *context, bool* eflg)
{
    GNode* gnode = evaluatePerson(expr->arguments, context, eflg);
    if (*eflg || !gnode || nestr(gnode->tag, "INDI")) return nullPValue;
    if (!(gnode = NAME(gnode)) || !gnode->value) {
        *eflg = true;
        return nullPValue;
    }
    return PVALUE(PVString, uString, strsave(soundex(getSurname(gnode->value))));
}

//  __inode -- Return the root of a person.
//    usage: inode(INDI) -> NODE
//--------------------------------------------------------------------------------------------------
PValue __inode(PNode *node, Context *context, bool *eflg)
{
    GNode *gnode = evaluatePerson(node->arguments, context, eflg);
    if (!gnode || nestr("INDI", gnode->tag)) {
        *eflg = true;
        prog_error(node, "the argument to inode must be a person");
        return nullPValue;
    }
    return PVALUE(PVPerson, uGNode, gnode);
}

//  __indi -- Convert a key to an person root node.
//    usage: indi(STRING) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __indi(PNode *pnode, Context *context, bool* errflg)
{
    // Get the key string.
    PValue value = evaluate(pnode->arguments, context, errflg);
    if (value.type != PVString) {
        prog_error(pnode, "the argument to indi must be a string");
        *errflg = true;
        return nullPValue;
    }
    String key = value.value.uString;

    // Get the person with the key.
    GNode* person = keyToPerson(key, context->database);
    if (person == null) {
        prog_error(pnode, "could not find a person with the key '%s'", key);
        return nullPValue;
    }
    return PVALUE(PVPerson, uGNode, person);
}

//  firstindi -- Return the first person in the database.
//    usage: firstindi() -> INDI
//--------------------------------------------------------------------------------------------------
PValue __firstindi (PNode *node, Context *context, bool *eflg)
{
    static char key[10];  // Static buffer used to hold person keys.
    int i = 0;
    *eflg = false;
    while (true) {
        sprintf(key, "I%d", ++i);
        GNode *indi = keyToPerson(key, context->database);
        if (!indi) return nullPValue;
        return PVALUE(PVPerson, uGNode, indi);
    }
}

//  nextindi -- Return the next person in the database.
//    usage: nextindi(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __nextindi (PNode *pnode, Context *context, bool *eflg)
{
    GNode *indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) {
        *eflg = true;
        prog_error(pnode, "the argument to nextindi must be a person");
        return nullPValue;
    }
    static char key[10];
    strcpy(key, personToKey(indi));
    int i = atoi(&key[1]);
    while (true) {
        sprintf(key, "I%d", ++i);
        indi = keyToPerson(key, context->database);
        if (!indi) return nullPValue;
        return PVALUE(PVPerson, uGNode, indi);
    }
}

//  previndi -- Return the previous person in the database.
//    usage: previndi(INDI) -> INDI
//--------------------------------------------------------------------------------------------------
PValue __previndi (PNode *node, Context *context, bool *eflg)
{
    GNode *indi = evaluatePerson(node->arguments, context, eflg);
    static char key[10];
    int i;
    if (*eflg) return nullPValue;
    strcpy(key, personToKey(indi));
    i = atoi(&key[1]);
    while (true) {
        sprintf(key, "I%d", --i);
        indi = keyToPerson(key, context->database);
        if (!indi) return nullPValue;
        return PVALUE(PVPerson, uGNode, indi);
    }
}

//  lastindi -- Return the last person in the database.
//    usage: lastindi() -> INDI
//--------------------------------------------------------------------------------------------------
//WORD __lastindi (node, stab, eflg)
//INTERP node; TABLE stab; BOOLEAN *eflg;
//{
//}
