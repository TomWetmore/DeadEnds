// DeadEnds
//
// file.c
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 1 July 2024.

#include <stdio.h>
#include "file.h"

// createFile creates a File structure.
File* createFile(String path, String mode) {
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

// deleteFile deletes a File structure.
void deleteFile(File* file) {
	if (file->fp) fclose(file->fp);
	if (file->path) stdfree(file->path);
	if (file->name) stdfree(file->name);
}
