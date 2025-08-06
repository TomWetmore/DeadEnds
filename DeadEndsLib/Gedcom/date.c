// DeadEnds Library

// date.c has the functions that deal with Gedcom-based dates.
//
// Created by Thomas Wetmore on 22 February 2023.
// Last changed on 13 September 2024.

#include <time.h>
#include "standard.h"
#include "date.h"
#include "stringtable.h"
#include "integertable.h"

static void format_ymd(String, String, String, int, int, String*);
static void formatDateModifier(int, String*);
static String formatDay(int, int);
static String formatMonth(int, int);
static String formatYear(int, int);
/*static*/ void setExtractString(String);
/*static*/ DateToken getDateToken(int*, String*);
static void initMonthTable(void);

// Strings that make up monthTable.
static struct {
    char *sl, *su, *ll, *lu;
} monthStrings[] = {
	// Month words.
    { "Jan", "JAN", "January", "JANUARY" },
    { "Feb", "FEB", "February", "FEBRUARY" },
    { "Mar", "MAR", "March", "MARCH" },
    { "Apr", "APR", "April", "APRIL" },
    { "May", "MAY", "May", "MAY" },
    { "Jun", "JUN", "June", "JUNE" },
    { "Jul", "JUL", "July", "JULY" },
    { "Aug", "AUG", "August", "AUGUST" },
    { "Sep", "SEP", "September", "SEPTEMBER" },
    { "Oct", "OCT", "October", "OCTOBER" },
    { "Nov", "NOV", "November", "NOVEMBER" },
    { "Dec", "DEC", "December", "DECEMBER" },
    // Modifier words.
    { "abt", "ABT", "about", "ABOUT" },     // 1
    { "bef", "BEF", "before", "BEFORE" },   // 2
    { "aft", "AFT", "after", "AFTER" },     // 3
    { "bet", "BET", "between", "BETWEEN" }, // 4 - range
    { "and", "AND", "and", "AND" },         // 5
    { "from", "FROM", "from", "FROM" },     // 6 - range
    { "to", "TO", "to", "TO" },             // 7
	{ "est", "EST", "estimated", "ESTIMATED" }, // 8
	{ "cal", "CAL", "calculated", "CALCULATED" }, // 9
	{ "cmp", "CMP", "computed", "COMPUTED" }, // 10
};

static String extractString = null;
static String extractCursor = null;
static StringTable *monthTable = null; // Maps month Strings to integers.

/*==========================================
 * formatDate -- Do general date formatting
 * str - raw string containing a date
 * dfmt - day format:  0 - num, space
 *                     1 - num, lead 0
 *                     2 - num, as is
 * mfmt -  month format:0 - num, space
 *                      1 - num, lead 0
 *                      2 - num, as is
 *                      3 - eg, MAR
 *                      4 - eg, Mar
 *                      5 - eg, MARCH
 *                      6 - eg, March
 * yfmt - year format: none yet
 * sfmt - date format: 0 - da mo yr
 *                     1 - mo da, yr
 *                     2 - mo/da/yr
 *                     3 - da/mo/yr
 *                     4 - mo-da-yr
 *                     5 - da-mo-yr
 *                     6 - modayr
 *                     7 - damoyr
 *                     8 - yr mo da
 *                     9 - yr/mo/da
 *                     10- yr-mo-da
 *                     11- yrmoda
 * cmplx - if TRUE, then treat string as complex, including
 *         date modifiers, ranges, and/or double-dating
 *========================================*/

String formatDate (String string, int dayFmt, int monthFmt, int yearFmt, int dateFmt, bool cmplx) {
    int mod, day, month, year;
    String sda, smo, syr;
    static char scratch[50], daystr[4];
    String p = scratch;
    if (!string) return null;
    extractDate(string, &mod, &day, &month, &year, &syr);
    if ((sda = formatDay(day, dayFmt))) sda = strcpy(daystr, sda);
    smo = formatMonth(month, monthFmt);
    if (!cmplx) syr = formatYear(year, yearFmt);
    else formatDateModifier(mod%100, &p);
    format_ymd(syr, smo, sda, dateFmt, mod, &p);
    if (cmplx && (mod%100 == 4 || mod%100 == 6)) {
        *p++ = ' ';
        formatDateModifier(mod%100 + 1, &p);
        extractDate(null, &mod, &day, &month, &year, &syr);
        if ((sda = formatDay(day, dayFmt))) sda = strcpy(daystr, sda);
        smo = formatMonth(month, monthFmt);
        format_ymd(syr, smo, sda, dateFmt, mod, &p);
    }
    return (String) scratch;
}

