// DeadEnds
//
// testgedcomstrings.c
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 20 May 2025.

#include "errors.h"
#include "gnode.h"
#include "standard.h"
#include "readnode.h"
#include "gnodelist.h"
#include "utils.h"

char* readFileIntoString(const char *filename);

// testGedcomStrings tests the stringToGnodeTree and showGNodeTree functions.
void testGedcomStrings(int testNumber) {
	printf("%d: START OF TEST GEDCOM STRINGS TEST %s\n", testNumber, getMsecondsStr());
	String file = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/ttw.ged";
	String record = readFileIntoString(file);
	ErrorLog* errlog = createErrorLog();
	printf("The length of the record is %ld\n", strlen(record));
	//GNodeList* nodelist = getGNodeListFromString(record, errlog);
	//GNodeList* rootlist = getNodeTreesFromNodeList(nodelist, "string", errlog);
	GNodeList* list = getGNodeTreesFromString(record, "string", errlog);
	if (list) printf("There are %d records from the string.\n", lengthList(list));
	FORLIST(list, el)
		GNode* root = ((GNodeListEl*) el)->node;
		showGNodeTree(root);
	ENDLIST
	//GNode* root = ((GNodeListEl*) getListElement(rootlist, 0))->node;
	//GNode* root = stringToGNodeTree(record, null);
	//showGNodeTree(root);
	printf("%d: END OF TEST GEDCOM STRINGS\n", testNumber);
}

// readFileIntoString reads a file into a string; written by ChatGPT.
char* readFileIntoString(const char* filename) {
	FILE* file = fopen(filename, "r");
	char* buffer = NULL;
	long length;

	if (file) {
		fseek(file, 0, SEEK_END); // Get length.
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = (char*) stdalloc((length + 1) * sizeof(char));
		if (buffer) {
			fread(buffer, 1, length, file);
			buffer[length] = '\0';
		}
		fclose(file);
	} else {
		fprintf(stderr, "Could not open file %s for reading\n", filename);
	}
	return buffer;
}
