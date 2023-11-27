//
//  DeadEnds
//
//  pvalue.c -- Functions that handle program expression values (PValues) when interpreting
//    DeadEnds programs.
//
//  Created by Thomas Wetmore on 15 December 22.
//  Last changed on 17 October 2023.
//

#include "pvalue.h"
#include "standard.h"
#include "gedcom.h"

extern const PValue nullPValue;  // Defined in builtin.c

//  isPVGNodeType -- Return true if a type is one of the PGNode types.
//--------------------------------------------------------------------------------------------------
bool isGNodeType(PVType type)
{
	return type >= PVGNode && type <= PVOther;
}

// ptypes -- Array of the PValue type names for debug printing.
//--------------------------------------------------------------------------------------------------
static char *ptypes[] = {
	"PVNull", "PVAny", "PVInt", "PVFloat", "PVBool", "PVString", "PVGNode", "PVPerson",
	"PVFamily", "PVSource", "PVEvent", "PVOther", "PVList", "PVTable", "PVSequence"};

//static int bool_to_int(bool);  // Convert an integer to a boolean.
//static double bool_to_float(bool);  // Convert a real number to a boolean.

static bool isZero(PValue);
bool isZeroVUnion(PVType, VUnion);

//  allocPValue -- Allocate a program value in the heap. Program values are normally value types,
//    but when there are stored in tables or lists they need to be in the heap.
//--------------------------------------------------------------------------------------------------
PValue *allocPValue(PVType type, VUnion value)
{
	PValue* ppvalue = (PValue*) stdalloc(sizeof(ppvalue));
	ppvalue->type = type;
	ppvalue->value = value;
	return ppvalue;
}

//  freePValue -- Free a PValue that has been allocated. Only PValues in symbol tables and
//    sequences are allocated on the heap.
//  TODO: Must handle other value types!!! Importantly, PVSequence and later others.
//--------------------------------------------------------------------------------------------------
void freePValue(PValue* ppvalue)
{
	switch (ppvalue->type) {
		case PVString:
			if (ppvalue->value.uString) stdfree(ppvalue->value.uString);
			break;
		case PVSequence:
			deleteSequence(ppvalue->value.uSequence, false);
			break;
		default:
			break;
	}
	stdfree(ppvalue);
}

// copyPValue -- Copy a program value.
//--------------------------------------------------------------------------------------------------
PValue *copyPValue(PValue pvalue)
{
	PValue *ppvalue = (PValue*) stdalloc(sizeof(ppvalue));
	memcpy(ppvalue, &pvalue, sizeof(PValue));
	return ppvalue;
}

//PValue copyPValue(PValue pvalue)
//{
//    if (!pvalue) return null;
//    return createPValue(pvalue->pvType, pvalue->pvValue);
//}

//  setPValue -- Set a program value.
//  TODO: THIS ISN'T GOING TO WORK ANYMORE.
//--------------------------------------------------------------------------------------------------
void setPValue(PValue pvalue, int type, VUnion value)
{
	if (pvalue.type == PVString && pvalue.value.uString) stdfree(pvalue.value.uString);
	pvalue.type = type;
	if (type == PVString && value.uString) value.uString = strsave(value.uString);
	pvalue.value = value;
}

//  numericPValue -- See if a PValue is numeric.
//--------------------------------------------------------------------------------------------------
bool numericPValue(PValue value)
{
	return value.type == PVInt || value.type == PVFloat;
}

// eq_conform_pvalues -- Make the types of two program expression values conform for doing
// equality operations.
//--------------------------------------------------------------------------------------------------
//void eq_conform_pvalues (PValue val1, PValue val2, bool* eflg)
//{
//    // If we start off in error, return with that error.
//    if (*eflg) return;
//    // If either value is null, it is an error.
//    if (!val1 || !val2) { *eflg = true; return; }
//
//    // If the values have the same type they are equatable.
//    if (ptype(val1) == ptype(val2)) return;
//
//    if (ptype(val1) == PINT && pvalue(val1) == 0 && !numericPValue(val2))
//        ptype(val1) = ptype(val2);
//
//    if (ptype(val2) == PINT && pvalue(val2) == 0 && !numericPValue(val1))
//        ptype(val2) = ptype(val1);
//
//    if (ptype(val1) == ptype(val2)) return;
//
//    if (numericPValue(val1) && numericPValue(val2)) {
//        int hitype = max(ptype(val1), ptype(val2));
//        if (ptype(val1) != hitype) coercePValue(hitype, val1, eflg);
//        if (ptype(val2) != hitype) coercePValue(hitype, val2, eflg);
//        return;
//    }
//    *eflg = true;
//}

