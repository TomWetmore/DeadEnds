//
//  DeadEnds
//
//  name.c -- Functions that deal with Gedcom names. Several functions return pointers to
//    static memory. Callers of those functions must be aware of the consequences.
//
//  Created by Thomas Wetmore on 7 November 2022.
//  Last changed on 16 November 2023.
//

#include "standard.h"
#include "name.h"
#include "gnode.h"
#include "nameindex.h"

static int old = 0;

// Static functions used in this file.
static int codeOf(int letter);
static String partsToName(String* parts);
static bool pieceMatch(String partial, String complete);
static void nameToParts(String name, String *parts);
static void squeeze(String string, String super);
static String nextPiece(String name);
static void cmpsqueeze (String in, String out);
//static String nameString (String);
static String nameSurnameFirst(String);

//  nameToNameKey - Convert Gedcom name or partial name to a name key. A name key is six
//    characters and consists of the name's first initial and the soundex of the surname.
//    MNOTE: This function returns the key in static data space.
//--------------------------------------------------------------------------------------------------
String nameToNameKey(String name)
//  name -- Gedcom name to convert to a name key.
{
    static char key[6];

    char finitial = getFirstInitial(name);
    String sdex = soundex(getSurname(name));
    key[0] = finitial;
    key[1] = *sdex++;
    key[2] = *sdex++;
    key[3] = *sdex++;
    key[4] = *sdex;
    key[5] = 0;
    return key;
}

//  getSurname -- Return the surname part of a Gedcom name. Returns the name in static data space.
//    MNOTE: This function returns the surname string in static memeory.
//--------------------------------------------------------------------------------------------------
#define NBUFFERS (4)
String getSurname(String name)
//  name -- String holding a Gedcom name.
{
    int c;

    static char buffer[NBUFFERS][MAXLINELEN+1];
    static int dex = 0;
    String p, surname;
    if (++dex > NBUFFERS-1) dex = 0;
    p = surname = buffer[dex];
    while ((c = *name++) && c != '/')
        ;
    if (c == 0) return "____";
    while (iswhite(c = *name++))
        ;
    if (c == 0 || c == '/' || !isLetter(c)) return "____";
    *p++ = c;
    while ((c = *name++) && c != '/')
        *p++ = c;
    *p = 0;
    striptrail(surname);
    return surname;
}

//  getFirstInitial -- Return the first initial of a Gedcom name.
//--------------------------------------------------------------------------------------------------
int getFirstInitial(String name)
//  name -- String holding a Gedcom name.
{
    int c;
    while (true) {
        while (iswhite(c = *name++))
            ;
        if (isLetter(c)) return toupper(c);
        if (c == 0) return '$';
        if (c != '/') return '$';
        while ((c = *name++) && c != '/')
            ;
        if (c == 0) return '$';
    }
}

//  soundex -- Return the Soundex code of a surname.
//    MNOTE: The soundex code is returned in static memory.
//--------------------------------------------------------------------------------------------------
String soundex(String name)
//  name -- Surname to find the Soundex code for.
{
    static char scratch[MAXNAMELEN];

    int c, j;
    if (!name || strlen(name) > MAXNAMELEN || !strcmp(name, "____"))
        return "Z999";
    String p = name;
    String q = scratch;
    while ((c = *p++))
        *q++ = toupper(c);
    *q = 0;
    p = q = &scratch[1];
    int i = 1;
    old = 0;
    while ((c = *p++) && i < 4) {
        if ((j = codeOf(c)) == 0) continue;
        *q++ = j;
        i++;
    }
    while (i < 4) {
        *q++ = '0';
        i++;
    }
    *q = 0;
    return scratch;
}

