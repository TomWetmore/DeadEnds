//  DeadEnds
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 6 December 2024.

#ifndef validate_h
#define validate_h

#include "database.h"
#include "errors.h"
#include "integertable.h"

typedef enum ValidationCodes {
	VCclosedKeys = 1,
	VClineageLinking = 2,
	VCnamesAndSex = 4,
} ValidationCodes;

extern void validatePersons(RecordIndex*, String name, IntegerTable*, ErrorLog*);
extern void validateFamilies(RecordIndex*, String name, IntegerTable*, ErrorLog*);
extern RefnIndex* getReferenceIndex(RecordIndex*, String name, IntegerTable*, ErrorLog*);
extern int rootLine(GNode*, IntegerTable*);

#endif // validate_h
