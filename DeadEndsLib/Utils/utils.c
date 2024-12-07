// DeadEnds
//
// utils.c
//
// Created by Thomas Wetmore on 13 November 2022.
// Last changed on 6 December 2024.

#include <sys/time.h>
#include <string.h>
#include "standard.h"
#include "utils.h"

// getMseconds gets the current time in milliseconds modulo 10 seconds.
double getMseconds(void) {
    struct timeval time;
    void* tz;
    (void) gettimeofday(&time, &tz);
    int seconds = time.tv_sec % 10;  // Seconds between 0 and 9.
    int milliseconds = (int) (time.tv_usec/1000);
    return seconds + milliseconds / 1000.;
}

// getMsecondsStr gets the current time in milliseconds in a static String.
String getMsecondsStr(void) {
	double millis = getMseconds();
	static char buffer[10];;
	sprintf(buffer, "%2.3f", millis);
	return buffer;
}

// substring returns a substring of a String.
String substring(String s, int i, int j) {
	static char scratch[MAXLINELEN+1];
	if (!s || *s == 0 || i <= 0 || i > j || j > strlen(s)) return "";
	strncpy(scratch, &s[i-1], j-i+1);
	scratch[j-i+1] = 0;
	return strsave(scratch);
}

// rightjustify right justifies a String value.
String rightjustify(String string, int len) {
	static char scratch[MAXLINELEN+1];
	if (len < 1) return "";
	if (len > MAXLINELEN) len = MAXLINELEN;
	int lstr = (int) strlen(string);
	int nsp = len - lstr;
	if (nsp < 0) nsp = 0;
	char *s = scratch;
	int i;
	for (i = 0; i < nsp; i++)
		s[i] = ' ';
	for (int i = nsp, j = 0; i < len; i++, j++)
		s[i] = string[j];
	s[i] = 0;
	return scratch;
}
