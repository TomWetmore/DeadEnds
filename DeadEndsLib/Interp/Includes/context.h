//
//  DeadEnds Library
//  context.h
//
//  Created by Thomas Wetmore on 21 May 2025.
//  Last changed on 13 September 2025.
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

// Program holds a (non running) DeadEnds script program.
typedef struct Program {
    FunctionTable* procedures;
    FunctionTable* functions;
    List* globalIdents;
    List* parsedFiles;
} Program;

Program* createProgram(void); // TODO: Change to take parameters.
void deleteProgram(Program*);
void validateCalls(FunctionTable*, FunctionTable*);

// Context holds the context in which interpretation takes place.
typedef struct Context {
    Database* database; // The database.
    Program* program;
    SymbolTable* globals; // Global symbol table.
    Frame* frame; // Bottom frame of run time stack.
    File* file; // Current program output file.
} Context;

// Interface.
Context* createContext(Program*, Database*, File*);
void deleteContext(Context*);

#endif // context_h
