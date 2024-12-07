// DeadEnds
//
// import.h has the declarations of the high level import function.
// TODO: The string functions aren't here yet.
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 6 December 2024.

#ifndef import_h
#define import_h

#include <stdio.h>
#include "recordindex.h"
#include "errors.h"
#include "database.h"
#include "gnodelist.h"
#include "stringset.h"
#include "integertable.h"

List *getDatabasesFromFiles(List*, int vcodes, ErrorLog*);
Database* getDatabaseFromFile(String, int vcodes, ErrorLog*);
RecordIndex* getRecordIndexFromFile(String, RootList*, RootList*, IntegerTable*, ErrorLog*);
void checkKeysAndReferences(GNodeList*, String name, IntegerTable*, ErrorLog*);

#endif // import_h
