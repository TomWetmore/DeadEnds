// DeadEnds
//
// rassa.c handles printing output from DeadEnds script programs.
//
// Created by Thomas Wetmore on 10 February 2024.
// Last changed on 17 May 2025.

#include <string.h>
#include <stdio.h>
#include "standard.h"
#include "hashtable.h"
#include "gedcom.h"
#include "interp.h"
#include "evaluate.h"

#define MAXROWS 512
#define MAXCOLS 512
#define MAXPAGESIZE (MAXROWS*MAXCOLS)
int __cols = 0, __rows = 0;
int curcol = 1, currow = 1;
int outputmode = BUFFERED;
static String pagebuffer = null; // buffer for PAGEMODE.
static char linebuffer[1024];
static int linebuflen = 0;
static String bufptr = linebuffer;
String outfilename;

String noreport = (String) "No report was generated.";
String whtout = (String) "What is the name of the output file?";

// initrassa initializes script program output.
void initrassa(void) {
	outputmode = BUFFERED;
	linebuflen = 0;
	bufptr = linebuffer;
	curcol = 1;
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
	outputmode = PAGEMODE;
	__rows = rows;
	__cols = cols;
	if (pagebuffer) stdfree(pagebuffer);
	pagebuffer = (String) stdalloc(__rows*__cols);
	memset(pagebuffer, ' ', __rows*__cols);
	return nullPValue;
}

// __linemode switches script program output to line mode.
// usage: linemode() -> VOID
PValue __linemode(PNode* pnode, Context* context, bool* errflg) {
	outputmode = BUFFERED;
	linebuflen = 0;
	bufptr = linebuffer;
	curcol = 1;
	*errflg = false;
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
	PNode *arg = pnode->arguments;
	int col = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to pos must be an integer");
		return nullPValue;
	}
	int row = evaluateInteger(arg->next, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to pos must be an integer.");
		return nullPValue;
	}
	*errflg = true;
	if (outputmode != PAGEMODE || row < 1 || row > __rows ||
		col < 1 || col > __cols) {
		scriptError(pnode, "There is an error in the page mode, row or col values.");
		return nullPValue;
	}
	*errflg = false;
	currow = row;
	curcol = col;
	return nullPValue;
}

// __row positions output to the start of a row.
// usage: row(INT) -> VOID
PValue __row(PNode* pnode, Context* context, bool* errflg) {
	int row = evaluateInteger(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The argument to row must be an integer.");
		return nullPValue;
	}
	*errflg = true;
	if (outputmode != PAGEMODE || row < 1 || row > __rows) {
		scriptError(pnode, "There is an error in the output mode or row value.");
		return nullPValue;
	};
	*errflg = false;
	currow = row;
	curcol = 1;
	return nullPValue;
}

// __col positions page output to specific column.
// usage: col(INT) -> VOID
PValue __col (PNode *pnode, Context *context, bool *errflg)
{
	int col = evaluateInteger(pnode->arguments, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The argument to col must be an integer.");
		return nullPValue;
	}
	if (col < 1) col = 1;
	if (col > 100) col = 100;
	curcol = col;
	return nullPValue;
}

// __TYPEOF returns the type of its argument as a string.
PValue __TYPEOF(PNode* pnode, Context* context, bool *errflg) {
    PValue pvalue = evaluate(pnode->arguments, context, errflg);
    if (*errflg) return nullPValue;
    String typename;
    switch (pvalue.type) {
        case PVNull:     typename = "null"; break;
        case PVInt:      typename = "int"; break;
        case PVFloat:    typename = "float"; break;
        case PVBool:     typename = "bool"; break;
        case PVString:   typename = "string"; break;
        case PVGNode:    typename = "gnode"; break;
        case PVPerson:   typename = "person"; break;
        case PVFamily:   typename = "family"; break;
        case PVSource:   typename = "source"; break;
        case PVEvent:    typename = "event"; break;
        case PVOther:    typename = "other"; break;
        case PVList:     typename = "list"; break;
        case PVTable:    typename = "table"; break;
        case PVSequence: typename = "sequence"; break;
        default:         typename = "unknown"; break;
    }
    return createStringPValue(typename);
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
//PValue __pageout(PNode* pnode, Context* context, bool* errflg) {
//	char scratch[MAXCOLS+2];
//	String p;
//	int row, i;
//	*errflg = true;
//	if (outputmode != PAGEMODE) return nullPValue;
////	if (!Poutfp) {
////		Poutfp = ask_for_file("w", whtout, &outfilename, llreports);
////		if (!Poutfp)  {
////			message(noreport);
////			return;
////		}
////		setbuf(Poutfp, NULL);
////	}
//	*errflg = false;
//	scratch[__cols] = '\n';
//	scratch[__cols+1] = 0;
//	p = pagebuffer;
//	for (row = 1; row <= __rows; row++) {
//		memcpy(scratch, p, __cols);
//		for (i = __cols - 1; i > 0 && scratch[i] == ' '; i--)
//			;
//		scratch[i+1] = '\n';
//		scratch[i+2] = 0;
//		fputs(scratch, Poutfp);
//		p += __cols;
//	}
//	memset(pagebuffer, ' ', __rows*__cols);
//	return nullPValue;
//}

// adjustCols adjusts the column after printing a string
void adjustCols(String string) {
	int c;
	while ((c = *string++)) {
		if (c == '\n')
			curcol = 1;
		else
			curcol++;
	}
}

// poutput outputs a string in the current mode.
//void poutput(String string) {
//	String p;
//	int c, len;
//	if (!string || *string == 0 || (len = (int) strlen(string)) <= 0) return;
////	if (!Poutfp) {
////		Poutfp = ask_for_file("w", whtout, &name, llreports);
////		if (!Poutfp)  {
////			message(noreport);
////			return;
////		}
////		setbuf(Poutfp, NULL);
////		outfilename = strsave(name);
////	}
//	switch (outputmode) {
//	case UNBUFFERED:
//		fwrite(string, len, 1, Poutfp);
//		adjustCols(string);
//		return;
//	case BUFFERED:
//		if (len > 1024) {
//			fwrite(linebuffer, linebuflen, 1, Poutfp);
//			fwrite(string, len, 1, Poutfp);
//			linebuflen = 0;
//			bufptr = linebuffer;
//			adjustCols(string);
//			return;
//		}
//		if (len + linebuflen > 1024) {
//			fwrite(linebuffer, linebuflen, 1, Poutfp);
//			linebuflen = 0;
//			bufptr = linebuffer;
//		}
//		linebuflen += len;
//		while ((c = *bufptr++ = *string++)) {
//			if (c == '\n')
//				curcol = 1;
//			else
//				curcol++;
//		}
//		--bufptr;
//		return;
//	case PAGEMODE:
//		p = pagebuffer + (currow - 1)*__cols + curcol - 1;
//		while ((c = *string++)) {
//			if (c == '\n') {
//				curcol = 1;
//				currow++;
//				p = pagebuffer + (currow - 1)*__cols;
//			} else {
//				if (curcol <= __cols && currow <= __rows)
//					*p++ = c;
//				curcol++;
//			}
//		}
//		return;
//	default:
//		FATAL();
//	}
//}

