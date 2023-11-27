//
//  DeadEnds
//
//  date.c
//
//  Created by Thomas Wetmore on 22 February 2023.
//  Last changed on 29 September 2023.
//

#include <time.h>
#include "standard.h"
#include "date.h"
#include "stringtable.h"
#include "integertable.h"

#define MONTH_TOK 1
#define CHAR_TOK  2
#define WORD_TOK  3
#define ICONS_TOK 4

static void format_ymd(String, String, String, int, int, String*);
static void format_mod(int, String*);
static String format_day(int, int);
static String format_month(int, int);
static String format_year(int, int);
static void set_date_string(String);
static int get_date_tok(int*, String*);
static void init_monthtbl(void);

static struct {
    char *sl, *su, *ll, *lu;
} monthstrs[19] = {
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

    /* date modifiers appended to the month table */

    { "abt", "ABT", "about", "ABOUT" },     /*  1 */
    { "bef", "BEF", "before", "BEFORE" },   /*  2 */
    { "aft", "AFT", "after", "AFTER" },     /*  3 */
    { "bet", "BET", "between", "BETWEEN" }, /*  4 - range */
    { "and", "AND", "and", "AND" },         /*  5 */
    { "from", "FROM", "from", "FROM" },     /*  6 - range */
    { "to", "TO", "to", "TO" },             /*  7 */
};

static String sstr = null;
static StringTable *monthtbl = null;

/*==========================================
 * format_date -- Do general date formatting
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
String format_date (String str, int dfmt, int mfmt, int yfmt, int sfmt, bool cmplx)
{
    int mod, da, mo, yr;
    String sda, smo, syr;
    static char scratch[50], daystr[4];
    String p = scratch;
    if (!str) return null;
    extract_date(str, &mod, &da, &mo, &yr, &syr);
    if ((sda = format_day(da, dfmt))) sda = strcpy(daystr, sda);
    smo = format_month(mo, mfmt);
    if (!cmplx) syr = format_year(yr, yfmt);
    else format_mod(mod%100, &p);
    format_ymd(syr, smo, sda, sfmt, mod, &p);
    if (cmplx && (mod%100 == 4 || mod%100 == 6)) {
        *p++ = ' ';
        format_mod(mod%100 + 1, &p);
        extract_date(null, &mod, &da, &mo, &yr, &syr);
        if ((sda = format_day(da, dfmt))) sda = strcpy(daystr, sda);
        smo = format_month(mo, mfmt);
        format_ymd(syr, smo, sda, sfmt, mod, &p);
    }
    return (String) scratch;

}
/*===================================================
 * format_ymd -- Assembles date according to dateformat
 *=================================================*/
