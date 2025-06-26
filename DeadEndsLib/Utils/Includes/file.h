//
// DeadEnds
//
// file.h holds the functions for File data types. These are the output files used for script output.
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 26 June 2025.
//

#ifndef file_h
#define file_h

#include "standard.h"

// Page is a page buffer if program output is in PageMode. nrows and ncols are used in page mode.
// currow is used in page mode. curcol is used in both modes. grid is used in page mode. curcol
// has no limit in line mode.
typedef struct Page {
    int nrows;
    int ncols;
    int currow;
    //int curcol;
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
    int curcol;     // Column where next output occurs.
    Page* page;     // Page if in page mode.
} File;

// Public API to File.
File* openFile(String path, String mode);
File* stdOutputFile(void);
void closeFile(File*);

#endif // file_h
