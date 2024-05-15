//  DeadEnds
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 14 May 2024.

#ifndef validate_h
#define validate_h

#include "database.h"
#include "errors.h"

extern bool validateDatabase(Database*, ErrorLog*);
extern bool validatePersonIndex(Database*, ErrorLog*);
extern bool validateFamilyIndex(Database*, ErrorLog*);
extern bool validateSourceIndex(Database*, ErrorLog*);
extern bool validateEventIndex(Database*, ErrorLog*);
extern bool validateOtherIndex(Database*, ErrorLog*);
extern int personLineNumber(GNode*, Database*);
extern int familyLineNumber(GNode*, Database*);

#endif // validate_h
