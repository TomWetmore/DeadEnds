// DeadEnds
//
// testsequence.c has code to test the Sequence data type.
//
// Created by Thomas Wetmore on 2 May 2024.
// Last changed on 2 May 2024.

#include "sequence.h"
#include "utils.h"

static void checkTest(String, int, int);
static Sequence* tomsAncestors(Database*);
static Sequence* lusAncestors(Database*);

// testSequence is the starting function to test the Sequence type.
void testSequence(Database* database, int testNumber) {
	printf("%d: TEST SEQUENCE: %2.3f\n", testNumber, getMilliseconds());

	// Create a Sequence.
	Sequence* sequence = createSequence(database);
	int length = lengthSequence(sequence);
	checkTest("Sequence should be empty", 0, length);

	// Add an element to a Sequence.
	appendToSequence(sequence, "@I1@", null);
	length = lengthSequence(sequence);
	checkTest("Sequence should have one element", 1, length);
	showSequence(sequence);

	// Test emptySequence.
	emptySequence(sequence);
	length = lengthSequence(sequence);
	checkTest("Empty sequence", 0, length);

	// Test copySequence.
	appendToSequence(sequence, "@I1@", null);
	length = lengthSequence(sequence);
	checkTest("Added element to emptied sequence", 1, length);
	Sequence* copied = copySequence(sequence);
	length = lengthSequence(copied);
	checkTest("Copied Sequence should have 1 element", 1, length);
	showSequence(sequence);
	showSequence(copied);

	// Test isInSequence.
	appendToSequence(sequence, "@I2@", null);
	showSequence(sequence);
	bool isIn = isInSequence(sequence, "@I1@");
	checkTest("I1 should be in", isIn ? 1 : 0, 1);
	isIn = isInSequence(sequence, "@I2@");
	checkTest("I2 should be in", isIn ? 1 : 0, 1);
	isIn = isInSequence(sequence, "@I3@");
	checkTest("I3 should not be in", isIn ? 1 : 0, 0);

	// Test removeFromSequence.
	bool removed = removeFromSequence(sequence, "@I1@");
	checkTest("I1 should be removed", removed ? 1 : 0, 1);
	showSequence(sequence);

	// Test keySortSequence.
	appendToSequence(sequence, "@I1@", null);
	appendToSequence(sequence, "@I6@", null);
	appendToSequence(sequence, "@I3@", null);
	appendToSequence(sequence, "@I5@", null);
	appendToSequence(sequence, "@I4@", null);
	showSequence(sequence);
	printf("Calling keySortSequence\n");
	keySortSequence(sequence);
	showSequence(sequence);

	// Test nameSortSequence.
	printf("Calling nameSortSequence\n");
	nameSortSequence(sequence);
	showSequence(sequence);

	// Test ancestorSequence.
	printf("Testing ancestorSequence\n");
	Sequence* ancestors = tomsAncestors(database);
	showSequence(ancestors);
	printf("Sort ancestors by key\n");
	keySortSequence(ancestors);
	showSequence(ancestors);
	printf("Sort ancestors by name\n");
	nameSortSequence(ancestors);
	showSequence(ancestors);
	//exit(0);

	// Test uniqueSequence.
	printf("Setting up to test uniqueSequence\n");
	emptySequence(sequence);
	emptySequence(ancestors);
	appendToSequence(sequence, "@I2@", null);
	ancestors = ancestorSequence(sequence);
	printf("THIS SHOULD BE LU'S ANCESTORS\n");
	showSequence(ancestors);
	emptySequence(copied);
	copied = copySequence(ancestors);
	printf("THIS SOULD BE A COPY OF LU'S ANCESTORS\n");
	showSequence(copied);
	printf("THIS SHOULD BE A SEQUENCE WITH ALL OF LU'S ANCESTORS TWICE\n");
	appendSequenceToSequence(ancestors, copied);
	showSequence(ancestors);
	printf("Now doing the uniqueing\n");
	Sequence* uniqued = uniqueSequence(ancestors);
	showSequence(uniqued);
	printf("Now doing the uniqueing in place -- first three copies\n");
	appendSequenceToSequence(uniqued, ancestors);
	showSequence(uniqued);
	uniqueSequenceInPlace(uniqued);
	printf("And now one copy\n");
	showSequence(uniqued);

	// Test personToChildren
	printf("Testing personToChildren, personToFathers, personToMothers\n");
	GNode* tom = getRecord("@I1@", database);
	Sequence* kids = personToChildren(tom, database);
	showSequence(kids);
	deleteSequence(kids);
	// Test personToFathers, personToMothers
	Sequence* fathers = personToFathers(tom, database);
	showSequence(fathers);
	Sequence* mothers = personToMothers(tom, database);
	showSequence(mothers);
	deleteSequence(fathers);
	deleteSequence(mothers);
	// Test familyToChildren, familyToFathers, familyToMothers
	printf("Testing familyToChildren, familyToFathers, familyToMothers\n");
	GNode* fam = getRecord("@F1@", database);
	kids = familyToChildren(fam, database);
	fathers = familyToFathers(fam, database);
	mothers = familyToMothers(fam, database);
	showSequence(kids);
	showSequence(fathers);
	showSequence(mothers);
	deleteSequence(kids);
	deleteSequence(fathers);
	deleteSequence(mothers);
	// Test personToSpouses
	printf("Testing personToSpouses\n");
	tom = getRecord("@I25@", database);
	Sequence* spouses = personToSpouses(tom, database);
	showSequence(spouses);
	// Test nameToSequence.
	printf("Testing nameToSequence\n");
	Sequence* tomwets = nameToSequence("Thomas/Wetmore/", database);

	printf("END TEST SEQUENCE: %2.3f\n", getMilliseconds());
//	Sequence *personToFamilies(GNode *person, bool, Database*);
//	Sequence *nameToSequence(String, Database*);
//	Sequence *refn_to_indiseq(String refn);
//	Sequence *unionSequence(Sequence*, Sequence*);
//	Sequence *intersectSequence(Sequence*, Sequence*);
//	Sequence *differenceSequence(Sequence*, Sequence*);
//	Sequence *childSequence(Sequence*);
//	Sequence *parentSequence(Sequence*);
//	Sequence *spouseSequence(Sequence*);
//	Sequence *ancestorSequence(Sequence*);
//	Sequence *descendentSequence(Sequence*);
//	Sequence *siblingSequence(Sequence*, bool);
//	bool elementFromSequence(Sequence*, int index, String* key, String* name);
//	void renameElementInSequence(Sequence* sequence, String key);
//	void sequenceToGedcom(Sequence*, FILE*);
}

static Sequence* tomsAncestors(Database* database) {
	Sequence* s = createSequence(database);
	appendToSequence(s, "@I1@", null);
	Sequence* a = ancestorSequence(s);
	deleteSequence(s);
	return a;
}
static Sequence* lusAncestors(Database* database) {
	Sequence* s = createSequence(database);
	appendToSequence(s, "@I2@", null);
	Sequence* a = ancestorSequence(s);
	deleteSequence(s);
	return a;
}
static Sequence* tomsDescendents(Database* database) {
	Sequence* s = createSequence(database);
	appendToSequence(s, "@I1@", null);
	Sequence* d = descendentSequence(s);
	deleteSequence(s);
	return d;
}
static Sequence* lssDescendents(Database* database) {
	Sequence* s = createSequence(database);
	appendToSequence(s, "@I2@", null);
	Sequence* d = descendentSequence(s);
	deleteSequence(s);
	return d;
}

static void checkTest(String name, int should, int was) {
	printf("TEST: %s: ", name);
	if (should == was) printf("PASSED\n");
	else printf("FAILED: %d != %d\n", should, was);
}
