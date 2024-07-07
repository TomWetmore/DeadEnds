// DeadEnds
//
// testgedcomstrings.c
//
// Created by Thomas Wetmore on 27 May 2024.
// Last changed on 6 July 2024.

#include "standard.h"
#include "readnode.h"
#include "gnodelist.h"

char* readFileIntoString(const char *filename);

// testGedcomStrings tests the stringToGnodeTree and showGNodeTree functions.
void testGedcomStrings(void) {
	printf("START OF TEST GEDCOM STRINGS\n");
	String file = "/Users/ttw4/Desktop/DeadEnds/Gedfiles/ttw.ged";
	String record = readFileIntoString(file);
	ErrorLog* errlog = createErrorLog();
	printf("The length of the record is %ld\n", strlen(record));
	GNodeList* nodelist = getGNodeListFromString(record, errlog);
	GNodeList* rootlist = getNodeTreesFromNodeList(nodelist, "string", errlog);
	if (rootlist) printf("There are %d records from the string.\n", lengthList(rootlist));
	GNode* root = ((GNodeListEl*) getListElement(rootlist, 0))->node;
	//GNode* root = stringToGNodeTree(record, null);
	showGNodeTree(root);
	printf("END OF TEST GEDCOM STRINGS\n");
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
		buffer = (char*)malloc((length + 1) * sizeof(char));
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
