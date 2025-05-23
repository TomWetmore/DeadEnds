//
//  context.c
//  Library
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 23 May 2025.
//

#include <stdio.h>
#include "context.h"
#include "frame.h"

// createContext creates a Context from a SymbolTable and Database.
Context* createContext(Database *database) {
    Context* context = (Context*) stdalloc(sizeof(Context));
    context->frame = null;
    context->database = database;
    context->file = stdOutputFile();
    return context;
}

// deleteContext deletes a Context; deletes the Symboltable but not the Database.
void deleteContext(Context *context) {
    deleteHashTable(context->frame->table);
    free(context);
}
