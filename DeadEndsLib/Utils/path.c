// DeadEnds
//
// path.c has functions to manipulate UNIX file paths.
//
// Created by Thomas Wetmore on 14 December 2022.
// Last changed on 2 September 2025.

#include <unistd.h>
#include "standard.h"

#define MAXPATHBUFFER 4096

// resolveFile tries to find a file within a sequence of paths.
String oldResolveFile(String name, String path, String suffix) {
    if (!name || *name == 0) return null;
    if (!path || *path == 0) return strsave(name);
    if (strchr(name, '/') != null) return strsave(name);
    char buf1[MAXPATHBUFFER];
    strcpy(buf1, path);
    char buf2[MAXPATHBUFFER];
    char* p = strtok(buf1, ":");
    while (p) {
        snprintf(buf2, sizeof(buf2), "%s/%s", p, name);
        if (access(buf2, F_OK) == 0) return strsave(buf2);
        p = strtok(null, ":");
    }
    return null;
}

/// resolveFile tries to find a file within a colon-separated path list.
/// If not found, and a suffix is provided, tries again with the suffix appended.
String resolveFile(String name, String path, String suffix) {

    if (!name || *name == 0) return null;  // No file name.
    if (!path || *path == 0) return strsave(name);  // No path.
    if (strchr(name, '/')) return strsave(name);  // File name is absolute.
    char buf1[MAXPATHBUFFER];
    char fullPath[MAXPATHBUFFER];

    // Search path list for original name
    strcpy(buf1, path);
    for (char* dir = strtok(buf1, ":"); dir; dir = strtok(NULL, ":")) {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, name);
        if (access(fullPath, F_OK) == 0) return strsave(fullPath);
    }

    // Try with suffix if provided
    if (suffix && *suffix != 0) {
        strcpy(buf1, path); // reset buf1 because strtok modifies it
        for (char* dir = strtok(buf1, ":"); dir; dir = strtok(NULL, ":")) {
            snprintf(fullPath, sizeof(fullPath), "%s/%s%s", dir, name, suffix);
            if (access(fullPath, F_OK) == 0) return strsave(fullPath);
        }
    }

    return null;
}

// fopenPath attempta to open a file using a path variable.
FILE *fopenPath(String name, String mode, String path, String suffix) {
    String str;
    if (!(str = resolveFile(name, path, suffix))) return null;
    return fopen(str, mode);
}

// lastPathSegment returns the last componenet of a path. Returns static memory.
String oldLastPathSegment (String path) {
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
String lastPathSegment (String path) {
    char *lastSlash = strrchr(path, '/');
    if (lastSlash)
        return (lastSlash + 1);
    else
        return path;
}
