//
//  DeadEnds
//
//  standard.c -- Standard routines.
//
//  Create by Thomas Wetmore on 7 November 2022.
//  Last changed on 5 April 2024.

#include <stdlib.h>
#include "standard.h"

#define ALLOCLOGFILE "./alloc.log"

static FILE *allocLogFile = null;  // The logging file.
static bool logopen = false;       // The logging file is open for writing.
static bool loggingAllocs = false; // Whether currently logging.
static long bytesAllocated = 0;
static long bytesFreed = 0;

String version = "deadends.1.0.0";

// logAllocations -- Turn allocation logging on or off. For developer use debugging memory issues.
//--------------------------------------------------------------------------------------------------
void __logAllocations(bool onOrOff)
{
    static bool firstOn = true;  // The first the log is opened it is opened for writing.
    if (onOrOff) {  // Turn on logging.
        if (firstOn) {
            bytesAllocated = 0;
            bytesFreed = 0;
            ASSERT(allocLogFile == null);
            allocLogFile = fopen(ALLOCLOGFILE, "w");  // On first opening, open for writing.
            if (!allocLogFile) fatal("Can't open log file");
            firstOn = false;
        } else {
            if (!allocLogFile) allocLogFile = fopen(ALLOCLOGFILE, "a");  // Else open for appending.
            if (!allocLogFile) fatal("Can't open log file");
        }
        loggingAllocs = true;
    } else {  // Turn off logging.
        if (allocLogFile) {
            fprintf(allocLogFile, "Bytes allocated: %ld\n", bytesAllocated);
            fprintf(allocLogFile, "Bytes freed:     %ld\n", bytesFreed);
            fclose(allocLogFile);
            allocLogFile = null;
        }
        loggingAllocs = false;
    }
}

// __alloc -- Allocate memory - used by stdalloc
//--------------------------------------------------------------------------------------------------
char *__alloc (size_t len, String file, int line)
// int len -- Number of bytes to allocate.
// String file -- File requesting the allocation.
// int line -- Line number in file requesting the allocation.
{
	char *p;
	if (len == 0) return null;
	ASSERT(p = malloc(len));
	//if (loggingAllocs) {
        //bytesAllocated += malloc_size(p);
        //fprintf(allocLogFile, "A  %s\t%d\t%zu\t%ld\t%ld\n", lastSegment(file), line, len, malloc_size(p), (long) p);
	//}
	return p;
}

//  __free -- Deallocate memory - used by sdtfree
//--------------------------------------------------------------------------------------------------
void __free (void* ptr, String file, int line)
// void* ptr -- Memory to deallocate back to the heap.
// String file  -- Name of the file deallocating the memory.
// int line -- Line number in the file where deallocating.
{
	//if (loggingAllocs) {
        //fprintf(allocLogFile, "F  %s\t%d\t%ld\t%ld\n", lastSegment(file), line, malloc_size(ptr), (long) ptr);
        //bytesFreed += malloc_size(ptr);
	//}
	free(ptr);
}

// alloc_out -- Write a message on the allocation log.
//--------------------------------------------------------------------------------------------------
void alloc_out(String str)
// String str -- Message to write on the allocation log.
{
	if (!loggingAllocs) return;
    // Open the allocation log if it isn't. It will remain open for the duration, I think.
	if (!logopen) {
		allocLogFile = fopen("alloc.log", "w");
		logopen = true;
	}
	fprintf(allocLogFile, "%s\n", str);
}

// strsave saves a copy of a String on the heap and returns a pointer to it.
String strsave(String string) {
	if (string == null || *string == 0) return null;
	return strcpy(stdalloc(strlen(string) + 1), string);
}

// strconcat -- Catenate two strings and return a new String on the heap with the value.
//--------------------------------------------------------------------------------------------------
String strconcat(String s1, String s2)
// String s1 -- first string to catenate.
// String s2 -- second string to catenate.
{
	int c;
    String s3, p;

	if (!s1) return strsave(s2);
	if (!s2) return strsave(s1);
	size_t len = strlen(s1) + strlen(s2);
	p = s3 = (String) stdalloc((int) len + 1);
    while ((c = *s1++)) *p++ = c;
    while ((c = *s2++)) *p++ = c;
	*p = 0;
	return s3;
}

// characterType returns the type of a character; not Unicode ready.
CharType characterType(int c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') return White;
    if (c >= 'a' && c <= 'z') return Letter;
    if (c >= 'A' && c <= 'Z') return Letter;
    if (c >= '0' && c <= '9') return Digit;
    return c;
}

