//  DeadEnds
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 26 November 2024.

#ifndef validate_h
#define validate_h

#include "database.h"
#include "errors.h"
#include "integertable.h"

extern void validatePersons(Database*, IntegerTable*, ErrorLog*);
extern void validateFamilies(Database*, IntegerTable*, ErrorLog*);
extern void validateReferences(Database*, IntegerTable*, ErrorLog*);
extern int rootLine(GNode*, IntegerTable*);

#endif // validate_h
