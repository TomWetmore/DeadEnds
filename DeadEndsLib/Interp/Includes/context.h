//
//  DeadEnds Library
//  context.h
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 3 June 2025.
//

#ifndef context_h
#define context_h

typedef struct Database Database;
typedef struct HashTable HashTable;
typedef HashTable FunctionTable;
typedef HashTable SymbolTable;
typedef struct PValue PValue;
typedef struct Frame Frame;
typedef struct File File;
typedef struct List List;

// Program holds the context in which interpretation takes place.
typedef struct Context {
    Database* database;
    Frame* frame;
    File* file; // Current program output file.
    SymbolTable* globals; // Global symbol table.
    FunctionTable* procedures; // User defined procedures.
    FunctionTable* functions; // User defined functions.
    List* fileNames; // Names of parsed script files.
    int parseErrors;
} Context;


// Public interface.
Context* createEmptyContext(void);
Context* createContext(Database*, File*); // Create a Context.
void deleteContext(Context*);  // Delete a Context.

#endif // context_h
