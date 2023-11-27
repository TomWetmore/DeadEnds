# Errors
#### ErrorType
Enumeration of Error types:
```
typedef enum {
    systemError,
    syntaxError,
    gedcomError,
    linkageError,
    scriptError
} ErrorType;
```
#### Error
Error structure:
```
typedef struct {
    ErrorType type;
    String fileName;
    int lineNumber;
    String message;
} Error;
```
|Field|Description|
|:---|:---|
|type|Type of this Error.|
|fileName|Name of file, if any, where error occurred.|
|lineNumber|Line number in file, if any, where error occurred.|
|message|Message that describes the error.|

#### ErrorLog
ErrorLog is a synonym of List. It is a specialized List type.
```
#define ErrorLog List
```
### User Interface

|Component|Description|
|:---|:---|
|ErrorLog \*createErrorLog(void)|Create an Error log.|
|Error \*createError(ErrorType, String fname, int lineno, String message)|Create an Error.|
|void addErrorToLog(ErrorLog\*, Error\*)|Add an Error to an ErrorLog.|

### Notes

#### Errors when reading Gedcom files.

When these occur we can continue reading the file/s to get a list of further errors. But once all files have been read, the program must end with an admonishment to the user to fix errors before continuing. So there has to be an error-log for the user to use to see the list of errors.

#### System Errors

1. Failure to open a file.
2. Failure to read a file.
3. Memory allocation error.

#### Syntax Errors
These are errors in the basic syntax of Gedcom. Each error must provide the file name and line number where it occurred. There must be a way of logging multiple errors because error checking must continue after the first errors are discovered.

1. Lines without level numbers.
1. Lines without tags.
1. Level 0 lines without keys.
1. Non-level 0 lines with keys.
1. Keys with bad format
1. Lines with incorrect levels.

#### Single Gedcom Record Errors
Errors that occur at the level of single Gedcom records.
1. NAME lines without values.
1. SEX lines without values or with incorrect values.
1. FAMC, FAMS, CHIL, HUSB, WIFE lines without keys for values.

#### Lineage Linking Errors
Errors that occur in the relationships between Gedcom records.
1. FAMC, FAMS, HUSB, WIFE and CHIL lines must point to other records in the same Gedcom file.
1. Every FAMC, FAMS, HUSB, WIFE and CHIL lines must have a return link from the INDI or FAM record that it points to.

#### DeadEndScript Errors

Run time errors can occur when running a DeadEnds script. When a run time error occurs the error is reported and execution of DeadEndsScript program ends immediately. The error must report the script's file name and line number where it occurred.

### Flow of Error Handling
1. When trying to open, read and write files errors can occur. If an error occurs then, log it, set flag to not load a database, and continue.
1. When reading Gedcom files any errors should be logged to be reported.
1. When validating a Gedcom file any errors should be logged to be reported.
1. When all files have been read and validated, if there were errors the program should quit while the user fixes the Gedcom.
