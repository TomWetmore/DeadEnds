//
//  context.h
//  C DeadEnds
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 23 May 2025.
//

#ifndef context_h
#define context_h

#include "database.h"
#include "symboltable.h"
#include "frame.h"

// Forward references.
typedef HashTable SymbolTable;
typedef struct Frame Frame;

// Context holds the context in which interpretation takes place.
typedef struct Context {
    Frame* frame; // Frame for running routine.
    Database *database;  // Database of persons, etcetera
    File* file; // File to use for script output.
} Context;

// Public interface.
Context* createContext(Database*); // Create a Context.
void deleteContext(Context*);  // Delete a Context.

#endif // context_h
