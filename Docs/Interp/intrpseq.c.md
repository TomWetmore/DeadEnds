# intprseq.c

This file has the report programming language's built-in functions that implement the *INDISET* data type. The data type generally holds lists of persons.
The underlying C datatype that implements the *INDISEQ* type is the *Sequence* structure. As for all the built-in functions, the PNode argument is the the function node that holds the argument nodes of the function, the SymbolTable holds the local symbol table, and the boolean passes back whether an error occurred.

|Component|Description|
|:---|:---|
|PValue __indiset (PNode\*, SymbolTable\*, bool \*err)|Declare and create a sequence and assign it to an identifier in a symbol table. Usage: *indiset(IDEN) &rarr; VOID*.|
|PValue __addtoset (PNode\*, SymbolTable\*, bool \*err)|Add a person to a sequence. Usage: *addtoset(SET, INDI, ANY) &rarr; VOID*.|
|PValue __lengthset (PNode\*, SymbolTable\*, bool \*err)|Return the length of a sequence. Usage: *lengthset(SET) &rarr; INT*.|
|PValue __inset (PNode\*, SymbolTable\*, bool \*err)|See if a person is in a sequence. Usage: *inset(SET, INDI) &rarr; BOOL*.|
|PValue __deletefromset (PNode\*, SymbolTable\*, bool \*err)|Remove a person from a sequence. Usage: *deletefromset(SET, INDI, BOOL) &rarr; VOID*.|
|PValue __namesort(PNode\*, SymbolTable\*, bool \*err)|Sort a sequence by the persons' names. Usage: *namesort(SET) &rarr; VOID*.|
|PValue __keysort (PNode\*, SymbolTable\*, bool \*err)|Sort a sequence by the persons' keys. Usage: *keysort(SET) &rarr; VOID*.|
|PValue __valuesort (PNode\*, SymbolTable\*, bool \*err)|Sort a sequence by its values. Usage: *valuesort(SET) &rarr; VOID*.|
|PValue __uniqueset (PNode\*, SymbolTable\*, bool \*err)|Eliminate duplicates from a sequence. Usage: *uniqueset(SET) &rarr; SET*.|
|PValue __union(PNode\*, SymbolTable\*, bool \*err)|Create the union of two sequences. Usage: *union(SET, SET) &rarr; SET*.|
|PValue __intersect (PNode\*, SymbolTable\*, bool \*err)|Create the intersection of two sequences. Usage: *intersect(SET, SET) &rarr; SET*.|
|PValue __difference(PNode\*, SymbolTable\*, bool \*err)|Create the difference of two sequences. Usage: *difference(SET, SET) &rarr; SET*.|
|PValue __parentset(PNode\*, SymbolTable\*, bool \*err)|Create the parent sequence of a sequence. Usage: *parentset(SET) &rarr; SET*.|
|PValue __childset(PNode\*, SymbolTable\*, bool \*err)|Create the child sequence of a sequence. Usage: *childset(SET) &rarr; SET*.|
|PValue __siblingset(PNode\*, SymbolTable\*, bool \*err)|Create the sibling sequence of a sequence. Usage: *siblingset(SET) &rarr; SET*.|
|PValue __spouseset (PNode\*, SymbolTable\*, bool \*err)|Create the spouse sequence of a sequence. Usage: *spouseset(SET) &rarr; SET*.|
|PValue __ancestorset (PNode\*, SymbolTable\*, bool \*err)|Create the ancestor sequence of a sequence. Usage: *ancestorset(SET) &rarr; SET*.|
|PValue __descendentset (PNode\*, SymbolTable\*, bool \*err)|Create the descendent sequence of a sequence. Two spellings allowed. Usage: *descendentset(SET) &rarr; SET* or *descendantset(SET) &rarr; SET*.|
|PValue __gengedcom(PNode\*, SymbolTable\*, bool \*errg)|Generate Gedcom output from a sequence. Usage: *gengedcom(SET) &rarr; VOID*.|