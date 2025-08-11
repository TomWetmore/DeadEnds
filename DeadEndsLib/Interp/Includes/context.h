//
//  DeadEnds Library
//  context.h
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 11 August 2025.
//

#ifndef context_h
#define context_h

typedef struct Context Context;
typedef struct Database Database;
typedef struct HashTable HashTable;
typedef HashTable FunctionTable;
typedef HashTable SymbolTable;
typedef struct PValue PValue;
typedef struct Frame Frame;
typedef struct File File;
typedef struct List List;

void validateCalls(Context*);

// Context holds the context in which interpretation takes place.
typedef struct Context {
    Database* database; // The database.
    Frame* frame; // The bottom frame on the run time stack.
    File* file; // Current program output file.
    SymbolTable* globals; // Global symbol table.
    FunctionTable* procedures; // User defined procedures.
    FunctionTable* functions; // User defined functions.
} Context;


// Public interface.
Context* createEmptyContext(void); // Create an empty context.
Context* createContext(Database*, File*); // Create a with a database and file.
void deleteContext(Context*);  // Delete a context.

#endif // context_h
