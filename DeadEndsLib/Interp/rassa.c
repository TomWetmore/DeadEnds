//
//  DeadEnds Library
//  rassa.c handles the page mode builtins for DeadEnds script programs.
//
//  Created by Thomas Wetmore on 10 February 2024.
//  Last changed on 23 July 2025.
//

#include <string.h>
#include <stdio.h>
#include "context.h"
#include "evaluate.h"
#include "file.h"
#include "gedcom.h"
#include "hashtable.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "standard.h"

#define MAXROWS 512
#define MAXCOLS 512

extern Page* createPage(int, int);
extern void deletePage(Page*);
static int unicodeToUtf8(int32_t, char*);
static int utf8ToUnicode(const char*, int32_t*);

// __pagemode switches script program output to page mode.
// usage: pagemode(INT, INT) -> VOID
// First parameter is number of rows, second is number of columns.
PValue __pagemode(PNode* pnode, Context* context, bool* errflg) {
	PNode* arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) {
		scriptError(pnode, "the rows argument to pagemode must be an integer.");
		return nullPValue;
	}
	int rows = (int) pvalue.value.uInt;
	arg = arg->next;
	pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) {
		scriptError(pnode, "the columns argument to pagemode must be an integer.");
		return nullPValue;
	}
	int cols = (int) pvalue.value.uInt;
	*errflg = true;
	if (cols < 1 || cols > MAXCOLS || rows < 1 || rows > MAXROWS) {
		scriptError(pnode, "the value of rows or cols to pagemode is out of range.");
		return nullPValue;
	}
	*errflg = false;
    File* file = context->file;
    // If in page mode remove the old Page before creating a new one.
    if (file->mode == pageMode) deletePage(file->page);
    file->mode = pageMode;
    file->page = createPage(rows, cols);
    file->curcol = 1;
    return nullPValue;
}

// __linemode switches script program output to line mode.
// usage: linemode() -> VOID
PValue __linemode(PNode* pnode, Context* context, bool* errflg) {
    File* file = context->file;
    // Do nothing if already in line mode.
    file->curcol = 1;
    if (file->mode == lineMode) return nullPValue;
    // Switch to line mode; assume the last page has been written by a pageout().
	file->mode = lineMode;
    deletePage(file->page);
    return nullPValue;
}

// __newfile switches script program output to a new file.
// usage: newfile() -> VOID
// usage: newfile(STRING[, BOOL]) -> VOID
PValue __newfile(PNode* pnode, Context* context, bool* errflg) {
    PNode* arg = pnode->arguments;
    // Revert to standard output if there are no arguments.
    if (!arg) {
        closeFile(context->file);
        context->file = stdOutputFile();
        return nullPValue;
    }
    // The first argument must be a non-empty string (file name).
    PValue pvalue = evaluate(arg, context, errflg);
    if (*errflg) return nullPValue;
    if (pvalue.type != PVString || !pvalue.value.uString || strlen(pvalue.value.uString) == 0) {
        *errflg = true;
        scriptError(pnode, "first argument to newfile() must be a non-empty string");
        return nullPValue;
    }
    String name = pvalue.value.uString;
    // Default mode is write
    char* mode = "w";
    // The optional second argument can specify append mode.
    arg = arg->next;
    if (arg) {
        pvalue = evaluateBoolean(arg, context, errflg);
        if (*errflg) {
            scriptError(pnode, "second argument to newfile() must be a boolean");
            return nullPValue;
        }
        if (pvalue.value.uBool) mode = "a";
    }
    // Open the new file.
    File* file = openFile(name, mode);
    if (!file) {
        *errflg = true;
        scriptError(pnode, "could not open file: %s", name);
        return nullPValue;
    }
    // Close the previous file and switch.
    closeFile(context->file);
    context->file = file;
    return nullPValue;
}

// __outfile returns the name of the script output file.
// usage: outfile() -> STRING
PValue __outfile(PNode* pnode, Context* context, bool* errflg) {
    return createStringPValue(context->file->name);
}

