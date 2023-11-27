# builtinlist.c

This file has the functions that implement the list datatype built-ins. They use the list structure as the underlying C type. The elements of the lists are program values. Memory management is an issue to be dealt with carefully.

|Component|Description|
|:---|:---|
|PValue __list (PNode\*, SymbolTable\*, bool\*)|Create a list. Usage: *list(IDENT) -> VOID*.|
|PValue __push (PNode\*, SymbolTable\*, bool\*)|Add an element to the front of a list. Usage: *push(LIST, ANY) -> VOID* or *enqueue(LIST, ANY) -> VOID*.|
|PValue __requeue (PNode\*, SymbolTable\*, bool\*)|Add an element to back of a list. Usage: *requeue(LIST, ANY) -> VOID*.|
|PValue __pop (PNode\*, SymbolTable\*, bool\*)|Pop and return an element from the front of a list. Usage: *pop(LIST) -> ANY*.|
|PValue __dequeue (PNode\*, SymbolTable\*, bool\*)|Remove and return an element from the back of a list. Usage: *dequeue(LIST) -> ANY*.|
|PValue __empty (PNode\*, SymbolTable\*, bool\*)|Check if a list is empty. Usage: *empty(LIST) -> BOOL*.|
|PValue __getel (PNode\*, SymbolTable\*, bool\*)|Return the nth value from a list. Usage: *getel(LIST, INT) -> ANY*.|
|PValue __setel (PNode\*, SymbolTable\*, bool\*)|Set the nth value in a list. Usage: *setel(LIST, INT, ANY) -> VOID*.|
|PValue __length (PNode\*, SymbolTable\*, bool\*)|Return the length of a list. Usage: *length(LIST) -> INT*.|
|InterpType interpForList (PNode\*, SymbolTable\*, PValue\*pval)|Interpret list loop. Usage: *forlist(LIST, ANY, INT) {BODY}*.|