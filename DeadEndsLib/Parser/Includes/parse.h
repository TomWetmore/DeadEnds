//
//  DeadEnds
//
//  parse.h -- Header file for the user interface to parsing.
//
//  Created by Thomas Wetmore on 4 January 23.
//  Last changed on 17 August 2025.
//

#ifndef parse_h
#define parse_h

#include "standard.h"  // String.
typedef struct Program Program;

Program* parseProgram(String, String);  // Parse script program.

#endif // parse_h