//  codeof -- Return a letter's Soundex code.
//--------------------------------------------------------------------------------------------------
static int codeOf(int letter)
//  letter -- A character from a surname.
{
    int new = 0;
    switch (letter) {
    case 'B': case 'P': case 'F': case 'V':
        new = '1'; break;
    case 'C': case 'S': case 'K': case 'G':
    case 'J': case 'Q': case 'X': case 'Z':
        new = '2'; break;
    case 'D': case 'T':
        new = '3'; break;
    case 'L':
        new = '4'; break;
    case 'M': case 'N':
        new = '5'; break;
    case 'R':
        new = '6'; break;
    default:    //  new stays zero
        break;
    }
    if (new == 0) {
        old = 0;
        return 0;
    }
    if (new == old) return 0;
    old = new;
    return new;
}

// remove_name -- Remove entry from name record
//--------------------------------------------------------------------------------------------------
bool remove_name (String name, String key)
// String name -- Person's name.
// String key -- Person indi key.
{
//    String rec, p;
//    int i, len, off;
//    bool found;
//    RKEY rkey;
//    rkey = str2rkey(key);
//    (void) getnamerec(name);
//    found = FALSE;
//    for (i = 0; i < NRcount; i++) {
//        if (!strncmp(rkey.r_rkey, NRkeys[i].r_rkey, 8) &&
//            !strcmp(name, NRnames[i])) {
//            found = true;
//            break;
//        }
//    }
//    if (!found) return FALSE;
//    NRcount--;
//    for ( ; i < NRcount; i++) {
//        NRkeys[i] = NRkeys[i+1];
//        NRnames[i] = NRnames[i+1];
//    }
//    p = rec = (String) stdalloc(NRsize);
//    len = 0;
//    memcpy(p, &NRcount, sizeof(int));
//    p += sizeof(int);
//    len += sizeof(int);
//    for (i = 0; i < NRcount; i++) {
//        memcpy(p, &NRkeys[i], sizeof(RKEY));
//        p += sizeof(RKEY);
//        len += sizeof(RKEY);
//    }
//    off = 0;
//    for (i = 0; i < NRcount; i++) {
//        memcpy(p, &off, sizeof(int));
//        p += sizeof(int);
//        len += sizeof(int);
//        off += strlen(NRnames[i]) + 1;
//    }
//    for (i = 0; i < NRcount; i++) {
//        memcpy(p, NRnames[i], strlen(NRnames[i]) + 1);
//        p += strlen(NRnames[i]) + 1;
//        len += strlen(NRnames[i]) + 1;
//    }
//    addrecord(BTR, NRkey, rec, len);
//    stdfree(rec);
    return true;
}

#if 0
//  replace_name -- Replace entry in name records.
//--------------------------------------------------------------------------------------------------
bool replace_name (String old, String new, String key)
String old;    /* person's old name */
String new;    /* person's new name */
String key;    /* person's INDI key */
{
    remove_name(old, key);
    add_name(new, key);
    return true;
}
#endif


//  exactMatch -- Check if a partial name is contained within a complete name.
//    TODO: This function is not called from anywhere.
//--------------------------------------------------------------------------------------------------
bool exactMatch(String partial, String complete)
//  partial -- Partial name.
//  complete -- Full Gedcom name.
{
    char part[MAXNAMELEN+2], comp[MAXNAMELEN+2], *p, *q;
    bool okay;

    if (strlen(partial) > MAXNAMELEN || strlen(complete) > MAXNAMELEN)
        return false;
    squeeze(partial, part);
    squeeze(complete, comp);
    q = comp;
    for (p = part; *p; p += strlen(p) + 1) {
        okay = false;
        for (; !okay && *q; q += strlen(q) + 1) {
            if (pieceMatch(p, q)) okay = true;
        }
        if (!okay) return false;
    }
    return true;
}

//  pieceMatch -- Match a partial word with a complete word. The partial word must be begin with
//    the same letter. The letters in partial must be in same order as in complete. The letters
//    are case sensitive. Return true if the partial word is contained in the complete word.
//--------------------------------------------------------------------------------------------------
static bool pieceMatch (String partial, String complete)
//  partial -- Partial word that may be contained within the complete word.
//  complete -- Complete word that may contain the partial word.
{
    if (*partial++ != *complete++) return false;
    while (*partial && *complete) {
        if (*partial == *complete++) partial++;
    }
    return *partial == 0;
}

