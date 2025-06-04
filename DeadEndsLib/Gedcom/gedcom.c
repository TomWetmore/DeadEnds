//
// DeadEnds Library
//
//  gedcom.c has basic Gedcom functions.
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 4 June 2025.
//

#include "gedcom.h"
#include "gnode.h"

// recordType returns the type of a Gedcom record.
RecordType recordType(GNode* root) {
    ASSERT(root);
    String tag = root->tag;
    if (eqstr(tag, "INDI")) return GRPerson;
    if (eqstr(tag, "FAM"))  return GRFamily;
    if (eqstr(tag, "SOUR")) return GRSource;
    if (eqstr(tag, "EVEN")) return GREvent;
    if (eqstr(tag, "HEAD")) return GRHeader;
    if (eqstr(tag, "TRLR")) return GRTrailer;
    return GROther;
}

//  compareRecordKeys compares record keys; longer keys sort after shorter keys.
int compareRecordKeys(String a, String b) {
    ASSERT(strlen(a) > 1 && strlen(b) > 1);  // Is this strictly necessary?
    if (strlen(a) != strlen(b)) return (int) (strlen(a) - strlen(b));
    for (int i = 0; i < strlen(a); i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

// sexTypeToString returns the Gedcom character for a SexType.
String sexTypeToString(SexType sex) {
	if (sex == sexMale) return "M";
	if (sex == sexFemale) return "F";
	if (sex == sexUnknown) return "U";
	return "X";
}

// sexTypeFromString returns the SexType of a String.
SexType sexTypeFromString(String s) {
	if (eqstr("M", s)) return sexMale;
	if (eqstr("F", s)) return sexFemale;
	if (eqstr("U", s)) return sexUnknown;
	return sexError;
}

// validSexString validates a sex String value to be M, F, or U.
bool validSexString(String s) {
	if (!s) return false;
	return sexTypeFromString(s) != sexError;
}

// keyToKey takes a "lazy" key (may omit @-signs and have lower case letters), and converts it to a real key.
// NOTE: Returns static memory form the upper function
String keyToKey(String userKey) {
    static char buffer[MAXSTRINGSIZE];
    if (strlen(userKey) > MAXSTRINGSIZE - 2) return userKey;
    if (userKey[0] != '@') {
        buffer[0] = '@';
        buffer[1] = 0;
        strcat(buffer, userKey);
        strcat(buffer, "@");
    } else {
        strcpy(buffer, userKey);
    }
    return upper(buffer);
}