// format_ymd -- Assembles date according to dateformat
static void format_ymd (String syr, String smo, String sda, int sfmt, int mod, String *output) {
    String p = *output;
    switch (sfmt) {
        case 0:        /* da mo yr */
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
                *p++ = ' ';
            }
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
                *p++ = ' ';
            }
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 1:        /* mo da, yr */
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
                *p++ = ' ';
            }
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
                *p++ = ',';
                *p++ = ' ';
            }
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 2:        /* mo/da/yr */
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '/';
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p++ = '/';
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 3:        /* da/mo/yr */
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p++ = '/';
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '/';
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 4:        /* mo-da-yr */
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '-';
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p++ = '-';
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 5:        /* da-mo-yr */
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p++ = '-';
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '-';
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 6:        /* modayr */
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 7:        /* damoyr */
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 8:         /* yr mo da */
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            if (smo) {
                *p++ = ' ';
                strcpy(p, smo);
                p += strlen(p);
            }
            if (sda) {
                *p++ = ' ';
                strcpy(p, sda);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 9:         /* yr/mo/da */
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p++ = '/';
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '/';
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 10:        /* yr-mo-da */
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            *p++ = '-';
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            *p++ = '-';
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p = 0;
            break;
        case 11:        /* yrmoda */
            if (syr) {
                strcpy(p, syr);
                p += strlen(p);
            }
            if (smo) {
                strcpy(p, smo);
                p += strlen(p);
            }
            if (sda) {
                strcpy(p, sda);
                p += strlen(p);
            }
            *p = 0;
            break;
    }
    if (mod >= 100) {
        strcpy(p, " BC");
        p += 3;
    }
    *output = p;
    return;
}

// formatDateModifier -- Format date modifier
static void formatDateModifier (int mod, String *pp) {
    if (mod < 1 || mod > 10) return;
    strcpy(*pp, monthStrings[mod+12-1].ll);
    *pp += strlen(*pp);
    **pp = ' ';
    *pp += 1;
    return;
}

// formatDay formats the day part of a date. day is the day number (0 for unknown), and
// format is a code.
// MNOTE: may return a static buffer.
static String formatDay (int day, int format) {
    static char scratch[3];
    String p;
    if (day < 0 || day > 99 || format < 0 || format > 2) return null;
    strcpy(scratch, "  ");
    if (day >= 10) {
        scratch[0] = day/10 + '0';
        scratch[1] = day%10 + '0';
        return (String) scratch;
    }
    p = scratch;
    if (day == 0) {
        if (format == 2) return null;
        return (String) scratch;
    }
    if (format == 0)  p++;
    else if (format == 1)  *p++ = '0';
    *p++ = day + '0';
    *p = 0;
    return (String) scratch;
}

// formatMonth formats the month part of a date. month is the month number (0 for unknown), format
// is a code.
// MNOTE: may return a static buffer or .text space.
static String formatMonth (int month, int format) {
    static char scratch[3];
    String p;
    if (month < 0 || month > 12 || format < 0 || format > 6) return null;
    if (format <= 2)  {
        if ((p = formatDay(month, format))) return strcpy(scratch, p);
        return null;
    }
    if (month == 0) return (String) "   ";
    switch (format) {
        case 3: return (String) monthStrings[month-1].su;
        case 4: return (String) monthStrings[month-1].sl;
        case 5: return (String) monthStrings[month-1].lu;
        case 6: return (String) monthStrings[month-1].ll;
    }
    return null;
}

// formatYear formats the year part of a date.
// MNOTE: return static .bss memory.
static String formatYear (int year, int format) {
    static char scratch[5];
    if (year <= 0 || year > 5000) return null;
    switch (format) {
        default: sprintf(scratch, "%d", year);
    }
    return (String) scratch;
}

