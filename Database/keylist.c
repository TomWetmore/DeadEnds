//  DeadEnds
//
//  keylist.c -- Extends List for handling a list of keys in a Database.
//
//  Created by Thomas Wetmore on 15 February 2024.
//  Last changed by Thomas Wetmore on 20 February 2024.
//

#include <stdio.h>
#include "keylist.h"
#include "database.h"
#include "name.h"

//  keyCompare is the compare function for Lists whose elements are Gedcom record keys.
//--------------------------------------------------------------------------------------------------
static int keyCompare(Word a, Word b)
{
	String keya = (String) a;
	String keyb = (String) b;
	// (This duplicates code from sequence.c. Should be cleaned up.)
	ASSERT(strlen(keya) > 2 && strlen(keyb) > 2);
	if (strlen(keya) != strlen(keyb)) return (int) (strlen(a) - strlen(b));
	for (int i = 0; i < strlen(keya); i++) {
		if (keya[i] != keyb[i]) return keya[i] - keyb[i];
	}
	return 0;
}

//  keyGet is the get key function for Lists whose elements are Gedcom record keys.
//--------------------------------------------------------------------------------------------------
static String keyGet(Word a)
{
	return (String) a;
}

// rootCompare is the compare function for Lists whose elments are Gedcom record GNode roots.
//--------------------------------------------------------------------------------------------------
static int rootKeyCompare(Word a, Word b) {
	String keya = ((GNode*) a)->key;
	String keyb = ((GNode*) b)->key;
	// (This duplicates code from sequence.c. Should be cleaned up.)
	ASSERT(strlen(keya) > 1 && strlen(keyb) > 1);
	if (strlen(keya) != strlen(keyb)) return (int) (strlen(a) - strlen(b));
	for (int i = 0; i < strlen(keya); i++) {
		if (keya[i] != keyb[i]) return keya[i] - keyb[i];
	}
	return 0;
}

//  rootKeyGet is the get key function for Lists whose elements are Gedcom record GNode roots.
//--------------------------------------------------------------------------------------------------
static String rootKeyGet(Word a)
{
	return ((GNode*) a)->key;
}

//  rootNameCompare is a compare function for Lists whose elements are person GNode roots and
//    the Lists are to be sorted by the persons' names.
//--------------------------------------------------------------------------------------------------
static int rootNameCompare(Word a, Word b)
{
	GNode *roota = (GNode*) a;
	GNode *rootb = (GNode*) b;
	ASSERT(eqstr(roota->tag, "INDI") && eqstr(rootb->tag, "INDI"));
	String namea = NAME(roota)->value;
	String nameb = NAME(rootb)->value;
	return compareNames(namea, nameb);
}

//  rootNameGet is the get function for Lists whose elements are person GNode roots and the
//    Lists are to be sorted by the persons' names.
//--------------------------------------------------------------------------------------------------
static String rootNameGet(Word a)
{
	GNode *root = (GNode*) a;
	return NAME(root)->value;
}

//  createKeyList creates and returns a new KeyList for holding Gedcom record keys.
//--------------------------------------------------------------------------------------------------
KeyList *createKeyList(Database *database)
{
	return createList(keyCompare, null, keyGet);
}

//  insertInKeyList appends a new Gedcom record key into a KeyList.
//--------------------------------------------------------------------------------------------------
void insertInKeyList(KeyList *list, String key)
{
	appendListElement(list, (Word) key);
}

//  createRootList creates and returns a new RootList for holding Gedcom record root GNodes.
//--------------------------------------------------------------------------------------------------
RootList *createRootList(void)
{
	return createList(rootKeyCompare, null, keyGet);
}

//  insertInRootList appends a new Gedcom record root Gnode to a RootList.
//--------------------------------------------------------------------------------------------------
void insertInRootList(RootList *list, GNode *gnode)
{
	appendListElement(list, (Word) gnode);
}

//  createRootNameList creates and returns a List of person root GNodes sorted by the names of the
//    persons.
//--------------------------------------------------------------------------------------------------
RootList *createRootNameList(void)
{
	return createList(rootNameCompare, null, rootNameGet);
}

//  insertInRootNameList appends a new Gedcom person record root GNode to a RootList.
//--------------------------------------------------------------------------------------------------
void insertInRootNameList(RootList *list, GNode *root)
{
	appendListElement(list, (Word) root);
}