// __pos positions page output to a row and column.
// usage: pos(INT, INT) -> VOID
PValue __pos(PNode* pnode, Context* context, bool* errflg) {
    // The file must be in page mode.
    File* file = context->file;
    if (file->mode != pageMode) {
        scriptError(pnode, "The output file must in page mode");
        *errflg = true;
        return nullPValue;
    }
    PNode *arg = pnode->arguments;
    // Get the column value.
	int row = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to pos must be an integer");
		return nullPValue;
	}
    // Get the row value.
	int col = evaluateInteger(arg->next, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to pos must be an integer.");
		return nullPValue;
	}
    Page* page = file->page;
	if (row < 1 || row > page->nrows ||	col < 1 || col > page->ncols) {
		scriptError(pnode, "row or col is out of range");
        *errflg = true;
		return nullPValue;
	}
	page->currow = row;
	file->curcol = col;
	return nullPValue;
}

// __row positions output to the start of a row.
// usage: row(INT) -> VOID
PValue __row(PNode* pnode, Context* context, bool* errflg) {
    // The file must be in page mode.
    File* file = context->file;
    if (file->mode != pageMode) {
        scriptError(pnode, "The output file must in page mode");
        *errflg = true;
        return nullPValue;
    }
	int row = evaluateInteger(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "the argument to row must be an integer");
		return nullPValue;
	}
    Page* page = file->page;
	if (row < 1 || row > page->nrows) {
		scriptError(pnode, "the row value is out of range");
        *errflg = true;
		return nullPValue;
	}
    page->currow = row;
    file->curcol = 1;
	return nullPValue;
}

// __col positions the output file to a specific column. The file can be in either line or
// page mode.
// usage: col(INT) -> VOID
PValue __col (PNode *pnode, Context *context, bool *errflg) {
    File* file = context->file;
	int col = evaluateInteger(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "the argument to col must be an integer");
		return nullPValue;
	}
    Page* page = file->page;
    // col is restricted when in page mode.
    if (col < 1 || (file->mode == pageMode && col > page->ncols)) {
        scriptError(pnode, "the column value is out of range");
        *errflg = true;
        return nullPValue;
    }
    // If in line mode may need to write to the file.
    if (file->mode == lineMode) {
        int diff = col - file->curcol;
        if (diff > 0) {
            for (int i = 0; i < diff; i++) putc(' ', file->fp);
        } else if (diff < 0) {
            putc('\n', file->fp);
            for (int i = 0; i < col; i++) putc(' ', file->fp);
        }
    }
    file->curcol = col;
	return nullPValue;
}

// NOTE: The next four functions should be in another file.

// __TYPEOF returns the type of its argument as a string.
// usage: typeof(PNODE) -> STRING
PValue __TYPEOF(PNode* pnode, Context* context, bool *errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    //if (*errflg) return nullPValue;
    return createStringPValue(typeOfPValue(pvalue));
}

/// Returns the value its argument PNode. This includes the PNode's type and value.
// usage: valueof(PNODE) -> STRING
PValue __VALUEOF(PNode* pnode, Context* context, bool *errflg) {
    static char scratch[1000];
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    String type = typeOfPValue(pvalue);
    String value = valueOfPValue(pvalue);
    sprintf(scratch, "%s: %s ", type, value);
    return createStringPValue(scratch);
}

// __SHOWSTACK prints the runtime stack.
// usage: showstack()
extern void showRuntimeStack(Context*, PNode*);
PValue __SHOWSTACK(PNode* pnode, Context* context, bool *errflg) {
    showRuntimeStack(context, pnode);
    return nullPValue;
}

/// Prints the current frame of the run time stack.
// usage: showframe()
extern void showFrame(Frame*);
PValue __SHOWFRAME(PNode* pnode, Context* context, bool* errflg) {
    showFrame(context->frame);
    return nullPValue;
}

