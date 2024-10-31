// DeadEnds
//
// intrpfamily.c
//
// Created by Thomas Wetmore on 17 March 2023.
// Last changed on 23 October 2024.

#include "standard.h"
#include "pnode.h"
#include "pvalue.h"
#include "evaluate.h"
#include "lineage.h"
#include "database.h"

// __marriage returns the first marriage event of a family.
// usage: marriage(FAM) -> EVENT
PValue __marriage(PNode* pnode, Context* context, bool* errflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, errflg);
	if (*errflg || !fam) return nullPValue;
	GNode* event = MARR(fam);
	return event ? PVALUE(PVEvent, uGNode, event) : nullPValue;
}

// __husband returns the first husband of a family.
// usage: husband(FAM) -> INDI
PValue __husband(PNode* pnode, Context* context, bool* errflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, errflg);
	if (*errflg || !fam) return nullPValue;
	GNode* husband = familyToHusband(fam, context->database);
	if (!husband) return nullPValue;
	return PVALUE(PVPerson, uGNode, husband);
}

// __wife returns the first wife of a family.
// usage: wife(FAM) -> INDI
PValue __wife(PNode* pnode, Context* context, bool* errflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, errflg);
	if (*errflg || !fam) return nullPValue;
	GNode* wife = familyToWife(fam, context->database);
	if (!wife) return nullPValue;
	return PVALUE(PVPerson, uGNode, wife);
}

// __nchildren returns the number of children in a family.
// usage: nchildren(FAM) -> INT
PValue __nchildren (PNode* pnode, Context* context, bool* eflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, eflg);
	if (*eflg || !fam) return nullPValue;
	int count = 0;
	GNode* this = CHIL(fam);
	while (this && eqstr("CHIL", this->tag)) {
		count++;
		this = this->sibling;
	}
	return PVALUE(PVInt, uInt, (long) count);
}

// __firstchild returns the first child of a family.
// usage: firstchild(FAM) -> INDI
PValue __firstchild(PNode* pnode, Context* context, bool* eflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, eflg);
	if (*eflg || !fam) return nullPValue;
	GNode* child = familyToFirstChild(fam, context->database);
	if (!child) return nullPValue;
	return PVALUE(PVPerson, uGNode, child);
}

// __lastchild returns the last child of a family.
// usage: lastchild(FAM) -> INDI
PValue __lastchild(PNode* pnode, Context* context, bool* eflg) {
	ASSERT(pnode && context);
	GNode* fam = evaluateFamily(pnode->arguments, context, eflg);
	if (*eflg || !fam) return nullPValue;
	GNode* child = familyToLastChild(fam, context->database);
	if (!child) return nullPValue;
	return PVALUE(PVPerson, uGNode, child);
}

// __fnode returns the root node of a family.
// usage: fnode(FAM) -> NODE
PValue __fnode(PNode* pnode, Context* context, bool* eflg) {
	GNode *gnode = evaluateFamily(pnode->arguments, context, eflg);
	if (*eflg || !gnode || nestr("FAM", gnode->tag)) {
		*eflg = true;
		scriptError(pnode, "the argument to fnode must be a family.");
		return nullPValue;
	}
	return PVALUE(PVFamily, uGNode, gnode);
}

// __fam converts family key to the family.
// usage: fam(STRING) -> FAM
PValue __fam(PNode* pnode, Context* context, bool* eflg) {
	ASSERT(pnode && context);
	PValue value = evaluate(pnode->arguments, context, eflg);
	if (value.type != PVString) {
		scriptError(pnode, "the argument must be a string\n");
		*eflg = true;
		return nullPValue;
	}
	String key = value.value.uString;
	GNode* family = keyToFamily(key, context->database); // Find family with key.
	if (!family) {
		scriptError(pnode, "Could not find a family with key %s.\n", key);
		return nullPValue;
	}
	return PVALUE(PVFamily, uGNode, family);
}

// firstfam returns the first family in the database in key order.
// usage: firstfam() -> FAM
PValue __firstfam(PNode* pnode, Context* context, bool* eflg) {
	List *familyRoots = context->database->familyRoots;
	if (!familyRoots || lengthList(familyRoots) == 0) {
		*eflg = true;
		scriptError(pnode, "There must be families in the database to call firstfam.");
		return nullPValue;
	}
	sortList(familyRoots);
	GNode *root = getListElement(familyRoots, 0);
	return PVALUE(PVFamily, uGNode, root);
}

// nextfam returns the next family in the database in key order.
// usage: nextfam(FAM) -> FAM
PValue __nextfam(PNode* pnode, Context* context, bool* eflg) {
	GNode* fam = evaluateFamily(pnode->arguments, context, eflg); // Current family.
	if (*eflg || !fam) {
		*eflg = true;
		scriptError(pnode, "The argument to nextfam must be a family.");
		return nullPValue;
	}
	List *familyRoots = context->database->familyRoots;
	sortList(familyRoots);
	int index;
	GNode* cur = findInList(familyRoots, fam->key, &index);
	if (fam != cur || index < 0 || index >= lengthList(familyRoots)) {
		*eflg = true;
		scriptError(pnode, "The argument family doesn't have a valid index; call maintenance.");
		return nullPValue;
	}
	if (index == lengthList(familyRoots) - 1) { // At last family.
		return nullPValue;
	}
	return PVALUE(PVFamily, uGNode, getListElement(familyRoots, index + 1));
}

// prevfam returns the previous family in the database.
// usage: prevfam(FAM) -> FAM
PValue __prevfam(PNode* pnode, Context* context, bool* eflg) {
	GNode* fam = evaluateFamily(pnode->arguments, context, eflg); // Following person.
	if (*eflg || !fam) {
		*eflg = true;
		scriptError(pnode, "The argument to prevfam must be a family.");
		return nullPValue;
	}
	List *familyRoots = context->database->familyRoots;
	sortList(familyRoots);
	int index;
	GNode* cur = findInList(familyRoots, fam->key, &index);
	if (fam != cur || index < 0 || index >= lengthList(familyRoots)) {
		*eflg = true;
		scriptError(pnode, "The argument person doesn't have a valid index; call maintenance.");
		return nullPValue;
	}
	if (index == 0) { // At first family.
		return nullPValue;
	}
	return PVALUE(PVFamily, uGNode, (GNode*) getListElement(familyRoots, index - 1));
}

// lastfam returns the last family in the database.
// usage: lastfam() -> FAM
PValue __lastfam(PNode* pnode, Context* context, bool* eflg) {
	List *familyRoots = context->database->familyRoots;
	if (!familyRoots || lengthList(familyRoots) == 0) {
		*eflg = true;
		scriptError(pnode, "There must be families in the database to call lastfam.");
		return nullPValue;
	}
	sortList(familyRoots);
	return PVALUE(PVFamily, uGNode, (GNode*) getListElement(familyRoots, lengthList(familyRoots) - 1));
}