//  squeeze -- Squeeze a string into a superstring, a string of uppercase, 0-terminated words,
//    ending with another 0; non-letters are not copied. For example, 'Anna /Van Cott/' maps to
//    'ANNA\0VANCOTT\0\0'.
//--------------------------------------------------------------------------------------------------
static void squeeze(String in, String out)
//  in -- String of words.
//  out -- Superstring of words.
{
    int c;
    //String out0 = out;
    while ((c = *in++) && chartype(c) != LETTER)
        ;
    if (c == 0) {
        *out++ = 0; *out = 0;
        return;
    }
    while (true) {
        *out++ = toupper(c);
        while ((c = *in++) && c != '/' && chartype(c) != WHITE) {
            if (chartype(c) == LETTER) *out++ = toupper(c);
        }
        if (c == 0) {
            *out++ = 0; *out = 0;
            return;
        }
        *out++ = 0;
        while ((c = *in++) && chartype(c) != LETTER)
            ;
        if (c == 0) {
            *out++ = 0; *out = 0;
            return;
        }
    }
}

//  personKeysFromName -- Find all persons with a name that matches the parameter name. Returns
//    the list of their keys.
//
//  TODO: THIS USED TO ALSO FIND PERSONS WHO MATCHED A KEY.
//--------------------------------------------------------------------------------------------------
String* personKeysFromName(String name, Database *database, int* pcount)
//  name -- Name to search for.
//  database -- Database.
//  pcount -- (out) Number of Persons with matching names.
{
    //  This function uses listOfKeys as a state variable that holds the most recent list of keys.
    //    Callers should copy this list if they need a persistent copy.
    static bool first = true;
    static int count = 0;
    static List listOfKeys;  //  NOTE: listOfKeys is not on the heap.

    ASSERT(name);

    // Create the state variables on the first call.
    if (first) {
        List *pListOfKeys = createList(null, null, null);
        memcpy(&listOfKeys, pListOfKeys, sizeof(List));
        stdfree(pListOfKeys);
        first = false;
    }

    //  Get the record keys of the persons with names that share the name key of the input name.
    //  The set of keys is in the index -- no memory obligations.
    Set *keySet = searchNameIndex(database->nameIndex, name);

    // If there are no keys there is nothing to do.
    if (!keySet || lengthSet(keySet) == 0) return null;

    // Empty the state variable list.
    deleteList(&listOfKeys);

    // Loop through the set of keys looking for persons with names that match the input name.
    List *list = keySet->list;
    String* keys = (String*) list->data;
    count = 0;
    for (int i = 0, n = list->length; i < n; i++) {
        GNode* person = keyToPerson(keys[i], database);
        for (GNode* node = NAME(person); node && eqstr(node->tag, "NAME"); node = node->sibling) {
            if (!compareNames(name, node->value)) continue;
            appendListElement(&listOfKeys, (Word) keys[i]);
            break;  // Don't care if other names of this person also match.
        }
    }
    *pcount = count;
    return (String*) listOfKeys.data;
}

//  compareNames -- Compare two Gedcom names. Return their relationship.
//--------------------------------------------------------------------------------------------------
int compareNames(String name1, String name2)
//  name1, name2 -- The two names to compare.
{
    char sqz1[MAXNAMELEN], sqz2[MAXNAMELEN];
    String p1 = sqz1, p2 = sqz2;
    int r = strcmp(getSurname(name1), getSurname(name2));
    if (r) return r;
    r = getFirstInitial(name1) - getFirstInitial(name2);
    if (r) return r;
    cmpsqueeze(name1, p1);
    cmpsqueeze(name2, p2);
    while (*p1 && *p2) {
        r = strcmp(p1, p2);
        if (r) return r;
        p1 += strlen(p1) + 1;
        p2 += strlen(p2) + 1;
    }
    if (*p1) return 1;
    if (*p2) return -1;
    return 0;
}

