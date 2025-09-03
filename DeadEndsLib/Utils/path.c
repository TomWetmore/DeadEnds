// DeadEnds
//
// path.c has functions to manipulate UNIX file paths.
//
// Created by Thomas Wetmore on 14 December 2022.
// Last changed on 2 September 2025.

#include <unistd.h>
#include "standard.h"

#define MAXPATHBUFFER 4096

/// resolveFile tries to find a file within a colon-separated path list.
/// If not found, and a suffix is provided, tries again with suffix appended.
String resolveFile(String name, String path, String suffix) {

    if (!name || *name == 0) return null;  // No file name.
    if (!path || *path == 0) return strsave(name);  // No path.
    if (*name == '/') return strsave(name);  // Absolute file name.
    char pathbuf[MAXPATHBUFFER];
    char fullpath[MAXPATHBUFFER];

    // Search path list for the filename.
    strcpy(pathbuf, path);
    for (char* dir = strtok(pathbuf, ":"); dir; dir = strtok(null, ":")) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);
        if (access(fullpath, F_OK) == 0) return strsave(fullpath);
    }
    // Try with suffix if provided.
    if (!suffix || !*suffix) return null;
    strcpy(pathbuf, path);  // Reset buf because strtok modifies it
    String fmt = (*suffix == '.') ? "%s/%s%s" : "%s/%s.%s";
    for (char* dir = strtok(pathbuf, ":"); dir; dir = strtok(null, ":")) {
        snprintf(fullpath, sizeof(fullpath), fmt, dir, name, suffix);
        if (access(fullpath, F_OK) == 0) return strsave(fullpath);
    }
    return null;
}

/// Tries to open a file using a path variable and optional suffix.
FILE *fopenPath(String name, String mode, String path, String suffix) {
    String str;
    if (!(str = resolveFile(name, path, suffix))) return null;
    return fopen(str, mode);
}

/// Returns the last componenet of a path. Returns static memory.
String lastPathSegment(String path) {
    char *lastSlash = strrchr(path, '/');
    if (lastSlash)
        return (lastSlash + 1);
    else
        return path;
}
