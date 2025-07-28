//
//  DeadEnds Library
//
//  pvalue.c holds the functions that handle program expression values (PValues) when interpreting
//  DeadEnds scripts.
//
//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 28 July 2025.
//

#include "gedcom.h"
#include "gnode.h"
#include "hashtable.h"
#include "list.h"
#include "pvalue.h"
#include "pvaluelist.h"
#include "pvaluetable.h"
#include "sequence.h"
#include "standard.h"

extern const PValue nullPValue;  // Defined in builtin.c

/// Returns true if a PVType is one of the GNode types.
bool isGNodeType(PVType type) {
    return type >= PVGNode && type <= PVOther;
}

static bool isZero(PValue);
bool isZeroVUnion(PVType, VUnion);

/// Allocates a PValue in the heap.
///
/// PValues are usually value types, but when stored in tables or
/// lists they must be in the heap.
PValue *allocPValue(PVType type, VUnion value) {
    PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue));
    ppvalue->type = type;
    ppvalue->value = value;
    return ppvalue;
}

/// Creates a String PValue with the String in the heap.
PValue createStringPValue(String string) {
    PValue pvalue;
    pvalue.type = PVString;
    // Save the String in the heap.
    pvalue.value.uString = string ? strsave(string) : null;
    return pvalue; // Returned on stack.
}

// In case we need it
PValue copyPValue(PValue orig) {
    if (orig.type == PVString && orig.value.uString) {
        return createStringPValue(orig.value.uString);
    }
    return orig;
}

// clonePValue returns a heap-allocated copy of the given PValue.
// If the value is a PVString, the underlying string is duplicated.
PValue* clonePValue(const PValue* original) {
    if (!original) return NULL;
    PValue* copy = stdalloc(sizeof(PValue));
    copy->type = original->type;
    switch (original->type) {
    case PVString:
        copy->value.uString = original->value.uString ? strsave(original->value.uString) : NULL;
        break;
    default:
        copy->value = original->value;
        break;
    }
    return copy;
}

// cloneAndReturnPValue clones a copy of a PValue on the heap, but then makes a stack copy,
// frees the help copy and returns the stack copy.
PValue cloneAndReturnPValue(const PValue* original) {
    if (!original) return nullPValue;
    PValue* heapCopy = clonePValue(original);
    PValue result = *heapCopy;
    stdfree(heapCopy);
    return result;
}

// typeOfPValue returns the 'type' of a PValue as a String.
String typeOfPValue(PValue pvalue) {
    String typename;
    switch (pvalue.type) {
    case PVNull:     typename = "null"; break;
    case PVInt:      typename = "int"; break;
    case PVFloat:    typename = "float"; break;
    case PVBool:     typename = "bool"; break;
    case PVString:   typename = "string"; break;
    case PVGNode:    typename = "gnode"; break;
    case PVPerson:   typename = "person"; break;
    case PVFamily:   typename = "family"; break;
    case PVSource:   typename = "source"; break;
    case PVEvent:    typename = "event"; break;
    case PVOther:    typename = "other"; break;
    case PVList:     typename = "list"; break;
    case PVTable:    typename = "table"; break;
    case PVSequence: typename = "sequence"; break;
    default:         typename = "unknown"; break;
    }
    return typename;
}

#define BIGNUMBER 4000

/// Returns the value of a `PValue` as a `String`.
///
/// The caller is responsible for freeing the `String`.
String valueOfPValue(PValue pvalue) {

    char *scratch = (char *) stdalloc(BIGNUMBER);
    scratch[0] = '\0';

    switch (pvalue.type) {
    case PVNull:
        strcpy(scratch, "null");
        break;
    case PVInt:
        sprintf(scratch, "%ld", pvalue.value.uInt);
        break;
    case PVFloat:
        sprintf(scratch, "%g", pvalue.value.uFloat);
        break;
    case PVBool:
        sprintf(scratch, "%s", pvalue.value.uBool ? "true" : "false");
        break;
    case PVString:
        snprintf(scratch, BIGNUMBER, "%s", pvalue.value.uString ? pvalue.value.uString : "(null)");
        break;
    case PVGNode:
    case PVPerson:
    case PVFamily:
    case PVSource:
    case PVEvent:
    case PVOther:
        snprintf(scratch, BIGNUMBER, "%s",
                 gnodeToString(pvalue.value.uGNode, gnodeLevel(pvalue.value.uGNode)));
        break;

    case PVList: {
        List *list = pvalue.value.uList;
        if (!list) {
            strcpy(scratch, "{null}");
            break;
        }
        size_t remaining = BIGNUMBER;
        char *p = scratch;
        size_t written = snprintf(p, remaining, "{");
        p += written; remaining -= written;

        FORLIST(list, element)
            String el = valueOfPValue(*((PValue *) element)); // Recursive.
            written = snprintf(p, remaining, "%s, ", el);
            stdfree(el); // Free String from recursive call.
            if (written >= remaining) break; // Avoid overflow.
            p += written; remaining -= written;
        ENDLIST

        if (p > scratch + 1) { // remove last ", " if added
            p[-2] = '}';
            p[-1] = '\0';
        } else {
            strcat(scratch, "}");
        }
        break;
    }
    case PVTable:
        snprintf(scratch, BIGNUMBER, "%d elements", sizeHashTable(pvalue.value.uTable));
        break;
    case PVSequence:
        snprintf(scratch, BIGNUMBER, "(sequence)");
        break;
    default:
        strcpy(scratch, "(unknown)");
        break;
    }
    return scratch; // safe because caller owns memory
}

