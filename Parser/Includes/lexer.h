//
//  DeadEnds
//
//  lexer.h -- Header file for the objectified Lexer.
//
//  Created by Thomas Wetmore on 12 December 2022.
//  Last changed on 28 September 2023.
//

#ifndef lexer_h
#define lexer_h

#include <stdio.h>    // FILE.
#include "standard.h" // String, Word.
#include "pnode.h"    // PNode.
#include "y.tab.h"    // Token types generated when yacc processes yacc.y.

// LexerType -- Specifies whether the lexer reads from a file or string.
//--------------------------------------------------------------------------------------------------
typedef enum { FileMode, StringMode } LexerType;

// SemanticValue -- Semantic values of the tokens and non-terminals that need them.
//--------------------------------------------------------------------------------------------------
typedef union SemanticValue {
    long integer;      // Integer constants for ICONS tokens.
    double floating;   // Floating constant for FCONS tokens.
    String identifier; // Identifier constants for IDEN tokens.
    String string;     // String constants for SCONS tokens; double quotes not included.
    PNode *pnode;      // PNode is the semantic value type used by several non-terminals.
} SemanticValue;

#endif // lexer_h
