//
// DeadEnds
//
// file.h holds the functions for File data types. These are the output files used for script output.
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 19 June 2025.
//

#ifndef file_h
#define file_h

#include "standard.h"

// Page is a page buffer if program output is in PageMode.
typedef struct Page {
    int nrows;
    int ncols;
    int currow;
    int curcol;
    int32_t* grid; // size = nrows * ncols, contains Unicode codepoints.
} Page;

// FileType is an enum that defines the types of Files, now restricted to lineMode and pageMode.
typedef enum FileMode {
    lineMode = 0,
    pageMode,
} FileMode;

// File is a structure that holds a file's name and Unix FILE pointer.
typedef struct File {
    FILE* fp;       // Unix file pointer.
    String path;    // Path to file.
    String name;    // Name of file.
    bool isStdout;  // True if file is standard out.
    FileMode mode;  // Mode of file -- line mode or page mode.
    Page* page;     // Page if in page mode.
} File;

// Public API to File.
File* openFile(String path, String mode);
File* stdOutputFile(void);
void closeFile(File*);

#endif // file_h
