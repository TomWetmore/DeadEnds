// DeadEnds
//
// lineage.h is the header file for operations on GNodes based on genealogical relationsips
// and properties
//
// Created by Thomas Wetmore on 17 February 2023.
// Last changed on 30 November 2024.

#ifndef lineage_h
#define lineage_h

#include "gnode.h"

GNode* personToFather(GNode*, RecordIndex*); // Return first father of a person.
GNode* personToMother(GNode*, RecordIndex*); // Return first wife of a person.
GNode* personToPreviousSibling(GNode*, RecordIndex*); // Return previous sibling of a person.
GNode* personToNextSibling(GNode*, RecordIndex*); // Return next sibling of a person.
GNode* familyToHusband(GNode*, RecordIndex*); // Return first husband of a family.
GNode* familyToWife(GNode*, RecordIndex*); // Return first wife of a family.
GNode *familyToSpouse(GNode*, SexType, RecordIndex* index); // Return first spouse of given sex.
GNode* familyToFirstChild(GNode*, RecordIndex*); // Return first child of family.
GNode* familyToLastChild(GNode*, RecordIndex*); // Return the last child of family.
String personToName(GNode*, int); // Return the first name of a person.
String personToTitle(GNode*, int); // Return the first title of a person.
int numberOfSpouses(GNode*, Database*); // Return the number of spouses of a person.
int numberOfFamilies(GNode*); // Return the number of families a person is a spouse in.
SexType oppositeSex(SexType); // Return the opposite sex of a person.

#endif // lineage_h
