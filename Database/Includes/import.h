//
//  DeadEnds
//
//  import.h -- Header file for the Gedcom import process.
//
//  Created by Thomas Wetmore on 13 November 2022.
//  Last changed on 11 November 2023.
//

#ifndef import_h
#define import_h

#include <stdio.h>
#include "recordindex.h"
#include "errors.h"
#include "database.h"
#include "list.h"

List *importFromFiles(String fileNames[], int count, ErrorLog*);

#endif // import_h
