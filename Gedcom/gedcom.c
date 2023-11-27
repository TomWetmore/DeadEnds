//
//  DeadEnds
//
//  gedcom.c
//
//  Created by Thomas Wetmore on 29 November 2022.
//  Last changed on 11 October 2023.
//

#include "gedcom.h"

RecordType recordType(GNode *root)
{
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

//  compareRecordKeys -- Compare function to use in Sets and Lists holding Gedcom record keys.
//   Longer keys sort after shorter keys.
//--------------------------------------------------------------------------------------------------
int compareRecordKeys(String a, String b)
{
    ASSERT(strlen(a) > 1 && strlen(b) > 1);  // Is this strictly necessary?
    if (strlen(a) != strlen(b)) return (int) (strlen(a) - strlen(b));
    for (int i = 0; i < strlen(a); i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}
