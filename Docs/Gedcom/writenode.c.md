# writenode.c
|Component|Description|
|:---|:---|
|bool gnodesToFile(int level, GNode* gnode, String fileName, bool indent)|Write a GNode tree to a gedcom file. Opens the file, calls writeGNodes to write the nodes, and closes the file. Returns whether the write occurred.|
|static void writeGNode(FILE \*fp, int level, GNode* gnode, bool indent)|Write a single GNode to a file. Called by writeGNodes.|
|static void writeGNodes(FILE \*fp, int level, GNode* gnode, bool indent, bool kids, bool sibs)|Write a GNode tree or forest to a Gedcom file. Recurse to children and siblings when flags are set.|
|String gnodesToString(GNode* gnode)|Convert a GNode tree into a Gedcom string. Find the length of final string and allocate it; fill the string with the output; and return the string.|
|String gnodeToString(GNode* gnode, int level)|Return a single GNode as a string. Calls nodeStringLength to get the length of string, then calls swriteGNode to write the node to the string.|
|static String swriteGNode(int level, GNode* node, String p)|Write a GNode to a string. Return the  position in the string for the next node.|
|static String swriteGNodes (int level, GNode* gnode, String p)|Write a GNode tree to a string. Recurses to children and siblings.|
|int treeStringLength(int level, GNode* gnode)|Recursively compute the string length of a Gedcom tree. This is used to preallocate the memory needed to hold the full string.|
|static int nodeStringLength(int level, GNode* gnode)|Compute a GNode's string length; counts the \n but not the final 0. *Unicode impact*.|