//
//  DeadEnds
//
//  gedcom.h
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changed on 14 November 2023.
//

#ifndef gedcom_h
#define gedcom_h

typedef struct GNode GNode;  //  Forward reference.
#include "standard.h"
#include "gnode.h"
#include "list.h"

//  SexType -- Enumeration for sex types.
//--------------------------------------------------------------------------------------------------
typedef enum SexType {
    sexMale = 1, sexFemale, sexUnknown
} SexType;

#define personToKey(indi) (indi->key)
#define familyToKey(fam)  (fam->key)

//  RecordType -- Enumeration of supported DeadEnds record types.
//--------------------------------------------------------------------------------------------------
typedef enum RecordType {
    GRUnknown = 0, GRPerson, GRFamily, GRSource, GREvent, GROther, GRHeader, GRTrailer
} RecordType;

RecordType recordType(GNode *root);  // Return the type of a Gedcom record tree.

int compareRecordKeys(String, String);  // gedcom.c

// FORCHILDREN / ENDCHILDREN -- Iterator for the children of a family.
//--------------------------------------------------------------------------------------------------
#define FORCHILDREN(fam, childd, num, database) \
    {\
    GNode* __node = findTag(fam->child, "CHIL");\
    GNode* childd;\
    int num = 0;\
    while (__node) {\
        childd = keyToPerson(__node->value, database);\
        ASSERT(childd);\
        num++;\
        {

#define ENDCHILDREN \
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "CHIL")) __node = null;\
    }}

//  FORFAMCS / ENDFAMCS
#define FORFAMILIES(indi, fam, database)\
{\
    GNode *__node = FAMS(indi);\
    GNode *fam;\
    while (__node) {\
        fam = keyToFamily(__node->value, database);\
        ASSERT(fam);\
        {
#define ENDFAMILIES\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "FAMS")) __node = null;\
    }\
}

#define FORFAMCS(person, family, database)\
{\
    GNode *__node = FAMC(person);\
    GNode *family;\
    while (__node) {\
        family = keyToFamily(__node->value, database);\
        ASSERT(family);\
        {

    #define ENDFAMCS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "FAMC")) __node = null;\
    }\
}

#define FORFAMSS(person, family, database)\
{\
    GNode *__node = FAMS(person);\
    GNode *family;\
    while (__node) {\
        family = keyToFamily(__node->value, database);\
        ASSERT(family);\
        {

#define ENDFAMSS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "FAMS")) __node = null;\
    }\
}


//  FORTAGVALUES -- Iterate a list of nodes looking for a particular tag.
//--------------------------------------------------------------------------------------------------
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

//  FORHUSBS -- Iterate over the husbands in one family; handles non-traditional families.
//--------------------------------------------------------------------------------------------------
#define FORHUSBS(fam, husb, database)\
{\
    GNode* __node = findTag(fam->child, "HUSB");\
    GNode* husb=0;\
    String __key=0;\
    while (__node) {\
        __key = __node->value;\
        if (!__key || !(husb = keyToPerson(__key, database))) {\
            __node = __node->sibling;\
            continue;\
        }\
        {

#define ENDHUSBS\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "HUSB")) __node = null;\
    }\
}

//  FORWIFES -- Iterate over the wives in one family; handles non-traditional families.
//--------------------------------------------------------------------------------------------------
#define FORWIFES(fam, wife, database)\
{\
    GNode* __node = findTag(fam->child, "WIFE");\
    GNode* wife = null;\
    String __key = null;\
    while (__node) {\
        __key = __node->value;\
        if (!__key || !(wife = keyToPerson(__key, database))) {\
            __node = __node->sibling;\
            if (__node && nestr(__node->tag, "WIFE")) __node = null;\
                continue;\
        }\
        {
#define ENDWIFES\
        }\
        __node = __node->sibling;\
        if (__node && nestr(__node->tag, "WIFE")) __node = null;\
    }\
}

//  FORSPOUSES -- Iterate over a person's spouses.
//--------------------------------------------------------------------------------------------------
#define FORSPOUSES(indi, spouse, fam, num, database)\
{\
    GNode* __fnode = FAMS(indi);\
    int __sex = SEXV(indi);\
    GNode* spouse;\
    GNode* fam;\
    int num = 0;\
    while (__fnode) {\
        spouse = null;\
        fam = keyToFamily(__fnode->value, database);\
        if (__sex == sexMale)\
            spouse = familyToWife(fam, database);\
        else\
            spouse = familyToHusband(fam, database);\
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

#define FORTRAVERSE(root, node)\
{\
    GNode* node = root;\
    List *stack = createList(null, null, null);\
    prependListElement(stack, node);\
    while (!isEmptyList(stack)) {\
        node = removeFirstListElement(stack);\
        {\

#define ENDTRAVERSE\
        }\
        if (node->sibling) prependListElement(stack, node->sibling);\
        if (node->child) prependListElement(stack, node->child);\
    }\
    deleteList(stack);\
}\

//  Macros that return specific gedcom nodes from a record tree.
//--------------------------------------------------------------------------------------------------
#define NAME(indi)  findTag(indi->child,"NAME")
#define SEX(indi)   findTag(indi->child,"SEX")
#define SEXV(indi)  val_to_sex(findTag(indi->child,"SEX"))
#define BIRT(indi)  findTag(indi->child,"BIRT")
#define DEAT(indi)  findTag(indi->child,"DEAT")
#define BAPT(indi)  findTag(indi->child,"CHR")
#define BURI(indi)  findTag(indi->child,"BURI")
#define FAMC(indi)  findTag(indi->child,"FAMC")
#define FAMS(indi)  findTag(indi->child,"FAMS")
#define HUSB(fam)   findTag(fam->child,"HUSB")
#define WIFE(fam)   findTag(fam->child,"WIFE")
#define MARR(fam)   findTag(fam->child,"MARR")
#define CHIL(fam)   findTag(fam->child,"CHIL")
#define DATE(evnt)  findTag(evnt->child,"DATE")
#define PLAC(evnt)  findTag(evnt->child,"PLAC")

#endif // gedcom_h