// chartype returns the type of a character. DEPRECATED.
int chartype(int c) {
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r') return WHITE;
	if (c >= 'a' && c <= 'z') return LETTER;
	if (c >= 'A' && c <= 'Z') return LETTER;
	if (c >= '0' && c <= '9') return DIGIT;
	return c;
}

// __fatal -- Fatal error routine
//--------------------------------------------------------------------------------------------------
void __fatal (String file, int line)
// String file -- Name of file calling __fatal.
// int line -- Line number of file calling __fatal.
{
	printf("FATAL: %s: line %d\n", file, line);
	//close_lifelines();
	//endwin();
	abort();
}

// __assert -- Make an assertion.
//--------------------------------------------------------------------------------------------------
void __assert (bool exp, String file, int line)
//  exp -- Expression that must evaluate to true.
// String file -- File calling the assert.
// int line -- Line number in file calling the assert.
{
	if (exp) return;
	__fatal(file, line);
}

// iswhite -- Check whether a character is white space. Probably doesn't change with Unicode.
//--------------------------------------------------------------------------------------------------
bool iswhite(int c)
// int c -- Checks whether this character is white space.
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// isLetter -- Checks whether a character is a letter. Must change with Unicode.
//--------------------------------------------------------------------------------------------------
bool isLetter(int c)
// int c -- Checks whether this character is a letter.
{
	if (c >= 'a' && c <= 'z') return true;
	return c >= 'A' && c <= 'Z';
}

//  lower -- Convert a string to lower case. Must change with Unicode.
//    MNOTE: Reuses data space buffere for the returned string.
//--------------------------------------------------------------------------------------------------
String lower(String str)
{
    ASSERT(strlen(str) < MAXSTRINGSIZE);
	static char scratch[MAXSTRINGSIZE];
    String p = scratch;
	int c;
    while ((c = *str++)) *p++ = tolower(c);
	*p = 0;
	return scratch;
}

//  upper -- Convert a string to upper case. Must change with Unicode.
//    MNOTE: Reuses data space buffer for the returned string.
//--------------------------------------------------------------------------------------------------
String upper(String str)
{
    ASSERT(strlen(str) < MAXSTRINGSIZE);
	static char scratch[MAXSTRINGSIZE];
    String p = scratch;
	int c;
    while ((c = *str++)) *p++ = toupper(c);
	*p = 0;
	return scratch;
}

// capitalize -- Capitalize a string. Must change with Unicode.
//    MNOTE: Reuses data space buffer for the returned string.
//--------------------------------------------------------------------------------------------------
String capitalize(String str)
{
    String p = lower(str);
	*p = toupper(*p);
	return p;
}

// toupper -- Convert a letter to uppercase.  Must change with Unicode.
//--------------------------------------------------------------------------------------------------
//int toupper(int c)
//{
//	if (c < 'a' || c > 'z') return c;
//	return c + 'A' - 'a';
//}

// tolower -- Convert letter to lowercase. Must change with Unicode.
//--------------------------------------------------------------------------------------------------
//int tolower(int c)
//// int c;
//{
//	if (c < 'A' || c > 'Z') return c;
//	return c + 'a' - 'A';
//}

// trim -- Trim a String if it is 'too long'.
//--------------------------------------------------------------------------------------------------
String trim(String string, int maxLength)
// String string -- String that may have to be trimmed.
// int maxLength -- Maximum desired length of string.
{
	static char scratch[MAXLINELEN+1];
	if (!string || strlen(string) > MAXLINELEN) return null;
	if (maxLength < 0) maxLength = 0;
	if (maxLength > MAXLINELEN) maxLength = MAXLINELEN;
	strcpy(scratch, string);
	scratch[maxLength] = 0;
	return scratch;
}

// striptrail -- Strip trailing white space.
//--------------------------------------------------------------------------------------------------
void striptrail(String p)
// String p -- (in, out) Strips trailing white space from this string.
{
    char* q = p + strlen(p) - 1;
    while (iswhite(*q) && q >= p) *q-- = 0;
}

// allwhite -- Checks whether a string is made up of all white space.
//--------------------------------------------------------------------------------------------------
bool allwhite (String p)
// String p -- Checks whether this string is all white space.
{
    while (*p) if (!iswhite(*p++)) return false;
    return true;
}
