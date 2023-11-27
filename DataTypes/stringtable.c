//
//  DeadEnds
//
//  stringtable.c -- Functions for a string table, which is a hash table whose keys and values
//    are strings.
//
//  Created by Thomas Wetmore on 23 April 2023.
//  Last changed on 6 July 2023.
//

#include "standard.h"
#include "stringtable.h"

// stringGetKey -- The get key function for string tables.
//--------------------------------------------------------------------------------------------------
static String stringGetKey(Word element) {
    return ((StringElement*) element)->key;
}

//  compareStringEls -- The compare function for string tables.
//--------------------------------------------------------------------------------------------------
static int compareStringEls(Word leftEl, Word rightEl)
{
    String a = ((StringElement*) leftEl)->key;
    String b = ((StringElement*) rightEl)->key;
    return strcmp(a, b);
}

//  deleteStringEl -- The delete function for string tables.
//  MNOTE: Key and value are assumed to not be referenced from other locations, so the two
//    strings are freed before the element is freed.
//--------------------------------------------------------------------------------------------------
static void deleteStringEl(Word word)
{
    StringElement *element = (StringElement*) word;
    stdfree(element->key);
    if (element->value) stdfree(element->value);
    stdfree(element);
}

//  createStringTable -- Create a string table.
//--------------------------------------------------------------------------------------------------
HashTable *createStringTable(void)
{
    return createHashTable(compareStringEls, deleteStringEl, stringGetKey);
}

//  searchStringTable -- Search a string table for the element with the given key. If found return
//    the element's value; otherwise return null.
//--------------------------------------------------------------------------------------------------
String searchStringTable(StringTable *table, String key)
{
    StringElement *element = (StringElement*) searchHashTable(table, key);
    return element ? element->value : null;
}

//  isInStringTable -- See if a string is in a string table.
//--------------------------------------------------------------------------------------------------
bool isInStringTable(StringTable *table, String key)
{
    return searchHashTable(table, key) != null;
}

//  insertInStringTable -- Insert a (key, value) pair into a hash table. If the values are null,
//    the string table behaves like a set of strings.
//--------------------------------------------------------------------------------------------------
void insertInStringTable(HashTable *table, String key, String value)
{
    //  See if the key is in the table; remove the old element if it is.
    StringElement *element = (StringElement*) searchHashTable(table, key);
    if (element) removeFromHashTable(table, key);

    // Create a new element to add to the table; they must have copies of the string/s.
    element = (StringElement*) stdalloc(sizeof(StringElement));
    element->key = strsave(key);
    element->value = value ? strsave(value) : null;

    // Add the new element to the table.
    insertInHashTable(table, (Word) element);
}

//  fixString -- Add an element to a string table that maps a string to itself. The key and the
//    value in the element will point to different strings.
//--------------------------------------------------------------------------------------------------
String fixString(StringTable *table, String string)
//  table -- String table that maintains a single copy of strings.
//  string -- String to be saved.
{
    ASSERT(table && string);

    // If the string is in the table, return the copy that is there.
    String saved = searchStringTable(table, string);
    if (saved) return saved;

    // If the tag is not in the table add a new entry to the table and return it.
    insertInStringTable(table, string, string);
    return searchStringTable(table, string);
}

//  showStringTable -- Show the contents of a string table. For debugging. Could have used
//    the generic showHashTable with a function parameter. Maybe change later.
//--------------------------------------------------------------------------------------------------
void showStringTable(HashTable *table)
{
    ASSERT(table);
    printf("String Table at Location %p\n", table);
    for (int i = 0; i < MAX_HASH; i++) {
        Bucket *bucket = table->buckets[i];
        if (!bucket || bucket->length <= 0) continue;
        printf("Bucket %d\n", i);
        for (int j = 0; j < bucket->length; j++) {
            StringElement *element = bucket->elements[j];
            printf("    %s -> %s\n", element->key, element->value ? element-> value : "null");
        }
    }
}
