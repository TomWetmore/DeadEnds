//
//  DeadEnds
//
//  parse.h -- Header file for the user interface to parsing.
//
//  Created by Thomas Wetmore on 4 January 23.
//  Last changed on 31 May 2025.
//

#ifndef parse_h
#define parse_h

#include "standard.h"  // String.
typedef struct Context Context;

Context* parseProgram(String fileName, String searchPath);  // Parse fileName found in searchPath.

#endif // parse_h
