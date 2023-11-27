# Files and File Names (and Line Numbers)

DeadEnds reads files. Specifically it reads Gedcom files to build databases and it read DeadEndScript files to be run agains tht databases.

## Gedcom Files

File names get into the system so that DeadEnds can read them and create databases. Any error messages that are created during the phases when the files are being read must contain the name fo the file. Later in life it seems reasonable that any record in any database should be able to know the name of the file it came from.

There is an  issue that arises. Files have potentially long path names, and we we tend to think of the file's name as being the last component of the path. It is this last component of the path that should be the name of the file in the error messages (or should it?).

One can certainly imagine the case where multiple files have the same last component. Is this something that must be worried about?

### How Does It Work Now?

I guess we can start here:

```
Database *importFromFile(String filePath, ErrorLog*);
```
*filePath* is passed to *fopen* to open. It can be an absolute or relative path. The last component is the name of the file and assigned to the *fileName* automatic.

#### What Does *importFromFile* Do?

*importFromFile* trys to open the file, adding an *Error* to the *ErrorLog* and returning if it cannot. If it opens the file it extracts the *fileName* from the *filePath* and calls *createDatabase* passing in *fileName* to become a field in the *Database* structure.

*importFromFile* then calls *firstNodeTreeFromFile* to read the first record, and then repeatedly calls *nextNodeTreeFromFile* to read the remaining records. After each record is read *importFromFile* calls *storeRecord* to add the record to the *Database*.
