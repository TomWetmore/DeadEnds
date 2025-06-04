//
//  DeadEnds Library
//
//  name.h is the header file for the Gedcom name functions.
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changed on 4 June 2025.
//

#ifndef name_h
#define name_h

typedef struct Database Database;
typedef struct List List;
typedef HashTable NameIndex;

// Some functions use static dataspace to construct names. MAXNAMELEN is the maximum length.
#define MAXNAMELEN 512

// User interface to name functions.
String manipulateName(String, bool caps, bool reg, int maxlen); // Manipulate a name.
String getSurname(String); // Get the surname of a Gedcom name.
String getGivenNames(String); // Get the given names of a Gedcom name.
int getFirstInitial(String name); // Get the first initial of a Gedcom name.
String soundex(String surname); // Get the Soundex code of a Gedcom surname.
String nameToNameKey(String name); // Convert a partial or full Gedcom name to a name key.
int compareNames(String name1, String name2); // Compare two Gedcom names.
String* personKeysFromName(String name, RecordIndex*, NameIndex*, int* pcount);
String nameString(String name); // Remove slashes from a name.
String trimName (String name, int len); // Trim name to specific length.
bool nameToList(String name, List*, int *len, int *sind);

#endif // name_h
