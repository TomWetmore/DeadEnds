// DeadEnds
//
// path.c has functions to manipulate UNIX file paths.
//
// Created by Thomas Wetmore on 14 December 2022.
// Last changed on 26 July                 2024.

#include <unistd.h>
#include "standard.h"

#define MAXPATHBUFFER 1024

// filePath finds a file in a sequence of paths.
String resolveFile(String name, String path) {
	if (!name || *name == 0) return null;
	if (!path || *path == 0) return name;
	if (*name == '/' || *name == '.') return name; // Bug: . could be part of name.
	if (strlen(name) + strlen(path) >= MAXPATHBUFFER) return null;
	unsigned char buf1[MAXPATHBUFFER];
	strcpy((String) buf1, path);
	String p = (String) buf1; // Convert :'s to 0's.
	int c;
	while ((c = *p)) {
		if (c == ':') *p = 0;
		p++;
	}
	*(++p) = 0; // Extra 0.
	p = (String) buf1;
	unsigned char buf2[MAXPATHBUFFER]; // Full file names.
	String q;
	while (*p) {
		q = (String) buf2;
		strcpy(q, p);
		q += strlen(q);
		strcpy(q++, "/");
		strcpy(q, name);
		if (access((const char*) buf2, 0) == 0)
			return strsave((String) buf2); // Memory leak.
		p += strlen(p);
		p++;
	}
	return name;
}

// fopenPath attempta to open a file using a path variable.
FILE *fopenPath(String name, String mode, String path) {
	String str;
	if (!(str = resolveFile(name, path))) return null;
	return fopen(str, mode);
}

// lastPathSegment returns the last componenet of a path. Returns static memory.
String lastPathSegment (String path) {
	static unsigned char scratch[MAXPATHBUFFER];
	if (!path || *path == 0) return NULL;
	int len = (int) strlen(path);
	String p = (String) scratch, q;
	strcpy(p, path);
	if (p[len-1] == '/') {
		len--;
		p[len] = 0;
	}
	q = p;
	int c;
	while ((c = *p++)) {
		if (c == '/') q = p;
	}
	return q;
}
