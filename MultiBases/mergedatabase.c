// DeadEnds
// mergedatabase.c has functions to merge Databases.
//
// Created by Thomas Wetmore on 17 November 2024
// Last changed on 17 November 2024.


// mergeDatabase merges Database two into Database one.
void mergeDatabase(Database* one, Database* two) {

}


/*
HOW SHOULD IT WORK?

 1. starting with valid databases, so each database is closed and consistent.
 2. worry first about persons and families.
 3. pretend that database redesign is out of the picture.
 4. keep all the keys associated with the first database (so change keys in the second when needed).
 5. when adding a record to the first database, make a complete copy so the second database can be
    deleted if so desired.



 */
