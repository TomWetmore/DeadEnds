//
//  frame.c
//  Library
//
//  Created by Thomas Wetmore on 20 May 2025.
//  Last changed on 20 May 2025.
//

#include <stdio.h>
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


// deleteFrame frees the memory of a frame. Deletes only the SymbolTable.
void deleteFrame(Frame* frame) {
    deleteSymbolTable(frame->table);
    free(frame);
}
