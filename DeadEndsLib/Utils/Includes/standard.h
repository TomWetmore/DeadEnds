// DeadEnds
//
// standard.h defines useful things.
//
// Created by Thomas Wetmore on 1 November 2022.
// Last changed on 16 November 2024.

#ifndef standard_h
#define standard_h

typedef char* String;

#include <sys/types.h>
#include <stdlib.h> // malloc, free, abort.
#include <stdio.h>  // FILE, fopen, fclose, printf, sprintf, fprintf.
#include <string.h> // strlen, strcmp, strcpy, strcmp, strrchr.
#include <ctype.h>
#include <stdbool.h> // bool, true and false.
#include <unistd.h>
#include "path.h"

//#define DEBUGALLOCS // EMPTY
#define MAXSTRINGSIZE 512
#define NAN 1313131313  // Poor man's implementation of NAN for integers.

// CharacterType -- Characters are partitioned into different types.
typedef enum { Letter = 300, Digit, White, Other } CharType;

String strsave(String);  // Save String in heap.
bool iswhite(int);       // Is character white space?
bool allwhite(String);   // Is String all white space?
void striptrail(String);  // Strip trailing white space.
int chartype(int);      // Return type of a character.
void alloc_out(String); // standard.c
CharType characterType(int);  // standard.c
String lower(String);  // Convert a string to lower case.
String upper(String);  // Convert a string to upper case.
String capitalize(String);

void basicDelete(void*);

#define MAXLINELEN 4096  // Max length for Gedcom lines when reading files.

#define unused(x) (void)(x)


// Max and min macros.
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef min
#define min(x,y) ((x)>(y)?(y):(x))
#endif

// User interface to the standard functions.
void* _alloc(size_t, String, int);
void _free(void* ptr, String, int);
bool isLetter(int);  // Is character is an Ascii letter?
String trim(String, int); // Trim String to size.
void _logAllocations(bool);  // Turn allocation logging on and off.

#ifdef DEBUGALLOCS // Debugging allocs and free.
	#define stdalloc(l) _alloc(l, __FILE__, __LINE__)
	#define stdfree(p)  _free(p, __FILE__, __LINE__)
	#define logAllocations(b) _logAllocations((b))
#else // Not debugging allocs and frees.
	#define stdalloc(l) malloc((l))
	#define stdfree(p) free((p))
	#define logAllocations(b)
#endif

#define fatal(s)      _fatal(__FILE__, __LINE__)
#define FATAL()       _fatal(__FILE__, __LINE__)
#define ASSERT(b)     if(!(b)) _fatal(__FILE__, __LINE__)
#define eqstr(s,t)    (!strcmp((s),(t)))
#define nestr(s,t)    (strcmp((s),(t)))

#define ARRAYSIZE(a)	(sizeof(a)/sizeof(a[0]))

void _fatal(String, int); // standard.c
void _assert(bool, String, int); // standard.c

// These macros are used for character types in the chartype() function.
#define WHITE  ' '
#define LETTER 'a'
#define DIGIT  '0'
#define ZERO    0

#define null ((void*) 0)

// Debugging aids.
#define PH if(debugging) fprintf(stderr, "%s %s %d\n", __FUNCTION__,\
	lastPathSegment(__FILE__), __LINE__);

#endif // standard_h
