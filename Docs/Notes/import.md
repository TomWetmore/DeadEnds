# Importing

Importing in DeadEnds consists of reading the records in a Gedcom file into an in-Ram database. The records are checked for syntax and lineage semantics. Person records are indexed by name. The records are also indexed for any REFN values.

###### importFromFiles

Importing starts by calling either the *importFromFiles* functions if there are more than one Gedcom file to be read, or *importFromFile* if there is a single file to read. *importFromFiles* calls *importFromFile* for each file in a list.

###### importFromFile

*importFromFile* reads a Gedcom file and builds a DeadEnds in-RAM database with its records. It determines the file's name and opens it. It then *getNodeListFromFile* to read every Gedcom line into a list of GNodes. It then calls *getNodeTreesFromNodeList* get the list of all the Gedcom records as GNode roots. Once the list of roots is created, it calls *createDatabase* and then calls *storeRecord* to add the records to the database. The GNode trees are normalized into a standard order as they are added. The line numbers where the records began are also stored. When finished storing records, two lists in the database, *personRoots* and *familyRoots* are sorted by key.

###### Database

A DeadEnds database is complsed of the following fields:

1. *filePath*, *lastSegment*&mdash; path to Gedcom file, and last segment of path used as datbase name
3. *personIndex*, *familyIndex* &mdash; maps from person and family keys to person and family roots.
5. *sourceIndex*, *eventIndex*, *otherIndex* &mdash; ditto.
6. *nameIndex* &mdash; map from name keys to lists of person record keys.
7. *refnIndex* &mdash; map from refn values to record keys.
8. *personRoots*, *familyRoots* &mdash; lists of all person and family roots in database, sorted by key.

When a database is created these are created.

###### storeRecord

*storeRecord* is called once for each record, adding it the proper database index. It must determine  the type of record. Header and trailer are ignored, though informating from the header record likely should also be kept in the database. Duplicate key checks are done. If the record is a person, it is added to the *personIndex* and *personRoots*. If a family it is added to the *familyIndex* and *familyRoots*. If a source, event or other it is added to the *sourceIndex*, *eventIndex* or *otherIndex*.

###### Validation

Now when *importFromFile* returns a database, no other operations are done.  Validation, name and reference indexing are not done. The code to do these operations exists, but are yet to be called. When *importFromFile* returns, if no errors were found, the database holds the records read from the file, and some of the database's maps and lists are initialized. However, semantic checks have not been done. For example a person might link to a family that does not link back to it. Also the name index and the reference index have not been done.

###### Indexing Names

A DeadEnds database must be indexed in various ways to facilitate access to the records. The name index for persons is the most important. Gedcom names are mapped to name keys that combine the first initial of the first given name and the soundex code of the surname. In the name index each name key maps to the set of person record keys that have names that match the name key. When a search for a name starts the the name key of the name is formed. Then the set of persons with names matching the key is gotten from the name index. Names with the same name key may not actually match, so each person record with a matching name key is checked for a name that match the search name. The records with names that match are returned from the index.

###### Indexing Reference Values

All Gedcom records (except header and trailer) have keys; they are the values of the level 0 root nodes.The user does not control these keys. However Gedcom allows records to have 1 REFN \<value\> lines that let users to assign their own keys to records. The reference index maps these user-assigned keys to the key of the record using the reference key. This allows users to easily access records using their own keys.

###### More Indexing

The LifeLines code that DeadEnds comes from uses only name and reference indexing. It would be very useful, however, if records could also be indexed by place and date values. I often thought about adding these indexes for LifeLines but is never happened.










