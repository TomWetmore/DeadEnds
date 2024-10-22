// DeadEnds
//
// pvalue.c holds the functions that handle program expression values (PValues) when interpreting
// DeadEnds scripts.
//
// Created by Thomas Wetmore on 15 December 2022.
// Last changed on 21 October 2024.

#include "pvalue.h"
#include "standard.h"
#include "gedcom.h"

extern const PValue nullPValue;  // Defined in builtin.c

// isPVGNodeType return true if a PVType is one of the GNode types.
bool isGNodeType(PVType type) {
	return type >= PVGNode && type <= PVOther;
}

// ptypes is the array of PValue type names for debugging.
static char *ptypes[] = {
	"PVNull", "PVAny", "PVInt", "PVFloat", "PVBool", "PVString", "PVGNode", "PVPerson",
	"PVFamily", "PVSource", "PVEvent", "PVOther", "PVList", "PVTable", "PVSequence"};

static bool isZero(PValue);
bool isZeroVUnion(PVType, VUnion);

// allocPValue allocates a PValue. PValues are usually value types, but when stored in tables or
// lists they must be in the heap.
PValue *allocPValue(PVType type, VUnion value) {
	PValue* ppvalue = (PValue*) stdalloc(sizeof(PValue));
	ppvalue->type = type;
	ppvalue->value = value;
	return ppvalue;
}

// freePValue frees an allocated PValue.
void freePValue(PValue* ppvalue) {
	switch (ppvalue->type) {
		case PVString:
			if (ppvalue->value.uString) stdfree(ppvalue->value.uString);
			break;
		case PVSequence:
			//deleteSequence(ppvalue->value.uSequence);
			// MNOTE: Possible memory leak, but fixes subtle bug.
			break;
		default:
			break;
	}
	stdfree(ppvalue);
}

// copyPValue copies a PValue.
PValue *copyPValue(PValue pvalue) {
	PValue *ppvalue = (PValue*) stdalloc(sizeof(ppvalue));
	memcpy(ppvalue, &pvalue, sizeof(PValue));
	return ppvalue;
}

// setPValue -- Set a program value.
// TODO: THIS ISN'T GOING TO WORK ANYMORE.
void setPValue(PValue pvalue, int type, VUnion value) {
	if (pvalue.type == PVString && pvalue.value.uString) stdfree(pvalue.value.uString);
	pvalue.type = type;
	if (type == PVString && value.uString) value.uString = strsave(value.uString);
	pvalue.value = value;
}

// numericPValue checks if a PValue has numeric type.
bool numericPValue(PValue value) {
	return value.type == PVInt || value.type == PVFloat;
}

// coercePValue converts a PValue from one type to another.
// THIS FUNCTION IS NOT CALLED.
void coercePValue(PValue* pvalue, int newType, bool* eflg) {
	int curType = pvalue->type;
	if (curType == newType) return; // Nothing to do.
	if (newType == PVFloat && curType == PVInt) { // PVInt to PVFloat.
		pvalue->type = newType;
		pvalue->value.uFloat = (float) pvalue->value.uInt;
	}
	if (newType == PVBool) { // Any to PVBool.
		pvalue->type = newType;
		pvalue->value.uBool = ((void*) pvalue->value.uGNode) != null;
	}
	printf("DID NOT COERCE FROM %d to %d\n", curType, newType);
	return;
}

//  isPValue checks a PValue for validity by checking its type.
bool isPValue(PValue pvalue) {
	return pvalue.type >= PVNull && pvalue.type <= PVSequence;
}

// isRecordType checks if a PValue is a Gedcom node type.
bool isRecordType(PVType type) {
	return type >= PVPerson && type <= PVOther;
}

// addPValues adds two PValues and returns their sum.
PValue addPValues(PValue val1, PValue val2, bool* eflg) {
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
PValue eqPValues(PValue val1, PValue val2, bool* eflg) {
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

// ltPValues checks the less than relation between PValues.
PValue ltPValues(PValue val1, PValue val2, bool* eflg) {
	bool b = false;
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

// gtPValues checks the greater than relation between PValues.
PValue gtPValues(PValue val1, PValue val2, bool* eflg) {
	bool b = false;
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

// incrPValue increments a PValue which must be an integer.
PValue incrPValue(PValue pvalue, bool* eflg) {
	if (pvalue.type == PVInt)
		return PVALUE(PVInt, uInt, pvalue.value.uInt + 1);
	else {
		*eflg = true;
		return nullPValue;
	}
}

// decrPValue decrement a PValue which must be an integer.
PValue decrPValue(PValue pvalue, bool* eflg) {
	if (pvalue.type == PVInt)
		return PVALUE(PVInt, uInt, pvalue.value.uInt - 1);
	else {
		*eflg = true;
		return nullPValue;
	}
}

// negPValue negates a PValue which must be numeric.
PValue negPValue(PValue value, bool* eflg) {
	if (value.type == PVInt)
		return PVALUE(PVInt, uInt, -value.value.uInt);
	if (value.type == PVFloat)
		return PVALUE(PVFloat, uFloat, -value.value.uFloat);
	*eflg = true;
	return nullPValue;
}

// showPValue shows the value of a PValue for debugging.
void showPValue(PValue pvalue)
{
	printf("%s\n", pvalueToString(pvalue, true));
}

// pvalueToString returns a String representation of a PValue. Caller must free the String.
String pvalueToString(PValue pvalue, bool showType) {
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
