//
//  DeadEnds
//
//  lexer.h -- Header file for the objectified Lexer.
//
//  Created by Thomas Wetmore on 12 December 2022.
//  Last changed on 18 June 2025.
//

#ifndef lexer_h
#define lexer_h

#include <stdio.h>    // FILE.
#include "standard.h" // String, Word.
#include "pnode.h"    // PNode.
#include "y.tab.h"    // Token types generated when yacc processes yacc.y.

// SemanticValue is the type of tokens and non-terminals that need semantic values.
typedef union SemanticValue {
    long integer;      // Integer constants for ICONS tokens.
    double floating;   // Floating constant for FCONS tokens.
    String identifier; // Identifier constants for IDEN tokens.
    String string;     // String constants for SCONS tokens; double quotes not included.
    PNode *pnode;      // PNode is the semantic value type used by several non-terminals.
} SemanticValue;

#endif // lexer_h