/// Frees the memory of an allocated PValue.
void freePValue(PValue* ppvalue) {
    switch (ppvalue->type) {
    case PVString:
        if (ppvalue->value.uString) stdfree(ppvalue->value.uString);
        break;
    case PVSequence: // Cannot delete Sequences until more memory management added.
        //deleteSequence(ppvalue->value.uSequence);
        break;
    case PVList: // Cannot delete lists until more memory management added.
        //deleteList(ppvalue->value.uList);
        break;
    case PVTable: // Cannot delete tables until more memory management added.
        break;
    default:
        break;
    }
    stdfree(ppvalue);
}

/// Checks if a PValue has numeric type.
bool numericPValue(PValue value) {
    return value.type == PVInt || value.type == PVFloat;
}

/// Checks a PValue for validity by checking its type.
bool isPValue(PValue pvalue) {
    return pvalue.type >= PVNull && pvalue.type <= PVSequence;
}

/// Checks if a PValue is a Gedcom node type.
bool isRecordType(PVType type) {
    return type >= PVPerson && type <= PVOther;
}

// addPValues adds two PValues and returns their sum.
//PValue addPValues(PValue val1, PValue val2, bool* eflg) {
//	PVType type1 = val1.type, type2 = val2.type;
//	if (type1 != type2 || (type1 != PVInt && type1 != PVFloat)) {
//		*eflg = true;
//		return nullPValue;
//	}
//	if (type1 == PVInt)
//		return PVALUE(PVInt, uInt, val1.value.uInt+val2.value.uInt);
//	else
//		return PVALUE(PVFloat, uFloat, val1.value.uFloat+val2.value.uFloat);
//}

// subPValues subtract two PValues and returns their difference.
PValue subPValues(PValue val1, PValue val2, bool* eflg) {
    PVType type1 = val1.type, type2 = val2.type;
    if (type1 != type2 || (type1 != PVInt && type1 != PVFloat)) {
        *eflg = true;
        return nullPValue;
    }
    if (type1 == PVInt)
        return PVALUE(PVInt, uInt, val1.value.uInt-val2.value.uInt);
    else
        return PVALUE(PVFloat, uFloat, val1.value.uFloat-val2.value.uFloat);
}

// mulPValues multiplies two PValues and returns their product.
PValue mulPValues(PValue val1, PValue val2, bool* eflg) {
    PVType type1 = val1.type, type2 = val2.type;
    if (type1 != type2 || (type1 != PVInt && type1 != PVFloat)) {
        *eflg = true;
        return nullPValue;
    }
    if (type1 == PVInt)
        return PVALUE(PVInt, uInt, val1.value.uInt*val2.value.uInt);
    else
        return PVALUE(PVFloat, uFloat, val1.value.uFloat*val2.value.uFloat);
}

// divPValues divides two PValues and returns their quotient.
PValue divPValues(PValue val1, PValue val2, bool* eflg) {
    PVType type1 = val1.type, type2 = val2.type;
    if (type1 != type2 || (type1 != PVInt && type1 != PVFloat) || isZero(val2)) {
        *eflg = true;
        return nullPValue;
    }
    if (type1 == PVInt)
        return PVALUE(PVInt, uInt, val1.value.uInt/val2.value.uInt);
    else
        return PVALUE(PVFloat, uFloat, val1.value.uFloat/val2.value.uFloat);
}

// modPValues takes the modulus of two PValues. Both values must be integers.
PValue modPValues(PValue val1, PValue val2, bool* eflg) {
    PVType type1 = val1.type, type2 = val2.type;
    if (type1 != type2 || type2 != PVInt || isZero(val2)) {
        *eflg = true;
        return nullPValue;
    }
    return PVALUE(PVInt, uInt, val1.value.uInt%val2.value.uInt);
}

