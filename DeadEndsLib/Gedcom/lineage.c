// DeadEnds
//
// lineage.c holds perations on GNodes based on genealogical relationsips and properties.
//
// Created by Thomas Wetmore on 17 February 2023.
// Last changed on 12 May 2024.

#include "lineage.h"
#include "gnode.h"
#include "name.h"

static bool debugging = false;

// personToFather returns the father of a person, the first HUSB in the first FAMC in the person.
GNode* personToFather(GNode* node, Database* database) {
	return familyToHusband(personToFamilyAsChild(node, database), database);
}

// personToMother returns the mother of a person, the first WIFE in the first FAMC in the person.
GNode* personToMother(GNode* node, Database* database) {
	return familyToWife(personToFamilyAsChild(node, database), database);
}

// personToPreviousSibling returns the previous sibling of a person, the previous CHIL in the
// first FAMC in the person.
GNode* personToPreviousSibling(GNode* indi, Database* database) {
	if (!indi) return null;
	GNode* famc = personToFamilyAsChild(indi, database);
	if (!famc) return null;
	GNode* prev = null;
	GNode* node = CHIL(famc);
	while (node && eqstr("CHIL", node->tag)) {
		if (eqstr(indi->key, node->value)) {
			if (!prev) return null;
			return keyToPerson(prev->value, database);
		}
		prev = node;
		node = node->sibling;
	}
	return null;
}

// personToNextSibling returns the next sibling of a person, the next CHIL in the first FAMC
// in the person.
GNode* personToNextSibling(GNode* indi, Database* database) {
	if (!indi) return null;
	GNode* fam = personToFamilyAsChild(indi, database);
	if (!fam) return null;
	GNode* node = CHIL(fam);
	while (node && eqstr("CHIL", node->tag)) {
		if (eqstr(indi->key, node->value)) break;
		node = node->sibling;
	}
	if (!node) return null;
	node = node->sibling;
	if (!node || nestr("CHIL", node->tag)) return null;
	return keyToPerson(node->value, database);
}

// familyToHusband -return the first husband of a family, the first HUSB in the family.
GNode* familyToHusband(GNode* node, Database* database) {
	if (!node) return null;
	if (!(node = findTag(node->child, "HUSB"))) return null;
	return keyToPerson(node->value, database);
}

// familyToWife returns the first wife of a family, the first WIFE in the family.
GNode* familyToWife(GNode* node, Database* database) {
	if (!node) return null;
	if (!(node = findTag(node->child, "WIFE"))) return null;
	return keyToPerson(node->value, database);
}

// familyToSpouse return the first spouse with given sex from a family.
GNode* familyToSpouse(GNode* family, SexType sex, Database* database) {
	if (sex != sexMale && sex != sexFemale) return null;
	return (sex == sexMale) ? familyToHusband(family, database) : familyToWife(family, database);
}

// personToSpouse -- Return the first spouse from the ...
GNode *personToSpouse(GNode *person, GNode *family)
//  family -- If present the spouse must be from this family. If null the spouse must be the first
//            person of the opposite sex in the first family the person is a spouse is that has
//            such a spouse.
{
	if (debugging) printf("personToSpouse called on person %s\n", person->key);
	return null;
}

// familyToFirstChild returns the first child of a family, the first CHIL in the family.
GNode* familyToFirstChild(GNode* node, Database* database) {
	if (!node) return null;
	if (!(node = CHIL(node))) return null;
	return keyToPerson(node->value, database);
}

// familyToLastChild return the last child of a family, the last CHIL in the family.
GNode* familyToLastChild(GNode* node, Database* database) {
	if (!node) return null;
	if (!(node = CHIL(node))) return null;
	GNode* chil = null;
	while (node) {
		if (eqstr(node->tag, "CHIL")) chil = node;
		node = node->sibling;
	}
	return keyToPerson(chil->value, database);
}

// numberOfSpouses returns the number of spouses of a person.
int numberOfSpouses(GNode* person, Database* database) {
	if (!person) return 0;
	int nspouses = 0;
	FORSPOUSES(person, spouse, family, count, database)
		if (spouse) nspouses++;
	ENDSPOUSES
	return nspouses;
}

// numberOfFamilies returns the number of families a person is a spouse in.
int numberOfFamilies(GNode* person) {
	if (!person) return 0;
	int nfamilies = 0;
	GNode* fams = FAMS(person);
	while (fams && eqstr("FAMS", fams->tag)) {
		nfamilies++;
		fams = fams->sibling;
	}
	return nfamilies;
}

// personToFamilyAsChild returns the first family a person is in as a child.
GNode* personToFamilyAsChild(GNode* person, Database* database) {
	if (!person) return null;
	if (!(person = FAMC(person))) return null;
	return keyToFamily(person->value, database);
}

// personToName returns the name of a person, the value of the first NAME in the person. length
// is the max number of characters to use for the name.
String personToName(GNode* person, int length) {
	if (!person) return "";
	if (!(person = findTag(person->child, "NAME"))) return "";
	return manipulateName(person->value, true, true, length);
}

// personToTitle returns the title of a person, the value of the first TITL node in the person.
String personToTitle(GNode* indi, int len) {
	if (!indi) return null;
	if (!(indi = findTag(indi->child, "TITL"))) return null;
	return indi->value;
}

// oppositeSex returns the opposite sex of a SexType.
SexType oppositeSex(SexType sex) {
	if (sex == sexMale) return sexFemale;
	if (sex == sexFemale) return sexMale;
	return sexUnknown;
}
