//
//  DeadEnds Library
//  rassa.c handles printing output from DeadEnds script programs.
//
//  Created by Thomas Wetmore on 10 February 2024.
//  Last changed on 17 June 2025.
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
static char linebuffer[1024];
static int linebuflen = 0;
static String bufptr = linebuffer;
String outfilename;

String noreport = (String) "No report was generated.";
String whtout = (String) "What is the name of the output file?";

// initrassa initializes script program output.
void initrassa(void) {
}

// finishrassa finalizes script program output.
//void finishrassa(void) {
//	if (outputmode == BUFFERED && linebuflen > 0 && Poutfp) {
//		fwrite(linebuffer, linebuflen, 1, Poutfp);
//		linebuflen = 0;
//		bufptr = linebuffer;
//		curcol = 1;
//	}
//}

// __pagemode switches script program output to page mode.
// usage: pagemode(INT, INT) -> VOID
PValue __pagemode(PNode* pnode, Context* context, bool* errflg) {
	PNode* arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) {
		scriptError(pnode, "the cols argument to pagemode must be an integer.");
		return nullPValue;
	}
	int cols = (int) pvalue.value.uInt;
	arg = arg->next;
	pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) {
		scriptError(pnode, "the rows argument to pagemode must be an integer.");
		return nullPValue;
	}
	int rows = (int) pvalue.value.uInt;
	*errflg = true;
	if (cols < 1 || cols > MAXCOLS || rows < 1 || rows > MAXROWS) {
		scriptError(pnode, "the value of rows or cols to pagemode is out of range.");
		return nullPValue;
	}
	*errflg = false;
    File* file = context->file;
    // Do nothing if already in page mode.
    if (file->mode == pageMode) return nullPValue;
    // Switch to page mode.
    file->mode = pageMode;
    file->page = stdalloc(rows*cols);
    memset(file->page, ' ', rows*cols);
    file->page->nrows = rows;
    file->page->ncols = cols;
    file->page->curcol = file->page->currow = 0;
	return nullPValue;
}

// __linemode switches script program output to line mode.
// usage: linemode() -> VOID
PValue __linemode(PNode* pnode, Context* context, bool* errflg) {
    File* file = context->file;
    // Do nothing if already in line mode.
    if (file->mode == lineMode) return nullPValue;
    // Switch to line mode; assume the last page has been written by a pageout().
	file->mode = lineMode;
    stdfree(file->page->buffer);
    stdfree(file->page);
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
	int col = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to pos must be an integer");
		return nullPValue;
	}
    // Get the row value.
	int row = evaluateInteger(arg->next, context, errflg);
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
	page->curcol = col;
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
    page->curcol = 1;
	return nullPValue;
}

// __col positions page output to a specific column.
// usage: col(INT) -> VOID
PValue __col (PNode *pnode, Context *context, bool *errflg) {
    // The file must be in page mode.
    File* file = context->file;
    if (file->mode != pageMode) {
        scriptError(pnode, "The output file must in page mode");
        *errflg = true;
        return nullPValue;
    }
	int col = evaluateInteger(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "the argument to col must be an integer");
		return nullPValue;
	}
    Page* page = file->page;
    if (col < 1 || col > page->ncols) {
        scriptError(pnode, "the column value is out of range");
        *errflg = true;
        return nullPValue;
    }
    page->curcol = col;
	return nullPValue;
}

// __TYPEOF returns the type of its argument as a string.
PValue __TYPEOF(PNode* pnode, Context* context, bool *errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    if (*errflg) return nullPValue;
    return createStringPValue(typeOf(pvalue));
}

// __SHOWSTACK prints the runtime stack.
// usage: showstack()
extern void showRuntimeStack(Context*, PNode*);
PValue __SHOWSTACK(PNode* pnode, Context* context, bool *errflg) {
    showRuntimeStack(context, pnode);
    return nullPValue;
}

// __SHOWFRAME prints the current frame.
// usage: showframe()
extern void showFrame(Frame*);
PValue __SHOWFRAME(PNode* pnode, Context* context, bool* errflg) {
    showFrame(context->frame);
    return nullPValue;
}

// __pageout outputs the current page and clears the page buffer.
// usage: pageout() -> VOID
PValue __pageout(PNode* pnode, Context* context, bool* errflg) {
    // The file must be in page mode.
    File* file = context->file;
    if (file->mode != pageMode) {
        scriptError(pnode, "The output file must in page mode");
        *errflg = true;
        return nullPValue;
    }
    Page* page = file->page;
    char scratch[page->ncols + 2];
    scratch[page->ncols] = '\n';
    scratch[page->ncols + 1] = 0;
    String p = page->buffer;
    for (int row = 1; row < page->nrows; row++) {
        memcpy(scratch, p, page->ncols);
        fputs(scratch, file->fp);
        p += page->ncols;
    }
    memset(page->buffer, ' ', page->nrows*page->ncols);
    return nullPValue;
}

// adjustCols adjusts the column after printing a string.
// NOTE: This looks buggy. NOTE: This looks buggy.
void adjustCols(String string, Page* page) {
	int c;
	while ((c = *string++)) {
		if (c == '\n')
			page->curcol = 1;
		else
			page->curcol++;
	}
}

// poutput outputs a string to the current program output file in the current mode.
void poutput(String string, Context* context) {
    File* file = context->file;
    // Handle line mode.
    if (file->mode == lineMode) {
        fprintf(file->fp, "%s", string);
        return;
    }

    // Handle page mode.
    if (file->mode != pageMode) FATAL();
    int len = 0;
    if (!string || *string == 0 || (len = (int) strlen(string)) <= 0) return;
    Page* page = file->page;
    String buffer = page->buffer;
    String p = buffer + (page->currow - 1)*page->ncols + page->curcol - 1;
    int c;
    while ((c = *string++)) {
        if (c == '\n') {
            page->curcol = 1;
            page->currow++;
            p = buffer + (page->currow)*page->ncols;
        } else {
            if (page->curcol <= page->ncols && page->currow <= page->nrows) {
                *p++ = c;
                page->curcol++;
            }
        }
    }
}

