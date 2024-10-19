// DeadEnds
//
// rassa.c -- Handle output to the product file

// Created on 10 February 2024.
// Last changed on 18 October 2024.

#include <string.h>
#include <stdio.h>
#include "standard.h"
#include "hashtable.h"
#include "gedcom.h"
#include "interp.h"
#include "evaluate.h"

#define MAXPAGESIZE 65536
#define MAXROWS 512
#define MAXCOLS 512
int __cols = 0, __rows = 0;
int curcol = 1, currow = 1;
int outputmode = BUFFERED;
static String pagebuffer = null;
static char linebuffer[1024];
static int linebuflen = 0;
static String bufptr = linebuffer;
String outfilename;

String noreport = (String) "No report was generated.";
String whtout = (String) "What is the name of the output file?";

/*=======================================
 * initrassa -- Initialize program output
 *=====================================*/
void initrassa (void)
{
	outputmode = BUFFERED;
	linebuflen = 0;
	bufptr = linebuffer;
	curcol = 1;
}
/*=======================================
 * finishrassa -- Finalize program output
 *=====================================*/
void finishrassa (void)
{
	if (outputmode == BUFFERED && linebuflen > 0 && Poutfp) {
		fwrite(linebuffer, linebuflen, 1, Poutfp);
		linebuflen = 0;
		bufptr = linebuffer;
		curcol = 1;
	}
}

// __pagemode switches output to page mode.
// usage: pagemode(INT, INT) -> VOID
PValue __pagemode(PNode* pnode, Context* context, bool* errflg)
{
	PNode* arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) return nullPValue;
	int cols = (int) pvalue.value.uInt;
	arg = arg->next;
	pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVInt) return nullPValue;
	int rows = (int) pvalue.value.uInt;
	*errflg = true;
	if (cols < 1 || cols > MAXCOLS || rows < 1 || rows > MAXROWS)
		return nullPValue;
	*errflg = false;
	outputmode = PAGEMODE;
	__rows = rows;
	__cols = cols;
	if (pagebuffer) stdfree(pagebuffer);
	pagebuffer = (String) stdalloc(__rows*__cols);
	memset(pagebuffer, ' ', __rows*__cols);
	return nullPValue;
}

// __linemode switches output to line mode.
// usage: linemode() -> VOID
PValue __linemode(PNode* pnode, Context* context, bool* errflg) {
	outputmode = BUFFERED;
	linebuflen = 0;
	bufptr = linebuffer;
	curcol = 1;
	*errflg = false;
	return nullPValue;
}

//  __newfile -- Switch output to new file
//    usage: newfile(STRING, BOOL) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __newfile (PNode *pnode, Context *context, bool *errflg)
{
	PNode *arg = pnode->arguments;
	PValue pvalue = evaluate(arg, context, errflg);
	if (*errflg || pvalue.type != PVString || strlen(pvalue.value.uString) == 0) {
		scriptError(pnode, "First argument to newfile must be a string.");
		return nullPValue;
	}
	String name = pvalue.value.uString;
	arg = arg->next;
	pvalue = evaluateBoolean(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "Second argument to newfile must be a boolean.");
		return nullPValue;
	}
	bool aflag = pvalue.value.uBool;
	if (Poutfp) {
		finishrassa();
		fclose(Poutfp);
		Poutfp = null;
	}
	outfilename = strsave(name);
	if (!(Poutfp = fopenPath(name, aflag?"a":"w", "." /*llprograms*/))) {
		scriptError(pnode, "Could not open file %s", name);
		return nullPValue;
	}
	return nullPValue;
}

//  __outfile -- Return output file name
//    usage: outfile() -> STRING
//--------------------------------------------------------------------------------------------------
//PValue __outfile (PNode *pnode, Context *context, bool *errflg)
//{
//	if (!Poutfp) {
//		Poutfp = ask_for_file("w", whtout, &outfilename, llreports);
//		if (!Poutfp)  {
//			message(noreport);
//			return;
//		}
//		setbuf(Poutfp, NULL);
//	}
//	*eflg = false;
//	return (WORD) outfilename;
//}

//  __pos -- Position page output to row and column
//    usage: pos(INT, INT) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __pos (PNode *pnode, Context *context, bool *errflg)
{
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

//  __row -- Position output to start of row
//    usage: row(INT) -> VOID
//--------------------------------------------------------------------------------------------------
PValue __row (PNode *pnode, Context *context, bool *errflg)
{
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

//  __col -- Position output to column
//    usage: col(INT) -> VOID
//--------------------------------------------------------------------------------------------------
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

//  __pageout -- Output current page and clear page buffer
//    usage: pageout() -> VOID
//--------------------------------------------------------------------------------------------------
PValue __pageout (PNode *pnode, Context *context, bool *errflg)
{
	char scratch[MAXCOLS+2];
	String p;
	int row, i;
	*errflg = true;
	if (outputmode != PAGEMODE) return nullPValue;
//	if (!Poutfp) {
//		Poutfp = ask_for_file("w", whtout, &outfilename, llreports);
//		if (!Poutfp)  {
//			message(noreport);
//			return;
//		}
//		setbuf(Poutfp, NULL);
//	}
	*errflg = false;
	scratch[__cols] = '\n';
	scratch[__cols+1] = 0;
	p = pagebuffer;
	for (row = 1; row <= __rows; row++) {
		memcpy(scratch, p, __cols);
		for (i = __cols - 1; i > 0 && scratch[i] == ' '; i--)
			;
		scratch[i+1] = '\n';
		scratch[i+2] = 0;
		fputs(scratch, Poutfp);
		p += __cols;
	}
	memset(pagebuffer, ' ', __rows*__cols);
	return nullPValue;
}

//  adjust_cols -- Adjust column after printing string
//--------------------------------------------------------------------------------------------------
void adjust_cols (String str)
{
	int c;
	while ((c = *str++)) {
		if (c == '\n')
			curcol = 1;
		else
			curcol++;
	}
}

// poutput outputs a string in the current mode.
void poutput(String str)
{
	String p; //, name;
	int c, len;
	if (!str || *str == 0 || (len = (int) strlen(str)) <= 0) return;
//	if (!Poutfp) {
//		Poutfp = ask_for_file("w", whtout, &name, llreports);
//		if (!Poutfp)  {
//			message(noreport);
//			return;
//		}
//		setbuf(Poutfp, NULL);
//		outfilename = strsave(name);
//	}
	switch (outputmode) {
	case UNBUFFERED:
		fwrite(str, len, 1, Poutfp);
		adjust_cols(str);
		return;
	case BUFFERED:
		if (len > 1024) {
			fwrite(linebuffer, linebuflen, 1, Poutfp);
			fwrite(str, len, 1, Poutfp);
			linebuflen = 0;
			bufptr = linebuffer;
			adjust_cols(str);
			return;
		}
		if (len + linebuflen > 1024) {
			fwrite(linebuffer, linebuflen, 1, Poutfp);
			linebuflen = 0;
			bufptr = linebuffer;
		}
		linebuflen += len;
		while ((c = *bufptr++ = *str++)) {
			if (c == '\n')
				curcol = 1;
			else
				curcol++;
		}
		--bufptr;
		return;
	case PAGEMODE:
		p = pagebuffer + (currow - 1)*__cols + curcol - 1;
		while ((c = *str++)) {
			if (c == '\n') {
				curcol = 1;
				currow++;
				p = pagebuffer + (currow - 1)*__cols;
			} else {
				if (curcol <= __cols && currow <= __rows)
					*p++ = c;
				curcol++;
			}
		}
		return;
	default:
		FATAL();
	}
}
