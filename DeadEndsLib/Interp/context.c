//
//  DeadEnds Library
//  context.c
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 2 June 2025.
//

#include <stdio.h>
#include "context.h"
#include "frame.h"
#include "file.h"

// createEmptyContext creates an empty Context.
Context* createEmptyContext(void) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    if (context) memset(context, 0, sizeof(Context));
    return context;
}

// createContext creates a Context from a Database and a File.
Context* createContext(Database *database, File* file) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    context->frame = null;
    context->database = database;
    context->file = file;
    return context;
}

// deleteContext deletes a Context; it deletes the run time stack and closes the file, but leaves the database.
void deleteContext(Context *context) {
    Frame* frame = context->frame;
    while (frame) {
        Frame* parent = frame->caller;
        deleteFrame(frame);
        frame = parent;
    }
    if (context->file) closeFile(context->file);
    stdfree(context);
}
