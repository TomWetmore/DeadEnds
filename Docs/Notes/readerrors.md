# Importing Gedcom Files and Handling Errors

## Introduction

This note describes the code that imports Gedcom records from Unix files, and converts them into GNode trees in DeadEnds databases. It also describes the errors that can happen during import, and how they  are handled. *This is an area in flux*.

Errors that can happen during import:

1. System errors such as failing to open a file, or read a file, or allocate memory.
1. Syntax errors in Gedcom lines. A line might not be well-formed, or have an incorrect level. These errors prevent the file from being imported, but the program should not quit, so that later errors can be found.
1. Errors can occur at higher levels. For example, certain line types require formatted values (e.g., NAME and SEX lines in persons). These are treated at the validation level.
1. Errors can occur in inter-record links. Lineage linking requires that roles in person and family records (FAMS, FAMC, CHIL, WIFE, HUSB) refer, by key value, to other records. Source links in records must refer to existing source records. These also are treated above this level.

Errors must be reported to the user in a way that can be  understood, located in the Gedcom files, and fixed. For this to happen the program must know the file name and the line numbers where the errors occur. DeadEnds uses the *ErrorLog* type to hold the errors. Errors of different types can be added to logs during different phases, so they are sortable to organize the errors in a logical fashion.


## Read Stack
The *Read Stack* is the layering of software functions that convert Gedcom records from UNIX Gedcom files into their GNode tree form in *DeadEnds* databases. Each Gedcom file is converted to a single *Database*. The stack checks the syntax of the Gedcom files, whether the lines are well-formed and the level numbers are correct. A later validation phase checks the semantics of the records and the interrelationships betweeen records. The functions in the stack are found in import.c and readnode.c.

### Layers of the Stack
|||
|:--|:--|
|List *importFromFiles(String[] files, int count, ErrorLog\*)|Import Gedcom  record trees from a list of Gedcom files. If all imports are successful, return a list of Databases, one for each file. If there were errors then no Databases are returned, and the ErrorLog holds the list of errors found.|
|Database *importFromFile(String file, ErrorLog\*)|Import the Gedcom record trees from a single Gedcom file. If the import is successful return the Database of the records read from the file. If there were errors no Database is returned, and the ErrorLog holds the list of errors found.|
|GNode *firstNodeTreeFromFile(FILE\*, String file, int \*lineno, ErrorLog\*)|Read and return the first GNode tree in a file.|
|GNode \*nextNodeTreeFromFile(FILE\*, int \*lineNo, ErrorLog\*e)|Read and return the next GNode tree in the file.|
|static ReadReturn fileToLine(FILE\*, int\*level, String \*key, String \*tag, String \*value, Error\*\*)|Read and return the next Gedcom line from a file.|
|static ReadReturn extractFields (String p, int \*level, String \*key, String \*tag, String \*value, Error\*\*)|Extract the fields from a Gedcom line.|
    
#### List *importFromFiles(String[] fileNames, int count, ErrorLog *errorLog)
*importFromFiles* is the function at the top of the stack. It is passed an array of file names, the length of the array, and an error log. It calls *importFromFile* on each file. It puts the databases returned by *importFromFile* into a list of databases that it then returns. If errors are encountered they are added to the error log. Depending on the types of errors, databases from one or more files may not be created. If an application needs only to read a single file into a single database, it can call *importFromFile* directly.

#### Database *importFromFile(String fileName, ErrorLog *errorLog)
*importFromFile* is passed the name of a Gedcom file and an error log. It reads the file, breaking it into GNode trees, that are added to database it creates. If errors are encountered they are added to the error log. If there are errors a database is not be created and the function returns null.

*importFromFile* calls *fopen* to open the file for reading. The file handle is passed to lower layers. The function also calls *createDatabase* to create the database. Each database is given the name of the file it was built from

To read records from the file, *importFromFile* calls *firstNodeTreeFromFile* and *nextNodeTreeFromFile*, the former being called once to set up state variables. After each call to these functions, *importFromFile* calls *storeRecord* to put the record in the database.

#### NodeList *getNodeTreesFromNodeList(NodeList *lowerList, ErrorLog *errorLog)
*getNodeTreesFromNodeList* is passed a list of GNodes and an ErrorLog. It is called by *importFromFile* after *importFromFile* has called *getNodeListFromFile*.

#### NodeList \*getNodeListFromFile (FILE\*)
*getNodeListFromFile* uses *fileToLine* and *extractFields* to create a GNode for each line in a Gedcom file. Lines with errors store Errors in the list rather than GNodes.



#### static ReadReturn fileToLine(FILE *file, int *level, String *key, String *tag, String *value, Error **error)

