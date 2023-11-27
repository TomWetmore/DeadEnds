# DO NOT USE THIS FILE ANY MORE.
The *Read Stack* is the layering of software functions that convert Gedcom records in UNIX Gedcom files into their GNode tree form in *DeadEnds* databases. Each Gedcom file is converted to a single *Database*.The stack checks the syntax of the Gedcom files, whether the lines are well-formed and the level numbers are correct. A later validation phase checks the semantics of the records and the interrelationships betweeen records. The functions in the stack are found in readnode.c. 

A key function in readnode.c is the static function *bufferToLine*, which processes strings that hold single Gedcom lines. This function is called by both *fileToLine* and *stringToLine*, which allows the file handling and string handling functions to avoid duplicated code.

The higher level functions in the stack, those used by external code, are *firstNodeTreeFromFile* and *nextNodeTreeFromFile*.

The last argument to some of the functions is a pointer to an error message. This has to be changed to an *Error* object. In the current implementation, higher level code, specifically importing code, must transform these error messages into error objects to be placed in an error log.

(Reading Gedcom from strings was needed in *LifeLines*, because its database records are strings. *DeadEnds* keeps its records in GNode form so the string reading function in this file, *stringToLine*, is not yet needed in *DeadEnds*.)

|Component|Description|
|:---|:---|
|int fileLine = 0|Current line in file being read. Used in any error messages needed.|
|int fileToLine(FILE\*, int *level, String *key, String *tag, String *value, String \*errmsg)|Read the next Gedcom line from a file.|
|static bool stringToLine(String *ps, int *level, String *key, String *tag, String *value, String \*errmsg)|Get the next Gedcom line from a string holding one or more lines. It reads to the next newline, if any, and processes that part of the  string.|
|static int bufferToLine (String p, int\* level, String \*key, String \*tag, String \*value, String \*errmsg)|Process a Gedcom line, extracting the level, the key, if any, the tag, and the value, if any. Called by both fileToLine and stringToLine.|
|GNode \*firstNodeTreeFromFile (FILE\*, String \*errmsg)|Convert the first Gedcom record in a file to a GNode tree.|
|GNode \*nextNodeTreeFromFile(FILE\*, String *errmsg)|Convert the next Gedcom record in a file to a GNode tree.|
|GNode* stringToNodeTree(String str)|Convert a string holding a  Gedcom record to a GNode tree. _Was used by LifeLines when reading records from its database. Not yet needed by DeadEnds_.|

### Notes on Error Handling
Function *bufferToLine* does the heavy lifting in reading Gedcom lines and returning the fields needed to build GNodes. Normally *bufferToLine* returns the integer code of OKAY with pointers to all fields with their values. However, when an error occurs in the Gedcom, the function returns the code ERROR, and the fields are not set. In this case the function returns an error message in a string. The message will normally have the line number from the originial file so the user can locate and fix the error in the file.