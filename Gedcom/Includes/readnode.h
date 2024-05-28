//
// DeadEnds
//
// readnode.h is the header file for functions and variables that read GNodes from Gedcom files
// and strings.
//
// Created by Thomas Wetmore on 17 December 2022.
// Last changed on 27 May 2024.

#ifndef readnode_h
#define readnode_h

#include "standard.h"
#include "gnode.h"
#include "errors.h"

// Return codes from fileToLine and extractFields.
typedef enum ReadReturn {
	ReadAtEnd = 0, // At end of file.
	ReadOkay,    // Last line okay.
	ReadError    // Last line has an error.
} ReadReturn;

ReadReturn fileToLine(FILE*, Error**);
GNode* stringToGNodeTree(String, ErrorLog*);

#endif
