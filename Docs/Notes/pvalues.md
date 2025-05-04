When String PValues are taken from the symbol table their values must be copied.
Places where String PValues go "out of scope":
    1. In the __concat function.
    2. When they are printed at the top level.
Is there anything special to do when a String PValue is assigned to a variable?
