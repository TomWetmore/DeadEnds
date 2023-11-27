//
//  DeadEnds
//
//  lineage.h -- Header file for operations on Gedcom nodes based on genealogical relationsips
//    and properties
//
//  Created by Thomas Wetmore on 17 February 2023.
//  Last changed on 14 November 2023.
//

#ifndef lineage_h
#define lineage_h

#include "gnode.h"

GNode* personToFather(GNode*, Database*);  //  Return first father of a person.
GNode* personToMother(GNode*, Database*);  //  Return first wife of a person.
GNode* personToPreviousSibling(GNode*, Database*);  //  Return the previous sibling of a person.
GNode* personToNextSibling(GNode*, Database*);  //  Return the next sibling of a person.
GNode* familyToHusband(GNode*, Database*);  //  Return the first husband of a family.
GNode* familyToWife(GNode*, Database*);  //  Return the first wife of a family.
GNode *familyToSpouse(GNode*, SexType, Database*);  // Return first spouse of given sex.
GNode* familyToFirstChild(GNode*, Database*);  //  Return the first child of a family.
GNode* familyToLastChild(GNode*, Database*);  //  Return the last child of a family.
String personToName(GNode*, int);  //  Return the first name of a person.
String personToTitle(GNode*, int);  //  Return the first title of a person.
int numberOfSpouses(GNode*, Database*);  //  Return the number of spouses of a person.
SexType oppositeSex(SexType);  // Return the opposite sex of a person.


GNode* fam_to_spouse(Database*, GNode*, GNode*);

#endif // lineage_h