// coercePValue -- Convert program expression PValue from one type to another.
//--------------------------------------------------------------------------------------------------
void coercePValue(PValue* pvalue, int newType, bool* eflg)
{
	int curType = pvalue->type;
	if (curType == newType) return;  //  Nothing to do.

	// Handle PVInt to PVFloat.
	if (newType == PVFloat && curType == PVInt) {
		pvalue->type = newType;
		pvalue->value.uFloat = (float) pvalue->value.uInt;
	}
	printf("DID NOT COERCE FROM %d to %d\n", curType, newType);
	return;
//    if (*eflg) return;
//    if (!is_pvalue(pvalue)) { *eflg = true; return; }
//    // If the types are the same there is nothing to do.
//    if (type == pvalue->type) return;
//    VUnion u;
//    u.wvalue = pvalue->newValue.wvalue;
//    // Handle PVBool -- anything can be coerced to boolean.
//    if (type == PBOOL) {
//        set_pvalue(pvalue, PBOOL, (Word)(long)(u.wvalue != null));
//        return;
//    }
//    if (type == PANY) {    // Handle PANY as a special case.
//        pvalue->type = PANY;
//        return;
//    }
//    if ((pvalue->type == PVAny || pvalue->type == PVInt) && pvalue->newValue.wvalue == null) {
//        switch (type) {
//            case PVInt:
//            case PVBool:
//            case PVString:        // TODO: RECONSIDER THIS.
//                pvalue->type = type;
//                return;
//            case PVFloating:
//                ptype(pvalue) = type;
//                u.fvalue = 0.0;
//                pvalue->newValue.wvalue = u.wvalue;
//                return;
//            default:
//                *eflg = true;
//                return;
//        }
//    }
//
//    switch (ptype(pvalue)) {
//        case PINT:
//            switch (type) {
//                case PINT: return;
//                case PFLOAT: u.fvalue = u.ivalue; break;
//                default: goto bad;
//            }
//            break;
//        case PFLOAT:
//            switch (type) {
//                case PINT: u.ivalue = u.fvalue; break;
//                case PFLOAT: return;
//                default: goto bad;
//            }
//            break;
//        case PBOOL:
//            switch (type) {
//                case PINT: u.ivalue = bool_to_int(u.wvalue); break;
//                case PFLOAT: u.fvalue = bool_to_float(u.wvalue); break;
//                default: goto bad;
//            }
//            break;
//        case PINDI:
//            goto bad;
//        case PANY:
//            goto bad;
//        case PGNODE:
//            goto bad;
//        default:
//            goto bad;
//    }
//    ptype(pvalue) = type;
//    pvalue->newValue.wvalue = u.wvalue;
//    return;
//bad:
//    *eflg = true;
//    return;
}

//  isPValue -- Check a program value for validity -- only checks the type.
//--------------------------------------------------------------------------------------------------
bool isPValue(PValue pvalue)
{
	return pvalue.type >= PVNull && pvalue.type <= PVSequence;
}

//  isRecordType -- Check if a program value type is a Gedcom node type.
//--------------------------------------------------------------------------------------------------
bool isRecordType(PVType type)
{
	return type >= PVPerson && type <= PVOther;
}

//  addPValues -- Add two program values and return their sum.
//--------------------------------------------------------------------------------------------------
PValue addPValues (PValue val1, PValue val2, bool* eflg)
{
	PVType type1 = val1.type, type2 = val2.type;
	if (type1 != type2 || (type1 != PVInt && type1 != PVFloat)) {
		*eflg = true;
		return nullPValue;
	}
	if (type1 == PVInt)
		return PVALUE(PVInt, uInt, val1.value.uInt+val2.value.uInt);
	else
		return PVALUE(PVFloat, uFloat, val1.value.uFloat+val2.value.uFloat);
}

