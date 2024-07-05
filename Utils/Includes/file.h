// DeadEnds
//
// file.h
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 1 July 2024.

#ifndef file_h
#define file_h

#include "standard.h"

// File provides a file structure that hold both Unix FILE pointere and the file's name.
typedef struct File {
	FILE* fp;
	String path;
	String name;
} File;

File* createFile(String path, String mode);
void deleteFile(File*);


#endif // file_h
