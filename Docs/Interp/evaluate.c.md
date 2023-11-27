# evaluate.c

The functions in this file evaluate PNode expressions during report program interpretation. Function *evaluate()* is the generic function evaluator. It checks the type of the PNode passed to it, and depending on that type, either evaluates the PNode directly or calls a more specialized evaluator function to deal with the PNode type. In most cases the evaluate functions return PValues (on the stack, not on the heap)

|Component|Description|
|:---|:---|
|PValue evaluate(PNode\*, SymbolTable\*, bool\*)|Function evaluate() takes a PNode expression and evaluates it to a PValue. Evaluation starts in this function. Based on the type of PNode, a more specialized function may be called. Only PNodes of type PNICons, PNSCons, PNSCons, PNIdent, PNBltinCall, and PNFuncCall can be evaluated. Program nodes are heap objects because they form graph structures that must persist after the parser builds them.|
|bool evaluateConditional(PNode\*, SymbolTable\*, bool\*)|Evaluate a conditional expression. Conditional expressions have the form ([iden,] expr), where the identifier is optional. If it is there the value of the expression is assigned to it. This function is called from interpIfStatement and interpWhileStatement.|
|PValue evaluateBuiltin(PNode\*, SymbolTable\*, bool\*)|Evaluate a built-in function by calling its C code.|
|PValue evaluateUserFunc(PNode\*, SymbolTable\*, bool\*)|Evaluate a user defined function.|
|PValue evaluateBoolean(PNode\*, SymbolTable\*, bool\*)|Evaluate a PNode expression and convert it to a boolean PValue using C-like rules. In all but the error case this returns truePValue or falsePValue.|
|static bool pvalueToBoolean(PValue)|Convert a PValue to a bool using C-like rules. Called by evaluateConditional.|
|GNode* evaluatePerson(PNode\*, SymbolTable\*, bool\*)|Evaluate a person PNode expression. Return the root GNode of the person if there.|
|GNode* evaluateFamily(PNode\*, SymbolTable\*, bool\*)|Evaluate a family PNode expression. Return the root GNode of the family if there.|
|GNode* evaluateGNode(PNode\*, SymbolTable\*, bool\*)|Evaluate any Gedcom PNode expression. Return the Gedcom GNode.|
|bool iistype (PNode\*, int type)|See if a PNode has the specified type.|
|int num_params (PNode\*)|Return the number of parameters in a list. (*This is just a general purpose length of list routine for PNodes*.)|
|PValue eval_and_coerce(int type, PNode\*, SymbolTable\*, bool\*)|Generic evaluator and coercer. *This might not be called anymore*.|