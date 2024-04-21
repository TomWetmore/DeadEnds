// DeadEnds
//
// name.c has the functions that deal with Gedcom names. Several functions return pointers to
// static memory. Callers beware.
//
// Created by Thomas Wetmore on 7 November 2022.
// Last changed on 21 April 2024.

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

// nameToNameKey converts a Gedcom name or partial name to a name key.
String nameToNameKey(String name) {
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

// getSurname returns the surname part of a Gedcom name.
#define NBUFFERS (4)
String getSurname(String name) {
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

// getFirstInitial returns the first initial of a Gedcom name.
int getFirstInitial(String name) {
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

// soundex returns the Soundex code of a surname.
String soundex(String name) {
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

// codeof returns a letter's Soundex code.
static int codeOf(int letter) {
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

// exactMatch checks if a partial name is contained within a complete name.
//  TODO: This function is not called used.
bool exactMatch(String partial, String complete) {
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

// pieceMatch matches a partial word with a complete word. They begin with the same letter.
// The letters in partial must be in same order as in complete. The letters case sensitive.
static bool pieceMatch (String partial, String complete) {
    if (*partial++ != *complete++) return false;
    while (*partial && *complete) {
        if (*partial == *complete++) partial++;
    }
    return *partial == 0;
}

// squeeze squeezes a string into a superstring, a string of uppercase, 0-terminated words,
// ending with another 0; non-letters not copied. 'Anna /Van Cott/' maps to 'ANNA\0VANCOTT\0\0'.
static void squeeze(String in, String out) {
    int c;
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

// personKeysFromName finds all persons with a name that matches a given name; returns an
// array of Strings with the record keys; pcount set to number of Strings.
String* personKeysFromName(String name, Database *database, int* pcount)
{
    static bool first = true;
	static Block recordKeys; // Dangerous state variable.
    if (first) {
		initBlock(&recordKeys);
        first = false;
    }
    Set *keySet = searchNameIndex(database->nameIndex, name); // Keys of persons who match.
    if (!keySet || lengthSet(keySet) == 0) return null;
    emptyBlock(&recordKeys, null);

	int count = 0;
	List* list = listOfSet(keySet);
	FORLIST(list, recordKey)
		GNode* person = keyToPerson((String) recordKey, database);
		for (GNode* node = NAME(person); node && eqstr(node->tag, "NAME"); node = node->sibling) {
			if (!compareNames(name, node->value)) continue;
			appendToBlock(&recordKeys, recordKey);
			count++;
			break;
		}
	ENDLIST
	*pcount = count;
    return (String*) recordKeys.elements;
}

//  compareNames -- Compare two Gedcom names. Return their relationship.
//--------------------------------------------------------------------------------------------------
int compareNames(String name1, String name2)
//  name1, name2 -- The two names to compare.
{
    char sqz1[MAXNAMELEN], sqz2[MAXNAMELEN];
    String p1 = sqz1,  p2 = sqz2;
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

// nameToList convert a name to a List of Strings; List must exist; uses static memory.
bool nameToList(String name, List *list, int *plen, int *psind)
// name	-- Gedcom name
// list	-- list (must exist)
// plen	-- returned length
// psind -- index (rel 1) of surname in list
{
	int i;
	String str;
	String parts[MAXPARTS];
	if (!name || *name == 0 || !list) return false;
	emptyList(list);
	*psind = 0;
	nameToParts(name, parts);
	for (i = 0; i < MAXPARTS; i++) {
		if (!parts[i]) break;
		if (*parts[i] == '/') {
			*psind = i + 1;
			str = strsave(parts[i] + 1);
			if (str[strlen(str) - 1] == '/')
				str[strlen(str) - 1] = 0;
		} else
			str = strsave(parts[i]);
		setListElement(list, str, i + 1);
	}
	*plen = i;
	ASSERT(*psind);
	return true;
}

// partsToName converts a list of name parts to a single String; uses static memeory.
static String partsToName(String* parts) {
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

// upsurname -- Make a Gedcom name have an all uppercase surname.
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

// manipulateName converts a Gedcom name to various formats. caps specifies the surname to upper
// case. If reg is false surname comes first followed by comma. len specifies the max length.
String manipulateName (String name, bool caps, bool reg, int len) {
    if (!name || *name == 0) return null;
    if (caps) name = upsurname(name);
    name = trimName(name, reg ? len: len-1);
    if (reg) return trim(nameString(name), len);
    return trim(nameSurnameFirst(name), len);
}

// nameString removes the slashes from a Gedcom name; uses static memory.
String nameString(String name) {
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

// nameSurnameFirst converts a Gedcom name to surname first form.
static String nameSurnameFirst(String name) {
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