// expPValues raises the first number to the power of the second. They must be integers.
PValue expPValues(PValue val1, PValue val2, bool* eflg) {
    PVType type1 = val1.type, type2 = val2.type;
    if (type1 != type2 || type2 != PVInt) {
        *eflg = true;
        return nullPValue;
    }
    long prod = 1;
    long base = val1.value.uInt;
    long exp = val2.value.uInt;
    for (int i = 0; i < exp; i++) {
        prod *= base;
    }
    return PVALUE(PVInt, uInt, prod);
}

// iszero returns whether a PValue is zero.
static bool isZero(PValue value) {
    switch (value.type) {
    case PVInt: return value.value.uInt == 0;
    case PVFloat: return value.value.uFloat == 0.0;
    default: return false;
    }
}

// eqPValues returns whether two PValue are equal.
//PValue eqPValues(PValue val1, PValue val2, bool* eflg) {
//	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
//	if (*eflg) return nullPValue;
//	bool b = false;  // False until proven true.
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt == val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat = val2.value.uFloat; break;
//		case PVString: b = eqstr(val1.value.uString, val2.value.uString); break;
//		// TODO: ADD MORE CASES AS DEVELOPMENT CONTINUES. I.E., THE RECORD TYPES.
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}

// nePValues -- See if two PValues are not equal. First PValue is converted to a PVBool value
// with the result. The second PValue is freed.
//--------------------------------------------------------------------------------------------------
//PValue nePValues(PValue val1, PValue val2, bool* eflg)
////  val1 -- Left operand.
////  val2 -- Right operand.
////  eflg -- Returned error flag.
//{
//	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
//	if (*eflg) return nullPValue;
//	bool b = false;  // False until proven true.
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt != val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat != val2.value.uFloat; break;
//		case PVString: b = nestr(val1.value.uString, val2.value.uString); break;
//		// TODO: ADD MORE CASES AS DEVELOPMENT CONTINUES. I.E., THE RECORD TYPES.
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}

//  lePValues -- Check <= relation between PValues.
//--------------------------------------------------------------------------------------------------
//PValue lePValues(PValue val1, PValue val2, bool* eflg)
////  val1 -- Left operand.
////  val2 -- Right operand.
////  eflg -- Returned error flag.
//{
//	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
//	if (*eflg) return nullPValue;
//	bool b = false;  // False until proven true.
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt <= val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat <= val2.value.uFloat; break;
//		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) <= 0); break;
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}
//
////  gePValues -- Check >= relation between PValues.
////--------------------------------------------------------------------------------------------------
//PValue gePValues (PValue val1, PValue val2, bool* eflg)
////  val1, val2 -- Left and right operands.
////  eflg -- Returned error flag.
//{
//	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
//	if (*eflg) return nullPValue;
//	bool b = false;  // False until proven true.
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt <= val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat <= val2.value.uFloat; break;
//		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) >= 0); break;
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}
//
//// ltPValues checks the less than relation between PValues.
//PValue ltPValues(PValue val1, PValue val2, bool* eflg) {
//	bool b = false;
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt < val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat < val2.value.uFloat; break;
//		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) < 0); break;
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}
//
//// gtPValues checks the greater than relation between PValues.
//PValue gtPValues(PValue val1, PValue val2, bool* eflg) {
//	bool b = false;
//	int type = val1.type;
//	if (type != val2.type) {
//		*eflg = true;
//		return nullPValue;
//	}
//	switch (type) {
//		case PVInt: b = val1.value.uInt > val2.value.uInt; break;
//		case PVFloat: b = val1.value.uFloat > val2.value.uFloat; break;
//		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) > 0); break;
//		default: break;
//	}
//	return PVALUE(PVBool, uBool, b);
//}
//
//// incrPValue increments a PValue which must be an integer.
//PValue incrPValue(PValue pvalue, bool* eflg) {
//	if (pvalue.type == PVInt)
//		return PVALUE(PVInt, uInt, pvalue.value.uInt + 1);
//	else {
//		*eflg = true;
//		return nullPValue;
//	}
//}
//
//// decrPValue decrement a PValue which must be an integer.
//PValue decrPValue(PValue pvalue, bool* eflg) {
//	if (pvalue.type == PVInt)
//		return PVALUE(PVInt, uInt, pvalue.value.uInt - 1);
//	else {
//		*eflg = true;
//		return nullPValue;
//	}
//}
//
// negPValue negates a PValue which must be numeric.
PValue negPValue(PValue value, bool* eflg) {
    if (value.type == PVInt)
        return PVALUE(PVInt, uInt, -value.value.uInt);
    if (value.type == PVFloat)
        return PVALUE(PVFloat, uFloat, -value.value.uFloat);
    *eflg = true;
    return nullPValue;
}
