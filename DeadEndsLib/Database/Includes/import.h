//
//  DeadEnds Library
//
//  import.h has the declarations of the high level import function.
//  TODO: The string functions aren't here yet.
//
//  Created by Thomas Wetmore on 13 November 2022.
//  Last changed on 3 June 2025.
//

#ifndef import_h
#define import_h

#import "standard.h"

typedef struct Database Database;
typedef struct List List;
typedef struct HashTable HashTable;

typedef HashTable IntegerTable;
typedef List RootList;
typedef List GNodeList;
typedef List ErrorLog;

List *getDatabasesFromFiles(List*, int vcodes, ErrorLog*);
Database* getDatabaseFromFile(String, int vcodes, ErrorLog*);
RootList* getRecordListFromFile(String, IntegerTable*, ErrorLog*);
void checkKeysAndReferences(GNodeList*, String name, IntegerTable*, ErrorLog*);

#endif // import_h