*fileToLine* gets the next line from the Gedcom file, and places the four fields of the line (key, level, tag and value) in output parameters. If an error occurs reading the line an error message is also returned. The functional return value is an integer code, for NORMAL, ERROR, and ATEOF. (*This is not currently true.*)

*fileToLine* uses *fgets* to read the next line from the file. If *fgets* returns 0 the file is at end of file and *fileToLine* returns the EOF code. If *fgets* does get characters from the file it puts them in a buffer and then calls *extractFields* to scan the line and extract the four fields.

*fileToLine* calls *extractFields* to do the lexical work extracting the fields.
    
#### static int extractFields (String p, int* level, String* key, String* tag, String* value, String* errorMsg)

*extractFields* processes a Gedcom line, extracting the key (if any), level, tag, and value (if any) fields. The line may have a newline at the end. This function is called by both *fileToLine* and *stringToLine* to the lexical work required to extract the field.



# Read Stack


A key function in readnode.c is the static function *extractFields*, which processes strings that hold single Gedcom lines. This function is called by both *fileToLine* and *stringToLine*, which allows the file handling and string handling functions to avoid duplicated code.

The higher level functions in the stack, those used by external code, are *firstNodeTreeFromFile* and *nextNodeTreeFromFile*.

The last argument to some of the functions is a pointer to an error message. This has to be changed to an *Error* object. In the current implementation, higher level code, specifically importing code, must transform these error messages into error objects to be placed in an error log.

(Reading Gedcom from strings was needed in *LifeLines*, because its database records are strings. *DeadEnds* keeps its records in GNode form so the string reading function in this file, *stringToLine*, is not yet needed in *DeadEnds*.)

|Component|Description|
|:---|:---|
|int fileLine = 0|Current line in file being read. Used in any error messages needed.|
|int fileToLine(FILE\*, int *level, String *key, String *tag, String *value, String \*errmsg)|Read the next Gedcom line from a file.|
|static bool stringToLine(String *ps, int *level, String *key, String *tag, String *value, String \*errmsg)|Get the next Gedcom line from a string holding one or more lines. It reads to the next newline, if any, and processes that part of the  string.|
|static int extractFields (String p, int\* level, String \*key, String \*tag, String \*value, String \*errmsg)|Process a Gedcom line, extracting the level, the key, if any, the tag, and the value, if any. Called by both fileToLine and stringToLine.|
|GNode \*firstNodeTreeFromFile (FILE\*, String \*errmsg)|Convert the first Gedcom record in a file to a GNode tree.|
|GNode \*nextNodeTreeFromFile(FILE\*, String *errmsg)|Convert the next Gedcom record in a file to a GNode tree.|
|GNode* stringToNodeTree(String str)|Convert a string holding a  Gedcom record to a GNode tree. _Was used by LifeLines when reading records from its database. Not yet needed by DeadEnds_.|

### Notes on Error Handling
Function *extractFields* does the heavy lifting in reading Gedcom lines and returning the fields needed to build GNodes. Normally *extractFields* returns the integer code of OKAY with pointers to all fields with their values. However, when an error occurs in the Gedcom, the function returns the code ERROR, and the fields are not set. In this case the function returns an error message in a string. The message will normally have the line number from the originial file so the user can locate and fix the error in the file.

## Got to Rework the Functions that Read GNode Trees from Files

Currently done by *firstNodeTreeFromFile* and *nextNodeTreeFromFile* functions inc *readnode.c*.

Here is the flow when things go correctly. *importFromFile* calls *firstNodeTreeFromFile* which reads the first line of the Gedcom file, a level 0 line, and puts its fields into state variables. *fNTFF* then calls *nNTFF* to read the rest of the record the record which it returns to *iFF* through *fNTFF*. After this *iFF* calls *nNTFF* repeatedly until all records have been read. *iFF* stores each record in a database.

Because *nNTFF* does not know when the next level 0 line will appear in the file, it always reads *one too many* lines, so it stores the fields of that next root line in the state variables.

But lots of things can go wrong.
The file might not exist.

The file mignt not be openable or readable.

The first line might not be level 0, or mignt not be Gedcom at all.

At any time a line might have syntax errors.

A line might have have an invalid level, that is, two or more deeper than the current line.

Certain semantic errors can be found, for example, reuse of keys.

## Redesign Ideas

Read each line of a Gedcom file into a GNode and create a List of those GNodes. The elements could either be the GNodes themselves or a structure that refers to the GNode and has other fields. Possible other fields would be the line's line number in the file, an error created while processing the line, if any; and . Maybe if the line is in horrible shape, the GNode field might even be empty, or have a thrid field which is a copy of the string actually read from the file.