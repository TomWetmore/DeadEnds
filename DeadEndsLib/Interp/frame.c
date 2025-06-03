//
//  DeadEnds Library
//  frame.c
//
//  Created by Thomas Wetmore on 20 May 2025.
//  Last changed on 2 June 2025.
//

#include <stdio.h>
#include "standard.h"
#include "symboltable.h"
#include "frame.h"

// createFrame creates a new frame for the run time stack.
Frame* createFrame(PNode* pnode, PNode* routine, SymbolTable* table, Frame* caller) {
    Frame* frame = (Frame*) stdalloc(sizeof(Frame));
    frame->call = pnode;
    frame->defn = routine;
    frame->table = table;
    frame->caller = caller;
    return frame;
}


// deleteFrame frees the memory of a frame. Deletes the SymbolTable and the shell.
void deleteFrame(Frame* frame) {
    deleteSymbolTable(frame->table);
    stdfree(frame);
}
