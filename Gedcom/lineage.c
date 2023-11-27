//
//  DeadEnds
//
//  lineage.c -- Operations on Gedcom nodes based on genealogical relationsips and properties.
//
//  Created by Thomas Wetmore on 17 February 2023.
//  Last changed on 14 November 2023.
//

#include "lineage.h"
#include "gnode.h"
#include "name.h"

static bool debugging = false;

//  personToFather -- Return the father of a person. This is the first HUSB in the first FAMC
//    of the person's INDI record.
//--------------------------------------------------------------------------------------------------
GNode* personToFather(GNode* node, Database *database)
{
	return familyToHusband(personToFamilyAsChild(node, database), database);
}

//  personToMother -- Return the mother of a person. This is the first WIFE in the first FAMC
//    of the person's INDI record.
//--------------------------------------------------------------------------------------------------
GNode* personToMother(GNode* node, Database *database)
{
	return familyToWife(personToFamilyAsChild(node, database), database);
}

//  personToPreviousSibling -- Return previous sibling of a person. This is the previous CHIL in
//    the first FAMC of the person's INDI record.
//--------------------------------------------------------------------------------------------------
GNode* personToPreviousSibling(GNode* indi, Database *database)
{
	ASSERT(indi);
	if (!indi) return null;
	// Get the family the person is a child in.
	GNode* famc = personToFamilyAsChild(indi, database);
	if (!famc) return null;
	//  Find the person as a child in the family. Uses the assumption that all the CHIL nodes
	//   are sequential and in birth order.
	GNode* prev = null;
	GNode* node = CHIL(famc);  // Should be the first CHIL node in the family.
	while (node && eqstr("CHIL", node->tag)) {
		if (eqstr(indi->key, node->value)) {  // Found the person as a child.
			if (!prev) return null;  // There is no previous sibling.
			return keyToPerson(prev->value, database);  // There is a previous sibling.
		}
		prev = node;
		node = node->sibling;  //  Move to the next CHIL node.
	}
	ASSERT(false);  //  The person was not found in the family. Illegal database.
	return null;
}

//  personToNextSibling -- Return the next sibling of a person. This is the next CHIL in the
//    first FAMC of the person's INDI record.
//--------------------------------------------------------------------------------------------------
GNode* personToNextSibling(GNode* indi, Database* database)
{
	ASSERT(indi);
	if (!indi) return null;
	// Get the family the person is a child in.
	GNode* fam = personToFamilyAsChild(indi, database);
	if (!fam) return null;
	//  Find the person as child in the family. Uses the assumption that all CHIL nodes are
	//    sequential and in birth order.
	GNode* node = CHIL(fam);
	while (node && eqstr("CHIL", node->tag)) {
		if (eqstr(indi->key, node->value)) break;
		node = node->sibling;  //  Can be null or the next CHIL node.
	}
	if (!node) return null;
	node = node->sibling;
	if (!node || nestr("CHIL", node->tag)) return null;
	return keyToPerson(node->value, database);
}

//  familyToHusband -- Return the first husband of a family. This is the first HUSB in the
//    FAM record.
//--------------------------------------------------------------------------------------------------
GNode* familyToHusband(GNode* node, Database *database)
{
	if (debugging) {
		printf("familyToHusband called on family %s\n", node ? node->key : "null");
	}
	if (!node) return null;
	if (!(node = findTag(node->child, "HUSB"))) return null;
	if (debugging) {
		printf("familyToHusband found person %s\n", node->value);
	}
	return keyToPerson(node->value, database);
}

//  familyToWife -- Return the first wife of a family. This is the first WIFE in the FAM record.
//--------------------------------------------------------------------------------------------------
GNode *familyToWife(GNode* node, Database *database)
{
	if (debugging) {
		printf("familyToWife called on family %s\n", node ? node->key : "null");
	}
	if (!node) return null;
	if (!(node = findTag(node->child, "WIFE"))) return null;
	if (debugging) {
		printf("familyToWife found person %s\n", node->value);
	}
	return keyToPerson(node->value, database);
}

