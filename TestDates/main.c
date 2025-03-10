// TestDates
// main.c is the main procedure of the a program that tests DeadEnds dates.
//
// Created by Thomas Wetmore on 4 September 2024.
// Last changed on 10 September 2024.

#include <stdio.h>
#include "standard.h"
#include "date.h"

#define ALLOCFLAGS

static void runExtractTest(String date);
static void outDate(String date, int mod, int day, int month, int year, String yrstr);
static void testGedDataToken(void);

int main(int argc, const char * argv[]) {
	String date;
	runExtractTest("18 December 1949");
	runExtractTest("dec.  18th,     1949   ");
	runExtractTest("12/18/1949");
	runExtractTest("18/12/1949");
	testGedDataToken();
	return 0;
}

// testGedDataToken tests the getDateToken function.
static void testGedDataToken(void) {
	setExtractString("A m,n,Jan January Jan. JAN. JANUARY before after about ABT ABOUT AB hello 12 1 34 89 1948 12/12/12/12");
	DateToken tok;
	int ivalue;
	String svalue;
	while ((tok = getDateToken(&ivalue, &svalue)) != atEndToken) {
		printf("%d %d %s\n", tok, ivalue, svalue);
	}
}

// runExtractTest tests the extractDate function.
static void runExtractTest(String date) {
	int mod, day, month, year;
	String yearString;
	extractDate(date, &mod, &day, &month, &year, &yearString);
	outDate(date, mod, day, month, year, yearString);
}

// outDate prints the fields extracted by extractDate.
static void outDate(String date, int mod, int day, int month, int year, String yrstr) {
	printf("%s\t%i\t%i\t%i\t%i\t%s\n", date, mod, day, month, year, yrstr);
}