/// Writes the page grid of Unicode codepoints to the script output file in UTF-8 format.
// usage: pageout() -> VOID
PValue __pageout(PNode* pnode, Context* context, bool* errflg) {
    // Make sure the output file is in page mode.
    File* file = context->file;
    if (file->mode != pageMode) {
        scriptError(pnode, "the output file must be in page mode");
        *errflg = true;
        return nullPValue;
    }
    Page* page = file->page;
    char utf8[5]; // Buffer for UTF-8 encoding including an unused safety byte.
    // Convert every codepoint to a sequence of UTF-8 bytes and write them to the current output file.
    for (int row = 0; row < page->nrows; row++) {
        for (int col = 0; col < page->ncols; col++) {
            int32_t codepoint = page->grid[row * page->ncols + col];
            int len = unicodeToUtf8(codepoint, utf8); // Convert codepoint to UTF-8.
            if (len > 0) fwrite(utf8, 1, len, file->fp);
        }
        fputc('\n', file->fp);
    }
    // After writing the page fill the grid with spaces.
    for (int i = 0; i < page->nrows * page->ncols; i++) {
        page->grid[i] = 0x20;
    }
    return nullPValue;
}

/// Adjusts the current column after printing a string.
void adjustCols(String string, File* file) {
	int c;
	while ((c = *string++)) {
		if (c == '\n')
			file->curcol = 1;
		else
			file->curcol++;
	}
}

/// Outputs a string to the program output file in the current mode,
// decoding UTF-8 into Unicode codepoints stored in the page grid.
void poutput(String string, Context* context) {
    File* file = context->file;

    // Handle line mode
    if (file->mode == lineMode) {
        fprintf(file->fp, "%s", string);
        adjustCols(string, file);
        return;
    }

    // Guard against non-page mode or null/empty string
    if (file->mode != pageMode || !string || !*string) return;

    Page* page = file->page;
    int row = page->currow;
    int col = file->curcol;

    while (*string) {
        int32_t codepoint = 0;
        int bytes = utf8ToUnicode(string, &codepoint);

        if (bytes == 0) {
            // Invalid UTF-8; skip one byte and continue
            string++;
            continue;
        }
        if (codepoint == '\n') {
            row++;
            col = 1;
            if (row > page->nrows) break; // No room for more lines
        } else {
            if (row >= 1 && row <= page->nrows && col >= 1 && col <= page->ncols) {
                int offset = (row - 1) * page->ncols + (col - 1);
                page->grid[offset] = codepoint;
            }
            col++;
            if (col > page->ncols) {
                col = 1;
                row++;
                if (row > page->nrows) break; // No room for wrapping
            }
        }
        string += bytes;
    }
    page->currow = row;
    file->curcol = col;
}

// unicodeToUtf8 converts a 21-bit Unicode codepoint to one, two, three or four UTF8 bytes.
// Returns the number of bytes written, or 0 on error. Array out must be long enough.
static int unicodeToUtf8(int32_t codepoint, char* out) {
    if (codepoint <= 0x7F) {
        out[0] = (char) codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        out[0] = (char) (0xC0 | ((codepoint >> 6) & 0x1F));
        out[1] = (char) (0x80 | (codepoint & 0x3F));
        return 2;
    } else if (codepoint <= 0xFFFF) {
        out[0] = (char) (0xE0 | ((codepoint >> 12) & 0x0F));
        out[1] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char) (0x80 | (codepoint & 0x3F));
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        out[0] = (char) (0xF0 | ((codepoint >> 18) & 0x07));
        out[1] = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        out[2] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        out[3] = (char) (0x80 | (codepoint & 0x3F));
        return 4;
    } else {
        return 0; // Invalid codepoint.
    }
}

// utf8ToUnicode converts a UTF-8 character to a single Unicode codepoint. Returns the number of
// bytes read, or 0 on error. One to four bytes are read based on the codepoint. The codepoint
// is a 32 bit integer.
static int utf8ToUnicode(const char* s, int32_t* out) {
    unsigned char c = (unsigned char) s[0];
    if (c < 0x80) {
        *out = c;
        return 1;
    } else if ((c & 0xE0) == 0xC0) {
        if ((s[1] & 0xC0) != 0x80) return 0;
        *out = ((c & 0x1F) << 6) | (s[1] & 0x3F);
        return 2;
    } else if ((c & 0xF0) == 0xE0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return 0;
        *out = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        return 3;
    } else if ((c & 0xF8) == 0xF0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return 0;
        *out = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
               ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        return 4;
    }
    return 0;
}
