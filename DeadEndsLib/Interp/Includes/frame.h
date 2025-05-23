//
//  Frame.h
//  Library
//
//  Created by Thomas Wetmore on 20 May 2025.
//  Last changed on 21 May 2025.
//

#ifndef frame_h
#define frame_h

#include "symboltable.h"

typedef HashTable SymbolTable; // Forward reference.

// A Frame holds a routine's call PNode, definition PNode, SymbolTable, and its caller's Frame.
typedef struct Frame Frame;
typedef struct Frame {
    PNode* call; // Routine's call site.
    PNode* defn; // Routine's definition.
    SymbolTable* table; // Routine's SymbolTable.
    Frame* caller; // Frame of the routine's caller.
} Frame;

// Interface to Frame.
Frame* createFrame(PNode*, PNode*, SymbolTable*, Frame*);
void deleteFrame(Frame*);

#endif // frame_h
