//
//  DeadEnds Library
//  intrpstring.c
//
//  Created by Thomas Wetmore on 14 February 2024.
//  Last changed on 9 July 2025.
//

#include <stdio.h>
#include "evaluate.h"
#include "interp.h"
#include "pnode.h"
#include "pvalue.h"
#include "standard.h"
#include "utils.h"

// __substring finds a substring of a string.
//  usage: substring(STRING, INT, INT) -> STRING
PValue __substring (PNode *pnode, Context *context, bool *errflg) {
	PNode *arg = pnode->arguments;
	String full = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to substring must be a string.");
		return nullPValue;
	}
	arg = arg->next;
	int lo = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to substring must be an integer.");
		return nullPValue;
	}
	arg = arg->next;
	int hi = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The third argument to substring must be an integer.");
		return nullPValue;
	}
	return createStringPValue(substring(full, lo, hi));
}

// __index finds the nth occurrence of a substring in a string.
// usage: index(STRING, STRING, INT) -> INT
static int llindex(const char *str, const char *sub, int nth);
PValue __index (PNode* pnode, Context* context, bool* errflg) {
    PNode* sarg = pnode->arguments;
    PNode* barg = sarg->next;
    PNode* narg = barg->next;

    PValue spvalue = evaluate(sarg, context, errflg);
    if (*errflg) return nullPValue;
    if (spvalue.type != PVString) {
        *errflg = true;
        scriptError(pnode, "the first arg to index must be a string");
        return nullPValue;
    }
    PValue sub = evaluate(barg, context, errflg);
    if (*errflg) return nullPValue;
    if (sub.type != PVString) {
        *errflg = true;
        scriptError(pnode, "the second arg to index must be a string");
        return nullPValue;
    }
    PValue num = evaluate(narg, context, errflg);
    if (*errflg) return nullPValue;
    if (num.type != PVInt) {
        *errflg = true;
        scriptError(pnode, "the third arg to index must be an integer");
        return nullPValue;
    }
    int ncount = llindex(spvalue.value.uString, sub.value.uString, (int) num.value.uInt);
    return PVALUE(PVInt, uInt, ncount);
}

#define MAX_SUB_LEN 1024  // Adjust as needed

// Compute the prefix function for KMP algorithm
static void computePrefixTable(const char *pattern, size_t m, int *pi) {
    pi[0] = 0;
    int k = 0;
    for (size_t q = 1; q < m; q++) {
        while (k > 0 && pattern[k] != pattern[q])
            k = pi[k - 1];
        if (pattern[k] == pattern[q])
            k++;
        pi[q] = k;
    }
}

// llindex finds the 0-based index of the nth occurrence of `sub` in `str` using the KMP algorithm.
// Returns -1 if not found.
static int llindex(const char *str, const char *sub, int nth) {
    if (!str || !sub || *str == '\0' || *sub == '\0' || nth <= 0)
        return -1;

    size_t n = strlen(str);
    size_t m = strlen(sub);
    if (m >= MAX_SUB_LEN)
        return -1; // Avoid overflow; optional

    int pi[MAX_SUB_LEN];
    computePrefixTable(sub, m, pi);

    int q = 0;       // length of match
    int found = 0;   // number of matches found

    for (size_t i = 0; i < n; i++) {
        while (q > 0 && sub[q] != str[i])
            q = pi[q - 1];
        if (sub[q] == str[i])
            q++;
        if ((size_t)q == m) {
            found++;
            if (found == nth)
                return (int)(i - m + 1);  // 0-based index
            q = pi[q - 1];
        }
    }
    return -1; // not found
}

// __trim trims a string if too long,
// usage: trim(STRING, INT) -> STRING
PValue __trim (PNode *pnode, Context *context, bool *errflg) {
	// Get the String to be trimmed.
	PNode *arg = pnode->arguments;
	String string = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to trim must be a string.");
		return nullPValue;
	}
	// Get the maximum size of the String.
	arg = arg->next;
	int length = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to time must be an integer.");
		return nullPValue;
	}
	return createStringPValue(trim(string, length));
}

// __rjustify right justifies a string value.
// usage: rjustify(STRING, INT) -> STRING
PValue __rjustify (PNode *pnode, Context *context, bool *errflg) {
	// Get the String to be right justified.
	PNode *arg = pnode->arguments;
	String string = evaluateString(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The first argument to rjustify must be a string.");
		return nullPValue;
	}
	// Get the size of field to right justify the String into.
	arg = arg->next;
	int length = evaluateInteger(arg, context, errflg);
	if (*errflg) {
		scriptError(pnode, "The second argument to rjustify must be an integer.");
		return nullPValue;
	}
	return createStringPValue(rightjustify(string, length));
}