//  Return the first spouse with a given sex from a family.
//-------------------------------------------------------------------------------------------------
GNode *familyToSpouse(GNode *family, SexType sex, Database* database)
//  family -- Family the spouse is in.
//  person -- Sex of spouse.
{
	//  I beleive all spouses should be restrictied to persons with male or famale sex.
	if (sex != sexMale && sex != sexFemale) return null;
	return (sex == sexMale) ? familyToHusband(family, database) : familyToWife(family, database);
}

//  personToSpouse -- Return the first spouse from the ...
//-------------------------------------------------------------------------------------------------
GNode *personToSpouse(GNode *person, GNode *family)
//  person -- Person to find the spouse of.
//  family -- If present the spouse must be from this family. If null the spouse must be the first
//            person of the opposite sex in the first family the person is a spouse is that has
//            such a spouse.
{
	if (debugging) printf("personToSpouse called on person %s\n", person->key);
	return null;
}

//  familyToFirstChild -- Return the first child of a family. This is the first CHIL in the FAM
//    record. Assumes that the first CHIL in the FAM is the first child in birth order.
//--------------------------------------------------------------------------------------------------
GNode* familyToFirstChild(GNode* node, Database *database)
//  node -- Root of a family Gedcom record.
{
	if (!node) return null;
	if (!(node = CHIL(node))) return null;
	return keyToPerson(node->value, database);
}

//  familyToLastChild -- Return the last child of a family if any.
//--------------------------------------------------------------------------------------------------
GNode* familyToLastChild(GNode* node, Database *database)
//  node -- Root of a family Gedcom record.
{
	if (!node) return null;
	if (!(node = CHIL(node))) return null;
	GNode* chil = null;
	while (node) {
		if (eqstr(node->tag, "CHIL")) chil = node;
		node = node->sibling;
	}
	return keyToPerson(chil->value, database);
}

//  numberOfSpouses -- Returns the number of spouses of a person.
//--------------------------------------------------------------------------------------------------
int numberOfSpouses(GNode* person, Database *database)
{
	if (!person) return 0;
	int nspouses = 0;
	FORSPOUSES(person, spouse, family, count, database)
		if (spouse) nspouses++;
	ENDSPOUSES
	return nspouses;
}

//  numberOfFamilies -- Return the number of families a person is a spouse in.
//--------------------------------------------------------------------------------------------------
int numberOfFamilies(GNode* person)
{
	if (!person) return 0;
	int nfamilies = 0;
	GNode* fams = FAMS(person);
	while (fams && eqstr("FAMS", fams->tag)) {
		nfamilies++;
		fams = fams->sibling;
	}
	return nfamilies;
}

//  personToFamilyAsChild -- Return the family of the first family-as-child of a person. From the
//    value of the first FAMC node in the INDI record. A little poor practice in here when person
//    changes from being a pointer to a person to a pointer to a child node of th person.
//--------------------------------------------------------------------------------------------------
GNode* personToFamilyAsChild(GNode* person, Database* database)
{
	if (!person) return null;
	if (!(person = FAMC(person))) return null;
	return keyToFamily(person->value, database);
}

//  personToName -- Return the name of a person. From the value of the first NAME node in the
//    INDI record.
//--------------------------------------------------------------------------------------------------
String personToName(GNode* person, int length)
//  indi -- The person whose name is sought.
//  len -- Maximum number of characters to use for the name.
{
	if (!person) return "";
	if (!(person = findTag(person->child, "NAME"))) return "";
	return manipulateName(person->value, true, true, length);
}

//  personToTitle -- Return the title of a person. From the value of the first TITL node in the
//    INDI record, if there.
//--------------------------------------------------------------------------------------------------
String personToTitle(GNode* indi, int len)
{
	if (!indi) return null;
	if (!(indi = findTag(indi->child, "TITL"))) return null;
	return indi->value;
}

// fam_to_spouse -- Return other spouse of a family.
//   NOTE: This function is not called and I don't really know what it does.
//--------------------------------------------------------------------------------------------------
GNode* fam_to_spouse(Database* database, GNode* family, GNode* person)
{
	if (!family) return null;
	FORHUSBS(family, husband, database)
		if(husband != person) return(husband);
	ENDHUSBS
	FORWIFES(family, wife, database)
		if(wife != person) return(wife);
	ENDWIFES
	return null;
}

SexType oppositeSex(SexType sex)
{
	if (sex == sexMale) return sexFemale;
	else if (sex == sexFemale) return sexMale;
	else return sexUnknown;
}
