// DeadEnds
//
// intrpperson.c has the built-in script functions that deal with persons.
//
// Created by Thomas Wetmore on 17 March 2023.
// Last changed on 9 May 2025.

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"
#include "name.h"
#include "lineage.h"
#include "interp.h"
#include "recordindex.h"
#include "database.h"

// __name gets a person's name.
// usage: name(INDI [,BOOL]) -> STRING
PValue __name(PNode* pnode, Context* context, bool* errflg) {
    PNode* arg = pnode->arguments;
    GNode* indi = evaluatePerson(arg, context, errflg); // Person.
    if (!*errflg && !indi) return nullPValue; // Null chaining.
    if (*errflg || !indi) {
        *errflg = true;
        scriptError(pnode, "the first argument to name must be a person");
        return nullPValue;
    }
    bool useCaps = false;
    if ((arg = arg->next)) { // Optional allcaps surname.
        PValue caps = evaluateBoolean(arg, context, errflg);
        if (caps.type != PVBool) {
            *errflg = true;
            scriptError(pnode, "the second argument to name must be a boolean");
            return nullPValue;
        }
        useCaps = caps.value.uBool;
    }
    GNode* nameNode = NAME(indi);
    if (!nameNode) {
        *errflg = true;
        scriptError(pnode, "the person does not have a name");
        return nullPValue;
    }
    String name = manipulateName(nameNode->value, useCaps, true, 68);
    if (name) return createStringPValue(name);
    else return nullPValue;
}

// __fullname gets a person's name in various format.
// usage: fullname(INDI, BOOL, BOOL, INT) -> STRING
PValue __fullname(PNode* pnode, Context* context, bool* errflg) {
    PNode* arg = pnode->arguments;
    GNode* indi = evaluatePerson(arg, context, errflg); // Person.
    if (!*errflg && !indi) return nullPValue; // Null chaining.
    if (*errflg || !indi) {
        scriptError(pnode, "the first argument to fullname must be a person");
        return nullPValue;
    }
    PValue pvalue = evaluateBoolean(arg = arg->next, context, errflg); // Allcaps boolean.
    if (*errflg || pvalue.type != PVBool) {
        scriptError(pnode, "the second argument to fullname must be a boolean");
        return nullPValue;
    }
    bool caps = pvalue.value.uBool;
    pvalue = evaluateBoolean(arg = arg->next, context, errflg); // Surname first with comma boolean.
    if (*errflg || pvalue.type != PVBool) {
        scriptError(pnode, "the third argument to fullname must be a boolean");
        return nullPValue;
    }
    bool reg = pvalue.value.uBool;
    pvalue = evaluate(arg = arg->next, context, errflg); // Max field width.
    if (*errflg || pvalue.type != PVInt) {
        scriptError(pnode, "the fourth argument to fullname must be an integer");
        return nullPValue;
    }
    int len = (int) pvalue.value.uInt;
    GNode* name;
    if (!(name = NAME(indi)) || !name->value) {
        *errflg = true;
        scriptError(pnode, "the person must have a name");
        return nullPValue;
    }
    return createStringPValue(manipulateName(name->value, caps, reg, len));
}

// __surname gets a person's surname.
// usage: surname(INDI) -> STRING
PValue __surname(PNode* pnode, Context* context, bool* errflg) {
    GNode* gnode = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (!*errflg && !gnode) return nullPValue; // Null chaining.
    if (*errflg || !gnode) {
        *errflg = true;
        scriptError(pnode, "the argument to surname must be a person");
        return nullPValue;
    }
    if (!(gnode = NAME(gnode)) || !gnode->value) {
        *errflg = true;
        scriptError(pnode, "the person must have a name");
        return nullPValue;
    }
    return createStringPValue(getSurname(gnode->value));
}

// __givens gets the given names of a person. They are returned as a single string.
// usage: givens(INDI) -> STRING
PValue __givens(PNode* pnode, Context* context, bool* errflg) {
    GNode* this = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg || !this) {
        *errflg = true;
        scriptError(pnode, "the argument to givens must be a person");
        return nullPValue;
    }
    if (!(this = NAME(this)) || !this->value) {
        *errflg = true;
        scriptError(pnode, "the person must have a name");
        return nullPValue;
    }
    return createStringPValue(getGivenNames(this->value));
}

// __trimname trims a name if too long
// usage: trimname(INDI, INT) -> STRING
PValue __trimname(PNode* pnode, Context* context, bool *eflg) {
    PNode* arg = pnode->arguments;
    GNode* indi = evaluatePerson(arg, context, eflg); // Person
    if (*eflg || !indi) {
        scriptError(pnode, "the first argument to trimname must be a person");
        return nullPValue;
    }
    *eflg = true;
    if (!(indi = NAME(indi)) || !indi->value) {
        scriptError(pnode, "the person must have a name");
        return nullPValue;
    }
    *eflg = false;
    PValue length = evaluate(arg->next, context, eflg); // Field width.
    if (*eflg || length.type != PVInt) {
        scriptError(pnode, "the second argument to trimname must be an integer");
        return nullPValue;
    }
    return createStringPValue(nameString(trimName(indi->value, (int) length.value.uInt)));
}

