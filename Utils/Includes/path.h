//
//  path.h
//  ImportGedcom
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed 22 November 2023.
//

#ifndef path_h
#define path_h

#include <stdio.h>
#include "standard.h"

FILE *fopenPath(String fileName, String mode, String searchPath);
String lastPathSegment(String fileName);

#endif /* path_h */
