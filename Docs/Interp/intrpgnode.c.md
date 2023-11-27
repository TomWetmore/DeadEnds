# intprgnode.c

The functions in this file implement the built-in functions that handle Gedcom nodes in the report programming language.

|Component|Description|
|:---|:---|
|PValue __key (PNode\*, SymbolTable\*, bool\*)|Return the person or family key (or event or source or other) from a root GNode. Usage: *key(INDI\|FAM) -> STRING*.|
|PValue __xref (PNode\*, SymbolTable\*, bool\*)|Return the xref (aka key) field of a GNode. *Usage: xref(NODE) -> STRING*.|
|PValue __tag (PNode\*, SymbolTable\*, bool\*)|Return the tag field of a GNode. Usage: *tag(NODE) -> STRING*.|
|PValue __value (PNode\*, SymbolTable\*, bool\*)|Return the value field of a GNode. It may be empty. Usage: *value(NODE) -> STRING*.|
|PValue __parent (PNode\*, SymbolTable\*, bool\*)|Return the parent GNode of a GNode. Note: root GNodes do not have parents. Usage: *parent(NODE) -> NODE*.|
|PValue __child(PNode\*, SymbolTable\*, bool\*)|Return the child GNode of a GNode. Usage: *child(GNODE) -> GNODE*.|
|PValue __sibling(PNode\*, SymbolTable\*, bool\*)|Return the sibling GNode of a GNode. Usage: *sibling(NODE) -> NODE*.|