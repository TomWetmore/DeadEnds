//
//  DeadEnds Library
//
//  validate.h
//
//  Created by Thomas Wetmore on 12 April 2023.
//  Last changed on 4 June 2025.
//

#ifndef validate_h
#define validate_h

typedef struct GNode GNode;
typedef struct HashTable HashTable;
typedef HashTable IntegerTable;
typedef HashTable RecordIndex;
typedef HashTable RefnIndex;

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
