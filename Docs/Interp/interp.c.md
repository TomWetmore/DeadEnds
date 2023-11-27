# interp.c

|Component|Description|
|:---|:---|
|void initializeInterpreter(Database*)|Initialize the interpreter.|
|void finishInterpreter(void)|Finish the interpreter.|
|InterpType interpret(PNode\*, SymbolTable\*, PValue\*)|Interpret a list of program nodes. If a return node is encountered the function returns at that point with the return value as the last parameter. The language allows expressions at the statement level, so top level expressions are also interpreted. Output goes to the output file when any statement or top level expression evaluates to a string.|
|InterpType interpChildren(PNode\*, SymbolTable\*, PValue\*)|Interpret children loop. Loops through the children of a family.|
|InterpType interpSpouses(PNode\*, SymbolTable\*, PValue\**)|Interpret spouse loop. Loops through the spouses of a person.|
|InterpType interpFamilies(PNode\*, SymbolTable\*, PValue\*)|Interpret family loop (families a person is in as a spouse).|
|InterpType interpFathers(PNode\*, SymbolTable\*, PValue\*)|Interpret fathers loop. Most persons will have one father in the database, so most of the time the loop body will interpreted once.|
|InterpType interpMothers(PNode\*, SymbolTable\*, PValue\*)|Interpret mothers loop.|
|InterpType interpParents(PNode\*, SymbolTable\*, PValue\*)|Interpret parents loop; loops over all families a person is a child in. *Does this exist in LifeLines?*|
|InterpType interp_fornotes(PNode\*, SymbolTable\*, PValue\*)|Interpret notes loop.|
|InterpType interp_fornodes(PNode\*, SymbolTable\*, PValue\*)|Interpret fornodes loop. Loops through the children of a Gedcom node.|
|InterpType interpForindi(PNode\*, SymbolTable\*, PValue\*)|Interpret the forindi loop statement.|
|InterpType interp_forsour(PNode\*, SymbolTable\*, PValue\*)|Interpret the forsour loop statement. *Not ported.*|
|InterpType interp_foreven(PNode\*, SymbolTable\*, PValue\*)|Interpret the foreven loop statement.|
|InterpType interp_forothr(PNode\*, SymbolTable\*, PValue\*)|Interpret the forothr loop statement.|
|InterpType interpForFam (PNode\*, SymbolTable\*, PValue\*)|Interpret forfam loop statement. Loop through every family in the database. *Not ported.*|
|InterpType interp_indisetloop(PNode\*, SymbolTable\*, PValue\*)|Interpret a sequence loop statement.|
|InterpType interpIfStatement(PNode\*, SymbolTable\*, PValue\*)|Interpret an if statement.|
|InterpType interpWhileStatement(PNode\*, SymbolTable\*, PValue\*)|Interpret a while statement.|
|InterpType interpProcCall(PNode\*, SymbolTable\*, PValue\*)|Interpret a procedure call statement. This gets the procedure definition from the procedure table, evaluates the list of arguments and binds them to parameters in the symbol table, and then calls interpret on the first statement of the body.|
|InterpType interpTraverse(PNode\*, SymbolTable\*, PValue\*)|Interpret the traverse statement. This traverses a Gedcom node tree. This function adds two entries to the symbol table for the loop variables, and removes them when the loop finishes.|
|void prog_error(PNode*, String fmt, ...)|Report a run time program error.|)|Interpret notes loop.|
