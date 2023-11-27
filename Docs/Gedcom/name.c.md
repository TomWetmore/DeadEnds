# name.c
|Component|Description|
|:---|:---|
|String nameToNameKey(String name)|Convert Gedcom name or partial name to a name key. A name key is six characters and consists of the name's first initial and the soundex of the surname. This function returns the key in static data space.|
|String getSurname(String name)|Return the surname part of a Gedcom name. Returns the name in static data space.|
|int getFirstInitial(String name)|Return the first initial of a Gedcom name.|
|String soundex(String name)|Return the Soundex code of a surname. The code is returned in static memory.|
|static int codeOf(int letter)|Return a letter's Soundex code.|
|bool remove_name (String name, String key)|Remove entry from name record. *Not implemented.*|
|bool replace_name (String old, String new, String key)|Replace entry in name records. *Not implemented*.|
|bool exactMatch(String partial, String complete)|Check if a partial name is contained within a complete name. *Might not be called from anywhere yet*.|
|static bool pieceMatch (String partial, String complete)|Match a partial word with a complete word. The partial word must be begin with the same letter. The letters in partial must be in same order as in complete. The letters are case sensitive. Return true if the partial word is contained in the complete word. *Aren't the letters NOT case sensitive*?|
|static void squeeze(String in, String out)|Squeeze a string into a superstring, a string of uppercase, 0-terminated words, ending with another 0; non-letters are not copied. For example, 'Anna /Van Cott/' maps to 'ANNA\0VANCOTT\0\0'.|
|String* personKeysFromName(String name, NameIndex *index, int* pcount)|Find all persons with a name that matches the parameter name. Return the list of their keys.|
|int compareNames(String name1, String name2)|Compare two Gedcom names. Return their relationship.|
|static void cmpsqueeze (String in, String out)|Squeeze a Gedcom name to a superstring of given names.|
|String getGivenNames (String name)|Return the given names from a Gedcom format name. *I don't see how this ignores the surname. Test this thoroughly*.|
|static String nextPiece (String in)|Return next word in a Gedcom name string that is not the surname.|
|String trimName(String name, int len)|Trim a Gedcom name to be less or equal to a given length but not shorter than the first initial and surname.|
|static void nameToParts(String name, String* parts)|Convert Gedcom name to parts; keep slashes.|
|static String partsToName(String* parts)|Convert a list of name parts back to a string. The returned string is in static memory.|
|String upsurname(String name)|Make a Gedcom name have an all uppercase surname. The returned string is in static memory.|
|String manipulateName (String name, bool caps, bool reg, int len)|Converts a Gedcom name to various formats. The second parameter specifies the surname to be in upper case. The third specifies that the surname to be first in the string and separated from the givens by a comma. The fourth parameter specifies the max length of the output string.|
|String nameString(String name)|Removes the slashes from a Gedcom name. The returned string is in static memory.|
|static String nameSurnameFirst(String name)|Convert a Gedcom name to surname first form. The returned name is in static memory.|