// __birth returns the first birth event of a person.
// usage: birth(INDI) -> EVENT
PValue __birth(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to birth() must be a person.");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* event = BIRT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

// __death returns the first death event of a person.
// usage: death(INDI) -> EVENT
PValue __death(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to death must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* event = DEAT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

// __baptism returns the first baptism event of a person.
// usage: baptism(INDI) -> EVENT
PValue __baptism(PNode *pnode, Context *context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to baptism() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* event = BAPT(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

// __burial returns the first burial event of a person.
// usage: burial(INDI) -> EVENT
PValue __burial(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to burial() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* event = BURI(indi);
    return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

// __father returns the first father of a person.
// usage: father(INDI) -> INDI
PValue __father(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to father() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* father = personToFather(indi, context->database->recordIndex);
    return father ? PVALUE(PVPerson, uGNode, father) : nullPValue;
}

// __mother returns the first mother of a person.
// usage: mother(INDI) -> INDI
PValue __mother(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to mother() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* mother = personToMother(indi, context->database->recordIndex);
    return mother ? PVALUE(PVPerson, uGNode, mother) : nullPValue;
}

// __nextsib gets a person's next (younger) sibling if any.
// usage: nextsib(INDI) -> INDI
PValue __nextsib(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg); // Person.
    if (*errflg) {
        scriptError(pnode, "the argument to nextsib() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* sibling = personToNextSibling(indi, context->database->recordIndex);
    return sibling ? PVALUE(PVPerson, uGNode, sibling) : nullPValue;
}

// __prevsib gets a person's previous (older) sibling.
// usage: prevsib(INDI) -> INDI
PValue __prevsib(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg) {
        scriptError(pnode, "the argument to prevsib() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* sibling = personToPreviousSibling(indi, context->database->recordIndex);
    return sibling ? PVALUE(PVPerson, uGNode, sibling) : nullPValue;
}

// __sex returns the sex of a person as a string M, F or U.
// usage: sex(INDI) -> STRING
PValue __sex(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg) {
        scriptError(pnode, "argument to sex() must be a person");
        return nullPValue;
    }
    if (!indi) return nullPValue;
    GNode* sex = SEX(indi);
    if (sex && sex->value) {
        if (eqstr(sex->value, "M")) return createStringPValue("M");
        else if (eqstr(sex->value, "F")) return createStringPValue("F");
    }
    return createStringPValue("U");
}

// __male checks if a person is male.
// usage: male(INDI) -> BOOL
PValue __male(PNode* pnode, Context* context, bool* errflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !indi) return nullPValue;
    GNode* sex = SEX(indi);
    return (sex && sex->value && eqstr(sex->value, "M")) ? truePValue : falsePValue;
}

// __female checks if a person is female.
// usage: female(INDI) -> BOOL
PValue __female(PNode* pnode, Context* context, bool* eflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !indi) return nullPValue;
    GNode* sex = SEX(indi);
    return (sex && sex->value && eqstr(sex->value, "F")) ? truePValue : falsePValue;
}

// __pn generates pronouns.
// usage: pn(INDI, INT) -> STRING
static char *mpns[] = {  "He",  "he", "His", "his", "him" };
static char *fpns[] = { "She", "she", "Her", "her", "her" };
PValue __pn(PNode* node, Context* context, bool* eflg) {
    PNode *arg = node->arguments;
    GNode* indi = evaluatePerson(arg, context, eflg);
    if (*eflg || !indi) return nullPValue;
    PValue typ = evaluate(arg->next, context, eflg);
    if (*eflg || typ.type != PVInt) return nullPValue;
    int pncode = (int) typ.value.uInt;
    if (SEXV(indi) == sexFemale) return createStringPValue(fpns[pncode]);
    else return createStringPValue(mpns[pncode]);
}

// __nfamilies returns the number of families a person is a spouse in.
// usage: nfamilies(INDI) -> INT
PValue __nfamilies(PNode* node, Context* context, bool* eflg) {
	GNode* indi = evaluatePerson(node->arguments, context, eflg);
	if (*eflg || !indi) return nullPValue;
	return PVALUE(PVInt, uInt, numberOfFamilies(indi));
}

// __nspouses returns the number of spouses a person has.
// usage: nspouses(INDI) -> INT
PValue __nspouses(PNode* node, Context* context, bool* eflg) {
    GNode *indi = evaluatePerson(node->arguments, context, eflg);
    if (*eflg || !indi) return PVALUE(PVInt, uInt, 0);
    return PVALUE(PVInt, uInt, numberOfSpouses(indi, context->database));
}

// __parents returns the first family a person is a child in.
// usage: parents(INDI) -> FAM
PValue __parents(PNode* pnode, Context* context, bool* eflg) {
	GNode* indi = evaluatePerson(pnode->arguments, context, eflg);
	if (*eflg || !indi) return nullPValue;
	GNode* fam = personToFamilyAsChild(indi, context->database->recordIndex);
	return fam ? PVALUE(PVFamily, uGNode, fam) : nullPValue;
}

// __title returns the first title a person has, if any.
// usage: title(INDI) -> STRING
PValue __title(PNode* pnode, Context* context, bool* errflg) {
    GNode* gnode = evaluatePerson(pnode->arguments, context, errflg);
    if (*errflg || !gnode) return nullPValue;
    gnode = findTag(gnode->child, "TITL");
    if (gnode && gnode->value) return createStringPValue(gnode->value);
    return nullPValue;
}

// __soundex returns the soundex code of a person's name.
// usage: soundex(INDI) -> STRING
PValue __soundex (PNode* pnode, Context* context, bool* eflg) {
    GNode* gnode = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !gnode || nestr(gnode->tag, "INDI")) return nullPValue;
    if (!(gnode = NAME(gnode)) || !gnode->value) {
        *eflg = true;
        return nullPValue;
    }
    return createStringPValue(soundex(getSurname(gnode->value)));
}

// __inode returns the root of a person.
// usage: inode(INDI) -> NODE
PValue __inode(PNode* pnode, Context* context, bool* eflg) {
    GNode *gnode = evaluatePerson(pnode->arguments, context, eflg);
    if (*eflg || !gnode || nestr("INDI", gnode->tag)) {
        *eflg = true;
        scriptError(pnode, "the argument to inode must be a person");
        return nullPValue;
    }
    return PVALUE(PVPerson, uGNode, gnode);
}

// __indi converts a key to a person's root node.
// usage: indi(STRING) -> INDI
PValue __indi(PNode* pnode, Context* context, bool* errflg) {
    PValue value = evaluate(pnode->arguments, context, errflg); // Key.
    if (value.type != PVString) {
        scriptError(pnode, "the argument to indi must be a string");
        *errflg = true;
        return nullPValue;
    }
    String key = keyToKey(value.value.uString);
    GNode* person = keyToPerson(key, context->database->recordIndex);
    if (person == null) {
        scriptError(pnode, "could not find a person with the key '%s'", key);
        return nullPValue;
    }
    return PVALUE(PVPerson, uGNode, person);
}

// firstindi returns the first person in the database.
// usage: firstindi() -> INDI
extern int rootNameCompare(void* a, void* b);
extern String rootNameGet(void* a);
extern void checkPersonRoots(RootList*);

PValue __firstindi(PNode* pnode, Context* context, bool* eflg) {
	List *personRoots = context->database->personRoots;
	if (!personRoots || lengthList(personRoots) == 0) {
		*eflg = true;
		scriptError(pnode, "There must be persons in the database to call firstindi.");
		return nullPValue;
	}
	sortList(personRoots);
	GNode *root = getListElement(personRoots, 0);
	return PVALUE(PVPerson, uGNode, root);
}

// nextindi returns the next person in the database.
// usage: nextindi(INDI) -> INDI
PValue __nextindi(PNode* pnode, Context* context, bool* eflg) {
    GNode* indi = evaluatePerson(pnode->arguments, context, eflg); // Previous person.
    if (*eflg || !indi) {
        *eflg = true;
        scriptError(pnode, "The argument to nextindi must be a person.");
        return nullPValue;
    }
	List *personRoots = context->database->personRoots;
	sortList(personRoots);
	int index;
	GNode* cur = findInList(personRoots, indi->key, &index);
	if (indi != cur || index < 0 || index >= lengthList(personRoots)) {
		*eflg = true;
		scriptError(pnode, "The argument person doesn't have a valid index; call maintenance.");
		return nullPValue;
	}
	if (index == lengthList(personRoots) - 1) { // At last person.
		return nullPValue;
	}
	return PVALUE(PVPerson, uGNode, getListElement(personRoots, index + 1));
}

// previndi returns the previous person in the database.
// usage: previndi(INDI) -> INDI
PValue __previndi(PNode* pnode, Context* context, bool* eflg) {
	GNode* indi = evaluatePerson(pnode->arguments, context, eflg); // Following person.
	if (*eflg || !indi) {
		*eflg = true;
		scriptError(pnode, "The argument to previndi must be a person.");
		return nullPValue;
	}
	List *personRoots = context->database->personRoots;
	sortList(personRoots);
	int index;
	GNode* cur = findInList(personRoots, indi->key, &index);
	if (indi != cur || index < 0 || index >= lengthList(personRoots)) {
		*eflg = true;
		scriptError(pnode, "The argument person doesn't have a valid index; call maintenance.");
		return nullPValue;
	}
	if (index == 0) { // At first person.
		return nullPValue;
	}
	return PVALUE(PVPerson, uGNode, (GNode*) getListElement(personRoots, index - 1));
}

// lastindi returns the last person in the database.
// usage: lastindi() -> INDI
PValue __lastindi(PNode* pnode, Context* context, bool* eflg) {
	List *personRoots = context->database->personRoots;
	if (!personRoots || lengthList(personRoots) == 0) {
		*eflg = true;
		scriptError(pnode, "There must be persons in the database to call lastindi.");
		return nullPValue;
	}
	sortList(personRoots);
	return PVALUE(PVPerson, uGNode, (GNode*) getListElement(personRoots, lengthList(personRoots) - 1));
}
