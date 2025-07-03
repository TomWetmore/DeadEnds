//
// DeadEnds
//
// file.c
//
// Created by Thomas Wetmore on 1 July 2024.
// Last changed on 1 July 2025.
//

#include <stdio.h>
#include "file.h"

extern void deletePage(Page*);

// openFile creates a File structure and opens the underlying UNIX file. When a File is opened it is given
// line mode by default.
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
    file->mode = lineMode;
    file->curcol = 1;
    file->page = null;
	return file;
}

// closeFile closes the UNIX file and deletes the File structure.
void closeFile(File* file) {
    if (!file->isStdout && file->fp) fclose(file->fp);
    if (file->path) stdfree(file->path);
    if (file->name) stdfree(file->name);
    if (file->mode == pageMode) deletePage(file->page);
    stdfree(file);
}

// stdOutputFile returns a File structure for UNIX stdout.
File* stdOutputFile(void) {
    File* file = (File*) stdalloc(sizeof(File));
    file->path = strsave(".");
    file->name = strsave("stdout");
    file->fp = stdout;
    file->isStdout = true;
    file->mode = lineMode;
    file->page = null;
    return file;
}

// createPage creates a new page with a grid of rows * cols Unicode code points.
Page* createPage(int rows, int cols) {
    Page* page = (Page*) stdalloc(sizeof(Page));
    page->nrows = rows;
    page->ncols = cols;
    page->currow = 1;
    page->grid = (int32_t*) stdalloc(rows * cols * sizeof(int32_t));
    for (int i = 0; i < page->nrows * page->ncols; i++) {
        page->grid[i] = 0x20; // U+0020 = space
    }
    return page;
}

// deletePage deleges a Page. Not in the public API.
void deletePage(Page* page) {
    if (page && page->grid) stdfree(page->grid);
    if (page) stdfree(page);
}
