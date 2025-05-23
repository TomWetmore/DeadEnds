//
//  DeadEnds
//
//  stringtable.c holds the StringTable data type, a HashTable whose keys and values are Strings.
//
//  Created by Thomas Wetmore on 23 April 2023.
//  Last changed on 20 May 2025.
//

#include "standard.h"
#include "stringtable.h"

// getKey is the getKey function for StringTable.
static String getKey(void *element) {
	return ((StringElement*) element)->key;
}

// compare is the compare function for StringTable.
static int compare(String a, String b) {
	return strcmp(a, b);
}

// delete is the delete function for StringTable; key and value are both freed.
static void delete(void* word) {
	StringElement *element = (StringElement*) word;
	free(element->key);
	if (element->value) free(element->value);
	free(element);
}

// createStringTable creates and returns a StringTable.
StringTable *createStringTable(int numBuckets) {
	return createHashTable(getKey, compare, delete, numBuckets);
}

// searchStringTable searches a StringTable for the element with given key.
String searchStringTable(StringTable *table, String key) {
	StringElement *element = (StringElement*) searchHashTable(table, key);
	return element ? element->value : null;
}

// isInStringTable returns whether the StringTable has an element with the given key.
bool isInStringTable(StringTable *table, String key) {
	return searchHashTable(table, key) != null;
}

// addToStringTable creates a new StringElement from key and value, and adds it to the table.
void addToStringTable(HashTable *table, String key, String value) {
	StringElement *element = (StringElement*) searchHashTable(table, key);
	if (element) removeFromHashTable(table, key); // If in table remove element before replacing.
	element = (StringElement*) stdalloc(sizeof(StringElement));
	element->key = strsave(key);
	element->value = value ? strsave(value) : null;
	addToHashTable(table, element, false);
}

// fixString adds an element to a StringTable that maps a String to itself; key and value
// point to different Strings.
String fixString(StringTable *table, String string) {
	String saved = searchStringTable(table, string);
	if (saved) return saved;
	addToStringTable(table, string, string);
	return searchStringTable(table, string);
}

// show is the private function that shows StringElement in the showStringTable function.
static void show(void* element) {
	StringElement* el = (StringElement*) element;
	printf("  %s -> %s\n", el->key, el->value ? el-> value : "null");
}

// showStringTable shows the contents of a string table.
void showStringTable(HashTable *table) {
	showHashTable(table, show);
}
