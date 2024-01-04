//  
//  DeadEnds
//
//  addoperations.c -- Add operations for a Database.
//
//  Created by Thomas Wetmore on 30 December 2023.
//  Last changed on 2 January 2024.
//

#include "stdlib.h"
#include "splitjoin.h"
#include "gnode.h"
#include "gedcom.h"

//  addChildToFamily -- Add an existing child to an existing family in a Database.
//-------------------------------------------------------------------------------------------------
bool addChildToFamily (GNode *person, GNode *family, int index, Database *database)
//  person -- Person to add as a child to the family.
//  family -- Family to add the child to.
//  index -- Order number for the new child.
//  database -- Database with the person and family.
{
    GNode *frefn, *husb, *wife, *chil, *rest;
    splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
	GNode *prev = null;
	GNode *node = chil;
	if (index == -1) {  // Make the new child the last in the family.
		if (node) {
			while (node) {
				prev = node;
				node = node->sibling;
			}
		}
	} else {
        int j = 0;
		while (j++ < index) {
			prev = node;
			node = node->sibling;
		}
	}
    GNode *new = createGNode(null, "CHIL", person->key, family);
	new->sibling = node;
	if (prev)
		prev->sibling = new;
	else
		chil = new;
	joinFamily(family, frefn, husb, wife, chil, rest);

	// Add a FAMC node to the child.
    GNode *names, *irefns, *sex, *body, *famcs, *famss;
    splitPerson(person, &names, &irefns, &sex, &body, &famcs, &famss);
	GNode *nfmc = createGNode(null, "FAMC", family->key, person);
	prev = null;
	GNode *this = famcs;
	while (this) {
		prev = this;
		this = this->sibling;
	}
	if (!prev)
		famcs = nfmc;
	else
		prev->sibling = nfmc;
	joinPerson(person, names, irefns, sex, body, famcs, famss);

    return true;
}

//  addSpouseToFamily -- Add an existing spouse to an existing family.
//-------------------------------------------------------------------------------------------------
bool addSpouseToFamily (GNode *spouse, GNode *family, SexType sext, Database *database)
{
    // Add a HUSB or WIFE node to the family.
	GNode *frefn, *husb, *wife, *chil, *rest;
    splitFamily(family, &frefn, &husb, &wife, &chil, &rest);
    GNode *prev = null;
    GNode *this = null;
	if (sext == sexMale) {
		prev = null;
		this = husb;
		while (this) {
			prev = this;
			this = this->sibling;
		}
		GNode *new = createGNode(NULL, "HUSB", spouse->key, family);
		if (prev)
			prev->sibling = new;
		else
			husb = new;
	} else {
		prev = null;
		this = wife;
		while (this) {
			prev = this;
			this = this->sibling;
		}
		GNode *new = createGNode(NULL, "WIFE", spouse->key, family);
		if (prev)
			prev->sibling = new;
		else
			wife = new;
	}
	joinFamily(family, frefn, husb, wife, chil, rest);

    // Add a FAMS node to the spouse.
    GNode *names, *irefns, *sex, *body, *famcs, *famss;
    splitPerson(spouse, &names, &irefns, &sex, &body, &famcs, &famss);
	GNode *nfams = createGNode(NULL, "FAMS", family->key, spouse);

    prev = null;
	this = famss;
	while (this) {
		prev = this;
		this = this->sibling;
	}
	if (!prev)
		famss = nfams;
	else
		prev->sibling = nfams;
	joinPerson(spouse, names, irefns, sex, body, famcs, famss);
    return true;
}
