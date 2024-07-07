//  DeadEnds
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 5 July 2024.

#ifndef validate_h
#define validate_h

#include "database.h"
#include "errors.h"

extern void validatePersons(Database*, ErrorLog*);
extern void validateFamilies(Database*, ErrorLog*);
extern void validateReferences(Database*, ErrorLog*);

//extern bool validateDatabase(Database*, ErrorLog*);
//extern bool validateSourceIndex(Database*, ErrorLog*);
//extern bool validateEventIndex(Database*, ErrorLog*);
//extern bool validateOtherIndex(Database*, ErrorLog*);
extern int rootLine(GNode*, Database*);

#endif // validate_h
