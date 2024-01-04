//
//  DeadEnds
//
//  removeoperations.c -- Function that perform remove operations on records in Databases.
//
//  Created by Thomas Wetmore on 2 January 2024.
//  Last changed on 3 January 2024
//

#include "stdlib.h"
#include "splitjoin.h"
#include "gnode.h"
#include "gedcom.h"


//  removeChildFromFamily -- Remove an existing child from an existing family in a Database.
//-------------------------------------------------------------------------------------------------
bool removeChildFromFamily (GNode *person, GNode *family, Database *database)
{
    // Find the CHIL node in the family that links to the person.
    GNode *frefn, *husb, *wife, *chil, *rest;
    splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
    GNode *fprev = null;
    GNode *fnode = chil;
    while (fnode && nestr(person->key, fnode->value)) {
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
    splitPerson(person, &names, &irefns, &sex, &body, &famcs, &famss);
    GNode *pprev = null;
    GNode *pnode = famcs;
    while (pnode && nestr(family->key, pnode->value)) {
        pprev = pnode;
        pnode = pnode->sibling;
    }
    // If there is no FAMC link to the family, there is nothing to do.
    if (!pnode) {
        joinFamily(family, frefn, husb, wife, chil, rest);
        joinPerson(person, names, irefns, sex, body, famcs, famss);
        return false;  // Family is not family as child for the person.
    }
    // Remove the CHIL link from the family.
    if (fprev) {
        fprev->sibling = fnode->sibling;
    } else {
        chil = fnode;
    }
    // Remove the FAMC line from child.
    if (pprev) {
        pprev->sibling = pnode->sibling;
    } else {
        famcs = pnode;
    }
    freeGNode(fnode);
    freeGNode(pnode);
    joinFamily(family, frefn, husb, wife, chil, rest);
    joinPerson(person, names, irefns, sex, body, famcs, famss);
    return true;
}

//  removeSpouseFromFamily -- Remove an existing spouse from an existing family.
//-------------------------------------------------------------------------------------------------
bool removeSpouseFromFamily (GNode *spouse, GNode *family, Database *database)
{
	// Split the person and get its sex type..
	GNode *names, *irefns, *sex, *body, *famcs, *famss;
	splitPerson(spouse, &names, &irefns, &sex, &body, &famcs, &famss);
	SexType sext = sex ? valueToSex(sex) : sexUnknown;
	if (sext != sexMale && sext != sexFemale) {
		// CREATE AN ERROR
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
	// pnode is the FAMS node to remove.

	// Split the family and find the HUSB or WIFE link to remove.
	GNode *frefn, *husb, *wife, *chil, *rest;
	splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
	GNode *fprev = null;
	GNode *fnode = sext == sexMale ? husb : wife;
	GNode *fnode0 = fnode;
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
	// Now snode is the HUSB or WIFE node to remove.
	// Remove the FAMS node from the spouse.
	if (!pprev) {
		famss = pnode->sibling;
	} else {
		pprev->sibling = pnode->sibling;
	}
	joinPerson(spouse, names, irefns, sex, body, famcs, famss);
	freeGNode(pnode);
	if (!fprev) {
		fnode0 = fnode->sibling;
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
