//
//  DeadEnds
//
//  path.c - Functions to manipulate UNIX file paths
//
//  Created by Thomas Wetmore on 14 December 2022.
//  Last changed on 26 November 2023.
//

#include <unistd.h>  // access.
#include "standard.h"  // String, strlen, strcpy, null.

#define MAXPATHBUFFER 1024

// filePath -- Find a file in a sequence of paths.
//--------------------------------------------------------------------------------------------------
static String filePath (String fileName, String searchPath)
// fileName -- Name of a file to search for and open.
//  searchPath -- Search path to use in finding the file.
{
	// There must be a file name.
	if (!fileName || *fileName == 0) return null;

	// If there is no path return the name as the only possibility.
	if (!searchPath || *searchPath == 0) return fileName;

	// If the file in a fixed location, return it as the only possibility.
	// TODO: BUG: THE . COULD BE THE FIRST CHARACTER OF AN INVISIBLE FILE.
	if (*fileName == '/' || *fileName == '.') return fileName;

	// If the lengths of the file name and the path are too long return null.
	if (strlen(fileName) + strlen(searchPath) >= MAXPATHBUFFER) return null;

	// Make a copy of the search path in a buffer; changes are made to the copy.
	unsigned char buf1[MAXPATHBUFFER];
	strcpy((String) buf1, searchPath);

	// Convert every every ':' in the buffer to a 0.
	String p = (String) buf1;
	int c;
	while ((c = *p)) {
		if (c == ':') *p = 0;
		p++;
	}
	// Add an extra 0 to the end of the modified path.
	*(++p) = 0;

	// Put p back at the start of the buffer.
	p = (String) buf1;
	unsigned char buf2[MAXPATHBUFFER];  // Where the full file names are built.
	String q;
	while (*p) {
		q = (String) buf2;
		// Copy a path component into the second buffer.
		strcpy(q, p);
		q += strlen(q);
		// Put a / in the second buffer.
		strcpy(q++, "/");
		// Copy the file name after slash.
		strcpy(q, fileName);
		// See if the buffer contains a file in the file system.
		if (access((const char*) buf2, 0) == 0)
			// Yes it does; return first that matches.
			// MNOTE: MEMORY LEAK!!!!!!!
			return strsave((String) buf2);

		// Move to the next path segment and try again.
		p += strlen(p);
		p++;  // Move past the 0 that separates segments.
	}
	return fileName;  // Return the name if the search path didn't help find it.
}

// fopenPath -- Attempt to open a file using a path variable.
//--------------------------------------------------------------------------------------------------
FILE *fopenPath (String name, String mode, String path)
//  name - Name of the file.
//  path - Path variable that can be used to locate the file.
{
	String str;
	if (!(str = filePath(name, path))) return null;
	return fopen(str, mode);
}

//  lastPathSegment -- Return the last componenet of a path.
//    MNOTE: Returns pointer to an internal static buffer. Caller must deal with this.
//--------------------------------------------------------------------------------------------------
String lastPathSegment (String path)
//  path -- Path to find the last component of.
{
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
