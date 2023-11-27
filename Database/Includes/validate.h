//
//  DeadEnds
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 5 November 2023.
//

#ifndef validate_h
#define validate_h

#include "database.h"
#include "errors.h"

extern bool validateDatabase(Database*, ErrorLog*);

#endif // validate_h