// cmpsqueeze -- Squeeze a Gedcom name to a superstring of given names.
//--------------------------------------------------------------------------------------------------
static void cmpsqueeze (String in, String out)
//  in, out --
{
    int c;
    while ((in = nextPiece(in))) {
        while (true) {
            c = *in++;
            if (iswhite(c) || c == '/' || c == 0) {
                *out++ = 0;
                --in;
                break;
            }
            *out++ = c;
        }
    }
    *out = 0;
}

//  givens -- Return the given names from a Gedcom format name.
//    TODO: I don't see how this ignores the surname. Test this thoroughly.
//--------------------------------------------------------------------------------------------------
String getGivenNames (String name)
//  name -- Name in Gedcom format.
{
    int c;
    // Buffer to hold the given names.
    static char scratch[MAXNAMELEN+1];
    String out = scratch;
    // Scan the Gedcom name for its 'pieces'.
    while ((name = nextPiece(name))) {  // Get the next piece of the Gedcom name.
        while (true) {
            // If have reached the end of the Gedcom name.
            if ((c = *name++) == 0) {
                // If the last character in the out buffer is a space backup a character.
                if (*(out - 1) == ' ') --out;
                // Add null at the end of the out buffer and return it.
                *out = 0;
                return scratch;
            }
            // If found white space or a slash, add a space to the out buffer.
            if (iswhite(c) || c == '/') {
                *out++ = ' ';
                // Backup one character in the name string and break.
                --name;
                break;
            }
            // Otherwise add the current character to the out buffer and continue.
            *out++ = c;
        }
    }
    if (*(out - 1) == ' ') --out;
    *out = 0;
    return scratch;
}

// nextPiece -- Return next word in a Gedcom name string that is not the surname.
//--------------------------------------------------------------------------------------------------
static String nextPiece (String in)
// String in -- String holding a shrinking Gedcom name. Called multiple times. On each call the
// previous part has been 'removed'.
{
    int c;
    while (true) {  // Character scanning loop.
        // Move by white space.
        while (iswhite(c = *in++)) ;
        // If at the end of string scanning is done so return null.
        if (c == 0) return null;
        // If the character is not '/' return current location in String.
        if (c != '/') return --in;
        // When the first slash is found read by characters to the last slash.
        while ((c = *in++) && c != '/') ;
        // If at end of string before finding second slash return null.
        if (c == 0) return null;
    }
}

//  trimName -- Trim a Gedcom name to be less or equal to a given length but not shorter than
//    the first initial and surname.
//--------------------------------------------------------------------------------------------------
#define MAXPARTS 100
String trimName(String name, int len)
//  name -- Gedcom name string to be trimmed.
//  len -- Desired maximum final length for the returned string.
{
    String parts[MAXPARTS];
    int i, sdex = -1, nparts;
    nameToParts(name, parts);
    name = partsToName(parts);
    if (strlen(name) <= len + 2) return name;
    for (i = 0; i < MAXPARTS; i++) {
        if (!parts[i]) break;
        if (*parts[i] == '/') sdex = i;
    }
    nparts = i;
    ASSERT(sdex != -1);
    for (i = sdex-1; i >= 0; --i) {
        *(parts[i] + 1) = 0;
        name = partsToName(parts);
        if (strlen(name) <= len + 2) return name;
    }
    for (i = sdex-1; i >= 1; --i) {
        parts[i] = null;
        name = partsToName(parts);
        if (strlen(name) <= len + 2) return name;
    }
    for (i = nparts-1; i > sdex; --i) {
        parts[i] = null;
        name = partsToName(parts);
        if (strlen(name) <= len + 2) return name;
    }
    return name;
}

