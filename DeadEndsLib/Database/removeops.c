// DeadEnds
//
// removeops.c has functions that perform remove operations on records in Databases.
//
// Created by Thomas Wetmore on 2 January 2024.
// Last changed on 18 May 2025.
//

#include "stdlib.h"
#include "splitjoin.h"
#include "gnode.h"
#include "gedcom.h"

// removeChildFromFamily removes an existing child from an existing family.
bool removeChildFromFamily(GNode* child, GNode* family) {
    // Find the CHIL node in the family that links to the person.
    GNode *frefn, *husb, *wife, *chil, *rest;
    splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
    GNode *fprev = null;
    GNode *fnode = chil;
    while (fnode && nestr(child->key, fnode->value)) {
        fprev = fnode;
        fnode = fnode->sibling;
    }
    // If there is no CHIL link to the person, there is nothing to do.
    if (!fnode) {
        joinFamily(family, frefn, husb, wife, chil, rest);
        return false;  // Person is not a child in this family.
    }
    // Find the FAMC node in the child that links to the family.
    GNode *names, *irefns, *sex, *body, *famcs, *famss;
    splitPerson(child, &names, &irefns, &sex, &body, &famcs, &famss);
    GNode *pprev = null;
    GNode *pnode = famcs;
    while (pnode && nestr(family->key, pnode->value)) {
        pprev = pnode;
        pnode = pnode->sibling;
    }
    // If there is no FAMC link to the family, there is nothing to do.
    if (!pnode) {
        joinFamily(family, frefn, husb, wife, chil, rest);
        joinPerson(child, names, irefns, sex, body, famcs, famss);
        return false;  // Family is not family as child for the person.
    }
    // Remove the CHIL link from the family.
    if (fprev) {
        fprev->sibling = fnode->sibling;
    } else {
        chil = fnode->sibling;
    }
    // Remove the FAMC line from child.
    if (pprev) {
        pprev->sibling = pnode->sibling;
    } else {
        famcs = pnode->sibling;
    }
    freeGNode(fnode);
    freeGNode(pnode);
    joinFamily(family, frefn, husb, wife, chil, rest);
    joinPerson(child, names, irefns, sex, body, famcs, famss);
    return true;
}

// removeSpouseFromFamily removes an existing spouse from an existing family.
bool removeSpouseFromFamily(GNode* spouse, GNode* family, Error* error) {
	// Split the person and get its sex type.
	GNode *names, *irefns, *sex, *body, *famcs, *famss;
	splitPerson(spouse, &names, &irefns, &sex, &body, &famcs, &famss);
	SexType sext = sex ? valueToSex(sex) : sexUnknown;
	if (sext != sexMale && sext != sexFemale) {
        joinPerson(spouse, names, irefns, sex, body, famcs, famss);
        // CREATE ERROR
		return false;
	}
	// Find the FAMS node to remove from the spouse.
	GNode *pprev = null;
	GNode *pnode = famss;
	while (pnode && nestr(pnode->value, family->key)) {
		pprev = pnode;
		pnode = pnode->sibling;
	}
	// If the FAMS node is not found report an error.
	if (!pnode) {
		// Create error here.
		joinPerson(spouse, names, irefns, sex, body, famcs, famss);
		return false;
	}
	// Now pnode is the FAMS to remove if the HUSB or WIFE link is found.
	// Split the family and find the HUSB or WIFE link to remove.
	GNode *frefn, *husb, *wife, *chil, *rest;
	splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
	GNode *fprev = null;
	GNode *fnode = sext == sexMale ? husb : wife;
	while (fnode && nestr(fnode->value, spouse->key)) {
		fprev = fnode;
		fnode = fnode->sibling;
	}
	if (!fnode) {
		// Create error here.
		joinFamily(family, frefn, husb, wife, chil, rest);
		joinPerson(spouse, names, irefns, sex, body, famcs, famss);
		return false;
	}
	// Now fnode is the HUSB or WIFE node to remove.
	// Remove the FAMS node from the spouse.
	if (!pprev) {
		famss = pnode->sibling;
	} else {
		pprev->sibling = pnode->sibling;
	}
    // Remove the HUSB or WIFE node from the family.
    if (!fprev) {
        if (sext == sexMale)
            husb = fnode->sibling;
        else
            wife = fnode->sibling;
    } else {
        fprev->sibling = fnode->sibling;
    }
	// Put the spouse and family back together and free the two removed nodes.
	joinPerson(spouse, names, irefns, sex, body, famcs, famss);
	joinFamily(family, frefn, husb, wife, chil, rest);
	freeGNode(pnode);
	freeGNode(fnode);
	return true;
}
