// DeadEnds
//
// gedcom.h is the header file for Gedcom related data types and operations.
//
// Created by Thomas Wetmore on 7 November 2022.
// Last changed on 3 April 2025.

#ifndef gedcom_h
#define gedcom_h

typedef struct GNode GNode; // Forward ref.
#include "standard.h"
#include "gnode.h"
#include "list.h"

// LN returns the line in the original Gedcom file where a GNode came from.
#define LN(root, database, node) rootLine(root, database) + countNodesBefore(node)

// SexType is an enumeration of sex types.
typedef enum SexType {
    sexMale = 1, sexFemale, sexUnknown, sexError
} SexType;

#define personToKey(indi) (indi->key)
#define familyToKey(fam)  (fam->key)

// RecordType is an enumeration of DeadEnds record types.
typedef enum RecordType {
    GRUnknown = 0, GRPerson, GRFamily, GRSource, GREvent, GROther, GRHeader, GRTrailer
} RecordType;

String sexTypeToString(SexType);
SexType sexTypeFromStribg(String);
bool validSexString(String);

RecordType recordType(GNode *root);  // Return the type of a Gedcom record tree.
String keyToKey(String);

int compareRecordKeys(String, String);  // gedcom.c

// FORCHILDREN / ENDCHILDREN is a macro pair that iterates children in a family.
#define FORCHILDREN(fam, childd, key, num, index) \
	{\
	GNode* __node = findTag(fam->child, "CHIL");\
	GNode* childd;\
	int num = 0;\
	String key = null;\
	while (__node) {\
		key = __node->value;\
		childd = keyToPerson(__node->value, index);\
		ASSERT(childd);\
		num++;\
		{

#define ENDCHILDREN \
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "CHIL")) __node = null;\
    }}

// FORFAMCS / ENDFAMCS iterates the FAMC nodes in a person record.
#define FORFAMCS(person, family, key, index)\
{\
	GNode *__node = FAMC(person);\
	GNode *family;\
	String key;\
	while (__node) {\
		key = __node->value;\
		family = keyToFamily(key, index);\
		{

#define ENDFAMCS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "FAMC")) __node = null;\
    }\
}

// FORFAMSS / ENDFAMS iterates the FAMS nodes in a person record.
#define FORFAMSS(person, family, key, index)\
{\
	GNode *__node = FAMS(person);\
	GNode *family;\
	String key;\
	while (__node) {\
		key = __node->value;\
		family = keyToFamily(key, index);\
		{

#define ENDFAMSS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "FAMS")) __node = null;\
    }\
}

// FORTAGVALUES / ENDTAGVALUES iterates a list of nodes looking for a specific tag.
#define FORTAGVALUES(root, tagg, node, value)\
{\
    GNode *node, *__node = root->child;\
    String value, __value;\
    while (__node) {\
        while (__node && strcmp(tagg, __node->tag))\
            __node = __node->sibling;\
        if (__node == null) break;\
        __value = value = full_value(__node);\
        node = __node;\
        {

#define ENDTAGVALUES\
        }\
        if (__value) stdfree(__value);\
        __node = __node->sibling;\
    }\
}

// FORHUSBS / ENDHUSBS iterates over the HUSB nodes in a family.
#define FORHUSBS(fam, husb, key, index)\
{\
	GNode* __node = findTag(fam->child, "HUSB");\
	GNode* husb = null;\
	String key = null;\
	while (__node) {\
		key = __node->value;\
		husb = key ? keyToPerson(key, index) : null;\
		{

#define ENDHUSBS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "HUSB")) __node = null;\
    }\
}

// FORWIFES / ENDWIFES iterates over the WIFE nodes in a family.
#define FORWIFES(fam, wife, key, index)\
{\
	GNode* __node = findTag(fam->child, "WIFE");\
	GNode* wife = null;\
	String key = null;\
	while (__node) {\
		key = __node->value;\
		wife = key ? keyToPerson(key, index) : null;\
		{

#define ENDWIFES\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "WIFE")) __node = null;\
    }\
}

// FORSPOUSES / ENDSPOUSES iterates over a person's spouses.
#define FORSPOUSES(indi, spouse, fam, num, index)\
{\
    GNode* __fnode = FAMS(indi);\
    int __sex = SEXV(indi);\
    GNode* spouse;\
    GNode* fam;\
    int num = 0;\
    while (__fnode) {\
        spouse = null;\
        fam = keyToFamily(__fnode->value, index);\
        if (__sex == sexMale)\
            spouse = familyToWife(fam, index);\
        else\
            spouse = familyToHusband(fam, index);\
        if (spouse != null) {\
            num++;\
            {

#define ENDSPOUSES\
            }\
        }\
        __fnode = __fnode->sibling;\
        if(__fnode && nestr("FAMS", __fnode->tag)) __fnode = null;\
    }\
}

// FORTRAVERSE / ENDTRAVERSE is a macro pair that traverses the GNodes in a tree rooted at root.
#define FORTRAVERSE(root, node)\
{\
    GNode* node = root;\
	int protection = 0;\
    List *stack = createList(null, null, null, false);\
    prependToList(stack, node);\
    while (!isEmptyList(stack)) {\
		protection++;\
		if (protection > 500000) break;\
        node = getAndRemoveFirstListElement(stack);\
        {

#define ENDTRAVERSE\
        }\
        if (node->sibling) prependToList(stack, node->sibling);\
        if (node->child) prependToList(stack, node->child);\
    }\
    deleteList(stack);\
}

//  Macros that return specific GNodes from a record tree.
#define NAME(indi)  findTag((indi)->child,"NAME") // First name of person.
#define SEX(indi)   findTag((indi)->child,"SEX") // First sex of person.
#define SEXV(indi)  valueToSex(findTag((indi)->child,"SEX")) // First sex value of person.
#define BIRT(indi)  findTag((indi)->child,"BIRT") // First birth of person.
#define DEAT(indi)  findTag((indi)->child,"DEAT") // First death of person.
#define BAPT(indi)  findTag((indi)->child,"CHR") // First christening of person.
#define BURI(indi)  findTag((indi)->child,"BURI") // First burial of person.
#define FAMC(indi)  findTag((indi)->child,"FAMC") // First family as child of person.
#define FAMS(indi)  findTag((indi)->child,"FAMS") // First family as spouse of person.
#define HUSB(fam)   findTag((fam)->child,"HUSB") // First husband of family.
#define WIFE(fam)   findTag((fam)->child,"WIFE") // First wife of family.
#define MARR(fam)   findTag((fam)->child,"MARR") // First marriage of family.
#define CHIL(fam)   findTag((fam)->child,"CHIL") // First child of family.
#define DATE(evnt)  findTag((evnt)->child,"DATE") // First date of event.
#define PLAC(evnt)  findTag((evnt)->child,"PLAC") // First place of event.

#endif // gedcom_h