//  subPValues -- Subtract two program values and return their difference.
//--------------------------------------------------------------------------------------------------
PValue subPValues (PValue val1, PValue val2, bool* eflg)
{
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

// mulPValues -- Multiply two program values and return their product.
//--------------------------------------------------------------------------------------------------
PValue mulPValues (PValue val1, PValue val2, bool* eflg)
{
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

// divPValues -- Divide two PValues and return the quotient.
//--------------------------------------------------------------------------------------------------
PValue divPValues(PValue val1, PValue val2, bool* eflg)
{
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

//  modPValues -- Take the modulus of two PValues. Both values must be integers.
//--------------------------------------------------------------------------------------------------
PValue modPValues (PValue val1, PValue val2, bool* eflg)
{
	PVType type1 = val1.type, type2 = val2.type;
	if (type1 != type2 || type2 != PVInt || isZero(val2)) {
		*eflg = true;
		return nullPValue;
	}
	return PVALUE(PVInt, uInt, val1.value.uInt%val2.value.uInt);
}

//  exp_pvalues -- Exponentiation. Raise the first number to the power of the second. They must
//    both be integers.
//--------------------------------------------------------------------------------------------------
PValue expPValues (PValue val1, PValue val2, bool* eflg)
{
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

//  iszero -- Return true if a numeric PValue is zero. Otherwise return false.
//--------------------------------------------------------------------------------------------------
static bool isZero(PValue value)
{
	switch (value.type) {
		case PVInt: return value.value.uInt == 0;
		case PVFloat: return value.value.uFloat == 0.0;
		default: return false;
	}
}

// eqPValues -- See if two PValue are equal.
//--------------------------------------------------------------------------------------------------
PValue eqPValues (PValue val1, PValue val2, bool* eflg)
// val1 -- Left operand.
// val2 -- Right operand.
// eflg -- Returned error flag.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt == val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat = val2.value.uFloat; break;
		case PVString: b = eqstr(val1.value.uString, val2.value.uString); break;
		// TODO: ADD MORE CASES AS DEVELOPMENT CONTINUES. I.E., THE RECORD TYPES.
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

// nePValues -- See if two PValues are not equal. First PValue is converted to a PVBool value
// with the result. The second PValue is freed.
//--------------------------------------------------------------------------------------------------
PValue nePValues(PValue val1, PValue val2, bool* eflg)
//  val1 -- Left operand.
//  val2 -- Right operand.
//  eflg -- Returned error flag.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt != val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat != val2.value.uFloat; break;
		case PVString: b = nestr(val1.value.uString, val2.value.uString); break;
		// TODO: ADD MORE CASES AS DEVELOPMENT CONTINUES. I.E., THE RECORD TYPES.
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

//  lePValues -- Check <= relation between PValues.
//--------------------------------------------------------------------------------------------------
PValue lePValues(PValue val1, PValue val2, bool* eflg)
//  val1 -- Left operand.
//  val2 -- Right operand.
//  eflg -- Returned error flag.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt <= val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat <= val2.value.uFloat; break;
		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) <= 0); break;
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

//  gePValues -- Check >= relation between PValues.
//--------------------------------------------------------------------------------------------------
PValue gePValues (PValue val1, PValue val2, bool* eflg)
//  val1, val2 -- Left and right operands.
//  eflg -- Returned error flag.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt <= val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat <= val2.value.uFloat; break;
		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) >= 0); break;
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

// ltPValues -- Check < relation between PValues. The operands must have the same
// type.
//--------------------------------------------------------------------------------------------------
PValue ltPValues(PValue val1, PValue val2, bool* eflg)
// val1 -- Left operand.
// val2 -- Right operand.
// eflg -- Returned error flag.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt < val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat < val2.value.uFloat; break;
		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) < 0); break;
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

// gtPValues -- Check > relation between PValues.
//--------------------------------------------------------------------------------------------------
PValue gtPValues(PValue val1, PValue val2, bool* eflg)
// val1 -- Left PValue operand of >.
// val2 -- Left PValue operand of >.
// eflg -- Returned error flag if needed.
{
	//num_conform_pvalues(val1, val2, eflg);  // TODO: GET THIS CHECK OR SOMETHING SIMILAR INCORPORATED
	if (*eflg) return nullPValue;
	bool b = false;  // False until proven true.
	int type = val1.type;
	if (type != val2.type) {
		*eflg = true;
		return nullPValue;
	}
	switch (type) {
		case PVInt: b = val1.value.uInt > val2.value.uInt; break;
		case PVFloat: b = val1.value.uFloat > val2.value.uFloat; break;
		case PVString: b = (strcmp(val1.value.uString, val2.value.uString) > 0); break;
		default: break;
	}
	return PVALUE(PVBool, uBool, b);
}

