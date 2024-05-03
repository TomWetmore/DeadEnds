// DeadEnds
//
// testsequence.c has code to test the Sequence data type.
//
// Created by Thomas Wetmore on 2 May 2024.
// Last changed on 2 May 2024.

#include "sequence.h"
#include "utils.h"

static void checkTest(String, int, int);

// testSequence is the starting function to test the Sequence type.
void testSequence(Database* database, int testNumber) {
	printf("%d: TEST SEQUENCE: %2.3f\n", testNumber, getMilliseconds());

	// Create a Sequence.
	Sequence* sequence = createSequence(database);
	int length = lengthSequence(sequence);
	checkTest("Sequence should be empty", 0, length);

	// Add an element to a Sequence.
	appendToSequence(sequence, "@I1@", "", null);
	length = lengthSequence(sequence);
	checkTest("Sequence should have one element", 1, length);
	showSequence(sequence);

	// Test emptySequence.
	emptySequence(sequence);
	length = lengthSequence(sequence);
	checkTest("Empty sequence", 0, length);

	// Test copySequence.
	appendToSequence(sequence, "@I1@", "", null);
	length = lengthSequence(sequence);
	checkTest("Added element to emptied sequence", 1, length);
	Sequence* copied = copySequence(sequence);
	length = lengthSequence(copied);
	checkTest("Copied Sequence should have 1 element", 1, length);
	showSequence(sequence);
	showSequence(copied);

	// Test isInSequence.
	appendToSequence(sequence, "@I2@", "Luann Frances /Grenda/", null);
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
	appendToSequence(sequence, "@I1@", "Thomas Trask /Wetmore/ IV", null);
	appendToSequence(sequence, "@I6@", "Joan Marie /Hancock/", null);
	appendToSequence(sequence, "@I3@", "Anna Vivian /Wetmore/", null);
	appendToSequence(sequence, "@I5@", "Thomas Trask /Wetmore/ V", null);
	appendToSequence(sequence, "@I4@", "Marie Margaret /Wetmore/", null);
	showSequence(sequence);
	printf("Calling keySortSequence\n");
	keySortSequence(sequence);
	showSequence(sequence);

	// Test nameSortSequence.
	printf("Calling nameSortSequence\n");
	nameSortSequence(sequence);
	showSequence(sequence);

	// Test ancestorSequence.
	printf("Testing ancestorSequence");
	emptySequence(sequence);
	appendToSequence(sequence, "@I1@", "Thomas Trask /Wetmore/ IV", null);
	Sequence* ancestors = ancestorSequence(sequence);
	showSequence(ancestors);



	printf("END TEST SEQUENCE: %2.3f\n", getMilliseconds());
//	void deleteSequence(Sequence*);
//	void valueSortSequence(Sequence*); //  Sort a sequence by value (not properly implemented).
//	Sequence *uniqueSequence(Sequence*); //Return sequence uniqueued from another.
//
//	Sequence *personToChildren(GNode *person, Database*);
//	Sequence *personToFathers(GNode *person, Database*);
//	Sequence *personToMothers(GNode *person, Database*);
//	Sequence *familyToChildren(GNode *family, Database*);
//	Sequence *familyToFathers(GNode *family, Database*);
//	Sequence *familyToMothers(GNode *family, Database*);
//	Sequence *personToSpouses(GNode *person, Database*);
//	Sequence *personToFamilies(GNode *person, bool, Database*);
//	Sequence *nameToSequence(String, Database*);
//	Sequence *refn_to_indiseq(String refn);
//
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
//
//	void sequenceToGedcom(Sequence*, FILE*);
//
//	// FORSEQUENCE and ENDSEQUENCE iterate a Sequence.
//	#define FORSEQUENCE(sequence, element, count)\
//	#define ENDSEQUENCE }}
}

static void checkTest(String name, int should, int was) {
	printf("TEST: %s: ", name);
	if (should == was) printf("PASSED\n");
	else printf("FAILED: %d != %d\n", should, was);
}
