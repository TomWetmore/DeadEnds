//
//  DeadEnds
//
//  standard.h -- Useful things.
//
//  Created by Thomas Wetmore on 1 November 2022.
//  Last changed on 26 November 2023.
//

#ifndef standard_h
#define standard_h

#define DEBUGALLOCS // EMPTY

typedef char* String;

#include <sys/types.h>
#include <stdlib.h>  // malloc, free, abort.
#include <stdio.h>  // FILE, fopen, fclose, printf, sprintf, fprintf.
#include <string.h>  // strlen, strcmp, strcpy, strcmp, strrchr.
#include <ctype.h>
#include <stdbool.h>  // bool, true and false.
#include <unistd.h>
#include "path.h"

#define MAXSTRINGSIZE 512

#define NAN 313131  // Poor man's implementation of NAN for integers.

// Useful typedefs.
//--------------------------------------------------------------------------------------------------
typedef void* Word;
//typedef char* String;

// CharacterType -- Characters are partitioned into different types.
//--------------------------------------------------------------------------------------------------
typedef enum { Letter = 300, Digit, White, Other } CharType;  // TODO: NOT USED YET.

String strsave(String);  // Save a string in the heap.
bool iswhite(int);       // Check if a character is white space.
bool allwhite(String);   // Check if a string is all white space.
void striptrail(String);  // Strip trailing white space.
int chartype(int);      // Return the 'type' of a character.
void alloc_out(String); // standard.c
CharType characterType(int);  // standard.c
String lower(String);  // Convert a string to lower case.
String upper(String);  // Convert a string to upper case.
String capitalize(String);

#define MAXLINELEN 4096  // Maximum length allowed for Gedcom lines when reading files.

#define unused(x) (void)(x)


// Max and min. TODO: DON'T KNOW IF THESE ARE USED.
//--------------------------------------------------------------------------------------------------
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef min
#define min(x,y) ((x)>(y)?(y):(x))
#endif

// User interface to the standard functions.
//--------------------------------------------------------------------------------------------------
char* __alloc(size_t, String, int);
void __free(void* ptr, String, int);
bool isLetter(int ascii);  // Check if a character is a letter.
String trim(String, int);
//void logAllocations(bool);  // Turn allocation logging on and off.
void __logAllocations(bool);  // Turn allocation logging on and off.

// Turn on alloc and free debugging.
#ifdef DEBUGALLOCS
#define stdalloc(l)   __alloc(l, __FILE__, __LINE__)
#define stdfree(p)    __free(p, __FILE__, __LINE__)
#define logAllocations(b) __logAllocations((b))
// Otherwise stdalloc and stdfree use malloc and free directly.
#else
#define stdalloc(l) malloc((l))
#define stdfree(p) free((p))
#define logAllocations(b)
#endif

#define fatal(s)      __fatal(__FILE__, __LINE__)
#define FATAL()       __fatal(__FILE__, __LINE__)
#define ASSERT(b)     if(!(b)) __fatal(__FILE__, __LINE__)
#define eqstr(s,t)    (!strcmp((s),(t)))
#define nestr(s,t)    (strcmp((s),(t)))

#define ARRAYSIZE(a)	(sizeof(a)/sizeof(a[0]))

void __fatal(String, int); // standard.c
void __assert(bool, String, int);  // standard.c

// These macros are used for character types in the chartype() function.
#define WHITE  ' '
#define LETTER 'a'
#define DIGIT  '0'
#define ZERO    0

#define null ((void*) 0)

// Debugging aids.
#define PH if(debugging) printf("%s %s %d\n", __FUNCTION__, lastPathSegment(__FILE__), __LINE__);

#endif