//  incrPValue -- Increment a PValue. The PValue must be a PVInt, else error flag is set.
//--------------------------------------------------------------------------------------------------
PValue incrPValue(PValue pvalue, bool* eflg)
{
	ASSERT(!*eflg);
	if (pvalue.type == PVInt)
		return PVALUE(PVInt, uInt, pvalue.value.uInt + 1);
		//return (PValue) { .pvType = PVInt, pv(.uInt = pvalue.pvValue.uInt + 1) };
	else {
		*eflg = true;
		return nullPValue;
	}
}

//  decrPValue -- Decrement a PValue. The PValue must be a PVInt, else error flag is set.
//--------------------------------------------------------------------------------------------------
PValue decrPValue(PValue pvalue, bool* eflg)
{
	ASSERT(!*eflg);
	if (pvalue.type == PVInt)
		return PVALUE(PVInt, uInt, pvalue.value.uInt - 1);
		//return (PValue) { .pvType = PVInt, pv(.uInt = pvalue.pvValue.uInt - 1) };
	else {
		*eflg = true;
		return nullPValue;
	}
}

// negPValue -- Negate a PValue. The PValue must be a PVInt or PVFloat, else error flag is set.
//--------------------------------------------------------------------------------------------------
PValue negPValue(PValue value, bool* eflg)
{
	if (*eflg) return nullPValue;
	if (value.type == PVInt)
		return PVALUE(PVInt, uInt, -value.value.uInt);
		//return (PValue) { .pvType = PVInt, pv(.uInt = -value.pvValue.uInt) };
	if (value.type == PVFloat)
		return PVALUE(PVFloat, uFloat, -value.value.uFloat);
		//return (PValue) { .pvType = PVFloat, pv(.uInt = -value.pvValue.uFloat) };
	*eflg = true;
	return nullPValue;
}

//  showPValue -- Debugging routine that shows a PValue.
//--------------------------------------------------------------------------------------------------
void showPValue(PValue pvalue)
{
//    PValue *pvalue = ((Symbol*) element)->value;
//
//    if (!isPValue(pvalue)) {
//        printf("*Not PValue*");
//        return;
//    }
//    PVType type = pvalue.pvType;
//    printf("<%s,", ptypes[type]);
//    VUnion u = pvalue.pvValue;
//    switch (type) {
//        case PVInt: printf("%ld>\n", u.uInt); break;
//        case PVFloat: printf("%f>\n", u.uFloat); break;
//        case PVString: printf("%s>\n", u.uString); break;
//        case PVBool: printf("%s>\n", u.uBool ? "true" : "false"); break;
//        case PVPerson:
//        case PVFamily:
//        case PVSource:
//        case PVEvent:
//        case PVOther:
//        case PVGNode: printf("%s>\n", gnodeToString(u.uGNode, gnodeLevel(u.uGNode)));
//        case PVAny: printf("%p>\n", u.uWord); break;
//        case PVNull: printf("null>\n"); break;
//        default: printf("%p>\n", u.uWord); break;
//    }
}

//  pvalueToString -- Return a string representation of a program value.
//    MNOTE: Returned string is on the heap. Caller has the reponsibility to free it.
//--------------------------------------------------------------------------------------------------
String pvalueToString(PValue pvalue, bool showType)
{
	PVType type = pvalue.type;
	VUnion value = pvalue.value;
	static char scratch[1024];
	char *p = scratch;
	if (showType) {
		sprintf(p, "%s: ", ptypes[type]);
		p += strlen(p);
	}
	switch (type) {
		case PVInt: sprintf(p, "%ld", value.uInt); break;
		case PVFloat: sprintf(p, "%f", value.uFloat); break;
		case PVString: sprintf(p, "%s", value.uString); break;
		case PVBool: sprintf(p, "%s", value.uBool ? "true" : "false"); break;
		case PVPerson:
		case PVFamily:
		case PVSource:
		case PVEvent:
		case PVOther:
		case PVGNode: sprintf(p, "%s", gnodeToString(value.uGNode, gnodeLevel(value.uGNode))); break;
		case PVAny: sprintf(p, "any"); break;
		case PVNull: sprintf(p, "null"); break;
		default: sprintf(p, "not implemented");
	}
	return strsave(scratch);
}
