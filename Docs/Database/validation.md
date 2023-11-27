# Validation Stack
This section describes the process that validates a *DeadEnds* databases. Databases are created by the *read stack* described in ADD A LINK.

If no errors were found in the Gedcom syntax, or in  other single record checks done by the *read stack*, there will be a database of all the records from the file. These records must be further validated, because no inter-record checks were done by the *read stack*. For example, the database must be *closed* &mdash; all families that persons refer to, and all persons that families refer to, must exist in the database. When a person has a FAMC link to a family he or she is a child in, that family must also have a single CHIL link back to the person. Likewise for spouses.

## Validation Stack
### bool validateDatabase(Database \*database, ErrorLog *errorLog)
*validateDatabase* is the top layer of the validation stack. It is passed the database to validate and the error log to post errors to. It calls functions that validate different regions of the database. These functions are *validatePersonIndex*, *validateFamilyIndex*, *validateSourceIndex*, *validateEventIndex* and *validateOtherIndex*.
### bool validatePersonIndex(Database \*database, ErrorLog *errorLog)
*validatePersonIndex* validates all the persons in the database. The persons have all been added to the person index.

### static void validatePerson(GNode *person)
*validatePerson*