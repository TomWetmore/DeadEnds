// DeadEnds
//
// createfamily.c creates a new family in a Database.
//
// Created by Thomas Wetmore on 30 May 2024.
// Last changed on 8 June 2024.

#include "database.h"
#include "gnode.h"
#include "gedcom.h"
#include "splitjoin.h"

static bool checkFamilyMember(GNode*, SexType);

// createFamily creates a new family record. It does not add it to the Database.
GNode* createFamily(GNode* husb, GNode* wife, GNode* chil, GNode* rest, Database* database) {
	if (!husb && !wife && !chil) return null; // Must be at least one person in family.
	checkFamilyMember(husb, sexMale);
	checkFamilyMember(wife, sexFemale);
	checkFamilyMember(chil, sexUnknown);
	GNode* family = createGNode(generateFamilyKey(database), "FAM", null, null);
	joinFamily(family, null, husb, wife, chil, rest);
	return family;
}

// checkFamilyMember checks if a person can be added to a new family.
static bool checkFamilyMember(GNode* person, SexType sex) {
	if (!person) return true;
	if (nestr(person->tag, "INDI")) return false;
	if (sex == sexUnknown) return true;
	GNode* snode = findTag(person, "SEX)");
	if (!snode || !snode->value || nestr(snode->value, sexTypeToString(sex))) return false;
	return true;
}
