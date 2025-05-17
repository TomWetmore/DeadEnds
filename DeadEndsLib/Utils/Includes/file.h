// DeadEnds
//
// file.h
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 17 May 2025.

#ifndef file_h
#define file_h

#include "standard.h"

// File is a structure that holds a file's name and Unix FILE pointer.
typedef struct File {
    FILE* fp;       // Unix file pointer.
    String path;    // Path to file.
    String name;    // Name of file.
    bool isStdout;  // New flag
} File;

File* openFile(String path, String mode);
File* stdOutputFile(void);
void closeFile(File*);

#endif // file_h