// extractDate attempts to extract a date from any String.
void extractDate(String string, int *pmod, int *pday, int *pmonth, int *pyear, String *pyrstr) {
    int tok, ival, era = 0;
    String sval;
    static unsigned char yrstr[10];  // Year string?
    *pyrstr = "";
    *pmod = *pday = *pmonth = *pyear = 0;
    if (string) setExtractString(string);
    while ((tok = getDateToken(&ival, &sval))) {
        switch (tok) {
            case monthToken: // Month string
                if (*pmonth == 0) *pmonth = ival;
                continue;
            case charToken:
                continue;
			case wordToken:
                if (*pyear == 0 && *pday == 0 && ival < 0) ival = -ival;
                if (ival > 0 && ival < 20 && *pmod == 0) *pmod = ival;
                if (ival == -99) era = 100;
                if ((*pmod == 4 && ival == 5) ||
                    (*pmod == 6 && ival == 7)) goto combine;
                continue;
            case intToken:
                /* years 1-99 are denoted by at least two leading zeroes */
                if (ival >= 100 ||
                    (ival > 0 && sval[0] == '0' && sval[1] == '0')) {
                    if (eqstr(*pyrstr,"")) {
                        strcpy((char*) yrstr, sval);
                        *pyrstr = (char*) yrstr;
                        *pyear = ival;
                    }
                }
                else if (ival <= 31 && *pday == 0) *pday = ival;
                continue;
			case unknownToken:
				continue;
            default:
                FATAL();
        }
    }
combine:
    *pmod += era;
}

// setExtractString initializes the file static date extraction string.
/*static*/ void setExtractString (String str) {
    extractString = str;
	extractCursor = str;
    if (!monthTable)
        initMonthTable();
}

// getDateToken returns the next token from the date extraction string.
/*static*/ DateToken getDateToken(int *pInt, String *pString) {
    static unsigned char scratch[256]; // Where tokens are built.
    String p = (String) scratch;
	*pInt = 0;
	*pString = (String) scratch;
    int i, c;
    if (!extractString) return atEndToken; // Must exist.
    while (iswhite(*extractString++)) ; // Skip white.
    extractString--;
	// Found a letter so look for a word.
    if (isLetter(*extractString)) {
		while (isLetter(*p++ = *extractString++)) // Load word into scratch.
			;
        *--p = 0;
        extractString--;
		*pString = (String) scratch;
		if (strlen((const char*) scratch) == 1) return charToken;
		// If the word is in the month table, return the month's integer.
        if ((i = searchIntegerTable(monthTable, upper((String)scratch))) > 0 && i <= 12) {
            *pInt = i;
            return monthToken;
        }
		// If word is one of the known date words return its index.
        *pInt = 0;
		if (i > 12 && i <= 22) {
			*pInt = i - 12;
			return wordToken;
		}
        *pInt = 0;
        return unknownToken;
    }
    if (chartype(*extractString) == DIGIT) {
        i = 0;
		while (chartype(c = *p++ = *extractString++) == DIGIT) {
			i = i*10 + c - '0';
		}
        //if (c == '/') { // Intended to handle double year dates.
            //while (chartype(*p++ = *extractString++) == DIGIT) ;
        //}
        *--p = 0;
        extractString--;
        *pString = (String) scratch;
        *pInt = i;
        return intToken;
    }
    if (*extractString == 0)  {
        extractString = null;
        return atEndToken;
    }
	*p++ = *extractString++;
	*p = 0;
	*pString = (String) scratch;
	*pInt = (int) scratch[0];
	return charToken;
}

// initMonthTable initializes the monthTable a static IntegerTable.
static void initMonthTable(void) {
    int i, j;
    monthTable = createIntegerTable(5);
    for (i = 0; i < 19; i++) {
        j = i + 1;
        insertInIntegerTable(monthTable, monthStrings[i].su, j);
        insertInIntegerTable(monthTable, monthStrings[i].lu, j);
    }
    insertInIntegerTable(monthTable, "EST", -1); // Ignored after date.
    insertInIntegerTable(monthTable, "BC", -99);
}

// get_date gets today's date.
String get_date(void) {
    struct tm *pt;
    time_t curtime;
    static char dat[20];
    curtime = time(null);
    pt = localtime(&curtime);
    sprintf(dat, "%d %s %d", pt->tm_mday, monthStrings[pt->tm_mon].su, 1900 + pt->tm_year);
    return (String) dat;
}

