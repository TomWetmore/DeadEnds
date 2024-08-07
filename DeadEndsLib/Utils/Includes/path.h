// DeadEnds
//
// path.h
//
// Created by Thomas Wetmore on 14 December 2022.
// Last changed 26 July 2024.

#ifndef path_h
#define path_h

#include <stdio.h>
#include "standard.h"

#define MAXPATHLENGTH 1024

FILE *fopenPath(String fileName, String mode, String searchPath);
String resolveFile(String fileName, String searchPath);
String lastPathSegment(String fileName);

#endif // path_h
