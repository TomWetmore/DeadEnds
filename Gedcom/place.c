//
//  place.c
//  CloneOne
//
//  Created by Thomas Wetmore on 12 February 2024.
//  Last changed on 12 February 2024.
//

#include <stdio.h>
#include "standard.h"
#include "list.h"

static bool inString (int chr, String str);
bool value_to_list (String str, List *list, int *plen, String dlm);

/*===================================================
 * place_to_list -- Convert place string to word list
 *=================================================*/
bool placeToList(String place, List *list, int *plen)
{
	return value_to_list(place, list, plen, ",");
}
/*=============================================
 * value_to_list -- Convert string to word list
 *===========================================*/
bool value_to_list (String str, List *list, int *plen, String dlm)
{
	static String buf = null;
	static int len0 = 0;
	String p, q, n;
	int len, c, i, j;

	if (!str || *str == 0 || !list) return false;
	emptyList(list);
	if ((len = (int) strlen(str)) > len0 - 2) {
		if (buf) stdfree(buf);
		buf = (String) stdalloc(len0 = len + 80);
	}
	strcpy(buf, str);
	buf[len + 1] = 0;
	p = buf;
	j = 1;
	while ((c = *p++)) {
		if (inString(c, dlm)) {
			*(p - 1) = 0;
			j++;
		}
	}
	p = buf;
	for (i = 1;  i <= j;  i++) {
		n = p + strlen(p) + 1;
		while (chartype(c = *p++) == WHITE)
			;
		p--;
		q = p + strlen(p) - 1;
		while (q > p && chartype(*q) == WHITE)
			*q-- = 0;
		setListElement(list, i, strsave(p));
		p = n;
	}
	*plen = j;
	return true;
}

static bool inString (int chr, String str)
{
	while (*str && chr != *str)
		str++;
	return *str != 0;
}