// nameToParts -- Convert Gedcom name to parts; keep slashes.
//--------------------------------------------------------------------------------------------------
static void nameToParts(String name, String* parts)
//  name -- Gedcom name.
//  parts --
{
    static char scratch[MAXNAMELEN+1];
    String p = scratch;
    int c, i = 0;
    ASSERT(strlen(name) <= MAXNAMELEN);
    for (i = 0; i < MAXPARTS; i++)
        parts[i] = null;
    i = 0;
    while (true) {
        while (iswhite(c = *name++))
            ;
        if (c == 0) return;
        ASSERT(i < MAXPARTS);
        parts[i++] = p;
        *p++ = c;
        if (c == '/') {
            while ((c = *p++ = *name++) && c != '/')
                ;
            if (c == 0) return;
            *p++ = 0;
        } else {
            while ((c = *name++) && !iswhite(c) && c != '/')
                *p++ = c;
            *p++ = 0;
            if (c == 0) return;
            if (c == '/') name--;
        }
    }
}

//  partsToName -- Convert a list of name parts back to a string.
//    MNOTE: The returned string is in static memory.
//--------------------------------------------------------------------------------------------------
static String partsToName(String* parts)
//  parts -- Array of strings representing a name.
{
    int i;
    static char scratch[MAXNAMELEN+1];
    String p = scratch;
    for (i = 0; i < MAXPARTS; i++) {
        if (!parts[i]) continue;
        strcpy(p, parts[i]);
        p += strlen(parts[i]);
        *p++ = ' ';
    }
    if (*(p - 1) == ' ')
        *(p - 1) = 0;
    else
        *p = 0;
    return scratch;
}

//  upsurname -- Make a Gedcom name have an all uppercase surname.
//    MNOTE: The returned string is in static memory.
//--------------------------------------------------------------------------------------------------
String upsurname(String name)
//  name -- Gedcom name (with surname between slashes).
{
    static char scratch[MAXNAMELEN+1];
    String p = scratch;
    int c;
    while ((c = *p++ = *name++) && c != '/') ;
    if (c == 0) return scratch;
    while ((c = *name++) && c != '/') *p++ = toupper(c);
    *p++ = c;
    if (c == 0) return scratch;
    while ((c = *p++ = *name++)) ;
    return scratch;
}

//  manipulateName - Converts a Gedcom name to various formats. The second parameter specifies the
//    surname to be in upper case. The third specifies that the surname to be first in the string
//    and separated from the givens by a comma. The fourth parameter specifies the max length of
//    the output string.
//--------------------------------------------------------------------------------------------------
String manipulateName (String name, bool caps, bool reg, int len)
//  name -- Gedcom name.
//  caps -- Surname in capitals?
//  reg -- Regular order (surname where it is found)?
//  len -- Max length of the output string.
{
    if (!name || *name == 0) return null;
    if (caps) name = upsurname(name);
    name = trimName(name, reg ? len: len-1);
    if (reg) return trim(nameString(name), len);
    return trim(nameSurnameFirst(name), len);
}

//  nameString -- Removes the slashes from a Gedcom name.
//    MNOTE: The returned string is in static memory.
//--------------------------------------------------------------------------------------------------
String nameString(String name)
//  name -- Gedcom format name.
{
    static char scratch[MAXNAMELEN+1];
    String p = scratch;
    ASSERT(strlen(name) <= MAXNAMELEN);
    while (*name) {
        if (*name != '/') *p++ = *name;
        name++;
    }
    *p-- = 0;
    striptrail(scratch);
    return scratch;
}

//  nameSurnameFirst - Convert a Gedcom name to surname first form.
//    MNOTE: The returned name is in static memory.
//--------------------------------------------------------------------------------------------------
static String nameSurnameFirst(String name)
//  name -- Gedcom format name.
{
    static char scratch[MAXNAMELEN+1];
    String p = scratch;
    ASSERT(strlen(name) <= MAXNAMELEN);
    strcpy(p, getSurname(name));
    p += strlen(p);
    strcpy(p, ", ");
    p += strlen(p);
    strcpy(p, getGivenNames(name));
    return scratch;
}

