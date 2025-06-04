//
//  DeadEnds Library
//  readnode.h is the header file for functions that read GNodes from Gedcom files and Strings.
//
//  Created by Thomas Wetmore on 17 December 2022.
//  Last changed on 4 June 2025.
//

#ifndef readnode_h
#define readnode_h

#include "standard.h"

typedef struct GNode GNode;

// Return codes from fileToLine and extractFields.
typedef enum ReadReturn {
	ReadAtEnd = 0,
	ReadOkay,
	ReadError
} ReadReturn;

ReadReturn fileToLine(FILE*, int* line, int* lev, String* key, String* tag, String* val, String* err);
ReadReturn stringToLine(String* ps, int* line, int* lev, String* key, String* tag, String *val, String* err);

#endif