static void
format_ymd (String syr,
            String smo,
            String sda,
            int sfmt,           /* format code */
            int mod,
            String *output)
{
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

/*=====================================
 * format_mod -- Format date modifier
 *===================================*/
static void
format_mod (int mod,
            String *pp)
{
    if (mod < 1 || mod > 7) return;
    strcpy(*pp, monthstrs[mod+12-1].ll);
    *pp += strlen(*pp);
    **pp = ' ';
    *pp += 1;
    return;
}
/*=======================================
 * format_day -- Formats day part of date
 *=====================================*/
static String
format_day (int da,         /* day - 0 for unknown */
            int dfmt)       /* format code */
{
    static char scratch[3];
    String p;
    if (da < 0 || da > 99 || dfmt < 0 || dfmt > 2) return null;
    strcpy(scratch, "  ");
    if (da >= 10) {
        scratch[0] = da/10 + '0';
        scratch[1] = da%10 + '0';
        return (String) scratch;
    }
    p = scratch;
    if (da == 0) {
        if (dfmt == 2) return null;
        return (String) scratch;
    }
    if (dfmt == 0)  p++;
    else if (dfmt == 1)  *p++ = '0';
    *p++ = da + '0';
    *p = 0;
    return (String) scratch;
}
/*===========================================
 * format_month -- Formats month part of date
 *  returns static buffer
 *=========================================*/
static String
format_month (int mo,         /* month - 0 for unknown */
              int mfmt)       /* format code */
{
    static char scratch[3];
    String p;
    if (mo < 0 || mo > 12 || mfmt < 0 || mfmt > 6) return null;
    if (mfmt <= 2)  {
        if ((p = format_day(mo, mfmt))) return strcpy(scratch, p);
        return null;
    }
    if (mo == 0) return (String) "   ";
    switch (mfmt) {
        case 3: return (String) monthstrs[mo-1].su;
        case 4: return (String) monthstrs[mo-1].sl;
        case 5: return (String) monthstrs[mo-1].lu;
        case 6: return (String) monthstrs[mo-1].ll;
    }
    return null;
}
/*=========================================
 * format_year -- Formats year part of date
 *=======================================*/
static String
format_year (int yr,
             int yfmt)
{
    static char scratch[50];
    if (yr <= 0)  return null;
    switch (yfmt) {
        default: sprintf(scratch, "%d", yr);
    }
    return (String) scratch;
}

//  extract_date -- Extract date from free format string
//--------------------------------------------------------------------------------------------------
void
extract_date (String str,
              int *pmod,
              int *pda,
              int *pmo,
              int *pyr,
              String *pyrstr)
{
    int tok, ival, era = 0;
    String sval;
    static unsigned char yrstr[10];  // Year string?
    *pyrstr = "";
    *pmod = *pda = *pmo = *pyr = 0;
    if (str) set_date_string(str);
    while ((tok = get_date_tok(&ival, &sval))) {
        switch (tok) {
            case MONTH_TOK:
                if (*pmo == 0) *pmo = ival;
                continue;
            case CHAR_TOK:
                continue;
            case WORD_TOK:
                if (*pyr == 0 && *pda == 0 && ival < 0) ival = -ival;
                if (ival > 0 && ival < 20 && *pmod == 0) *pmod = ival;
                if (ival == -99) era = 100;
                if ((*pmod == 4 && ival == 5) ||
                    (*pmod == 6 && ival == 7)) goto combine;
                continue;
            case ICONS_TOK:
                /* years 1-99 are denoted by at least two leading zeroes */
                if (ival >= 100 ||
                    (ival > 0 && sval[0] == '0' && sval[1] == '0')) {
                    if (eqstr(*pyrstr,"")) {
                        strcpy((char*) yrstr,sval);
                        *pyrstr = (char*) yrstr;
                        *pyr = ival;
                    }
                }
                else if (ival <= 31 && *pda == 0) *pda = ival;
                continue;
            default:
                FATAL();
        }
    }
combine:
    *pmod += era;
}

//  set_date_string -- Initialize the date extraction string.
//--------------------------------------------------------------------------------------------------
static void set_date_string (String str)
{
    sstr = str;
    if (!monthtbl) init_monthtbl();
}
/*==================================================
 * get_date_tok -- Return next date extraction token
 *================================================*/
static int
get_date_tok (int *pival,
              String *psval)
{
    static unsigned char scratch[30];
    String p = (String) scratch;
    int i, c;
    if (!sstr) return 0;
    while (iswhite(*sstr++))
        ;
    sstr--;
    if (isLetter(*sstr)) {
        while (isLetter(*p++ = *sstr++))
            ;
        *--p = 0;
        sstr--;
        *psval = (String) scratch;
        if ((i = searchIntegerTable(monthtbl, upper((String)scratch))) > 0 && i <= 12) {
            *pival = i;
            return MONTH_TOK;
        }
        *pival = 0;
        if (i > 12) *pival = i - 12;
        if (i < 0) *pival = i;
        return WORD_TOK;
    }
    if (chartype(*sstr) == DIGIT) {
        i = 0;
        while (chartype(c = *p++ = *sstr++) == DIGIT)
            i = i*10 + c - '0';
        if (c == '/') {
            while (chartype(*p++ = *sstr++) == DIGIT) ;
        }
        *--p = 0;
        sstr--;
        *psval = (String) scratch;
        *pival = i;
        return ICONS_TOK;
    }
    if (*sstr == 0)  {
        sstr = null;
        return 0;
    }
    *pival = *sstr++;
    *psval = (String) "";
    return CHAR_TOK;
}

//  init_monthtbl -- Initialize the month string table.
//--------------------------------------------------------------------------------------------------
static void init_monthtbl (void)
{
    int i, j;
    monthtbl = createIntegerTable();
    for (i = 0; i < 19; i++) {
        j = i + 1;
        insertInIntegerTable(monthtbl, monthstrs[i].su, j);
        insertInIntegerTable(monthtbl, monthstrs[i].lu, j);
    }
    insertInIntegerTable(monthtbl, "EST", -1);  /* ignored after date */
    insertInIntegerTable(monthtbl, "BC", -99);
}

//  get_date -- Get today's date
//--------------------------------------------------------------------------------------------------
String get_date(void)
{
    struct tm *pt;
    time_t curtime;
    static char dat[20];
    curtime = time(null);
    pt = localtime(&curtime);
    sprintf(dat, "%d %s %d", pt->tm_mday, monthstrs[pt->tm_mon].su, 1900 + pt->tm_year);
    return (String) dat;
}

