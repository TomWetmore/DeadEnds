// DeadEnds
//
// file.c
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 2 April 2025.

#include <stdio.h>
#include "file.h"

// openFile creates a File structure.
File* openFile(String path, String mode) {
	if (!path || !mode) return null;
	FILE* fp = fopen(path, mode);
	String name = lastPathSegment(path);
	if (!fp || !name) return null;
	File* file = (File*) stdalloc(sizeof(File));
	file->path = strsave(path);
	file->name = strsave(name);
	file->fp = fp;
	return file;
}

// closeFile deletes a File structure.
void closeFile(File* file) {
	if (file->fp) fclose(file->fp);
	if (file->path) stdfree(file->path);
	//if (file->name) stdfree(file->name); // TODO: Find a better fix some day.
	stdfree(file);
}
