// DeadEnds
//
// patchsex.c holds the main program of the DeadEnds tool that patches incorrect SEX lines in
// INDI records, and adds them in INDI records that do not have one.
//
// Created by Thomas Wetmore on 10 July 2024.
// Last changed on 23 July 2024.

#include "patchsex.h"
#include "splitjoin.h"
#include "utils.h"
#include "file.h"

FILE* debugFile; // Get rid of this.

static void patchSexLine(GNode*, int);

// main is the main program of the patchsex tool. It processes a Gedcom file looking for persons
// with missing or erroneous SEX lines. If fixes them and then writes the file back out.
int main(void) {
	// Get the Gedcom records from the file.
	String fileName = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/07022024.ged";
	File* file = openFile(fileName, "r");
	ErrorLog* log = createErrorLog();
	GNodeList* roots = getGNodeTreesFromFile(file, log);
	if (lengthList(log) > 0) {
		printf("patchsex: cancelled due to errors\n");
		showErrorLog(log);
		closeFile(file);
		exit(1);
	}
	closeFile(file);
	printf("The length of roots is %d.\n", lengthList(roots));
	if (lengthList(roots) <= 0) {
		printf("patchsex: no persons to patch.\n");
		exit(1);
	}
	// Call patchSexLine on each INDI record form the file.
	FORLIST(roots, element)
		GNodeListEl* el = (GNodeListEl*) element;
		GNode* root = el->node;
		int line = el->line;
		if (recordType(root) == GRPerson) patchSexLine(root, line);
	ENDLIST
	// Write the possibly modified list of records out to a file.
	File* outfile = openFile("/Users/ttw4/Desktop/DeadEnds/Gedfiles/modified.ged", "w");
	writeGNodeTreesToFile(roots, outfile);
	closeFile(outfile);
	return 0;
}

// patchSexLine adds a SEX line to an INDI record if it does not have one. It has the side
// effect of "normalizing" the record.
static void patchSexLine(GNode* indi, int line) {
	GNode *name, *refn, *sex, *body, *famc, *fams;
	bool added = false;
	splitPerson(indi, &name, &refn, &sex, &body, &famc, &fams);
	if (sex && !validSexString(sex->value)) {
		printf("Changing a sex value from %s to U.\n", sex->value);
		sex->value = "U";
	}
	if (!sex) {
		printf("Adding a sex line.\n");
		sex = createGNode(null, "SEX", "U", indi);
		added = true;
	}
	joinPerson(indi, name, refn, sex, body, famc, fams);
	if (added) showGNodeTree(indi);
}
