// DeadEnds
//
// file.c
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 17 May 2025.

#include <stdio.h>
#include "file.h"

// openFile creates a File structure and opens the underlying UNIX file.
File* openFile(String path, String mode) {
    FILE* fp = fopen(path, mode);
    if (!fp) return null;
    String name = lastPathSegment(path);
    if (!name) {
        fclose(fp);
        return null;
    }
	File* file = (File*) stdalloc(sizeof(File));
	file->path = strsave(path);
	file->name = strsave(name);
    file->fp = fp;
    file->isStdout = false;
	return file;
}

// closeFile closes the UNIX file and deletes the File structure.
void closeFile(File* file) {
    if (!file->isStdout && file->fp) fclose(file->fp);
    if (file->path) stdfree(file->path);
    if (file->name) stdfree(file->name);
    stdfree(file);
}

// stdOutputFile returns a File structure for UNIX stdout.
File* stdOutputFile(void) {
    File* file = (File*) stdalloc(sizeof(File));
    file->path = strsave(".");
    file->name = strsave("stdout");
    file->fp = stdout;
    file->isStdout = true;
    return file;
}
