//
//  DeadEnds Library
//  frame.h is the header file for the sturture that defines the DeadEnds script run time stack.
//
//  Created by Thomas Wetmore on 20 May 2025.
//  Last changed on 3 June 2025.
//

#ifndef frame_h
#define frame_h

typedef struct HashTable HashTable;
typedef HashTable SymbolTable;
typedef struct PNode PNode;

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
