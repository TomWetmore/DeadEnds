// DeadEnds
//
// pvalue.h is the header file for the program value (PValue) type. PValues are the values of
// expressions in the DeadEnds script language. They are the values stored in symbol tables.
//
// Created by Thomas Wetmore on 15 December 2022.
// Last changed on 17 May 2025.

#ifndef pvalue_h
#define pvalue_h

// Forward references.
typedef struct Sequence Sequence;
typedef struct HashTable PValueTable;
typedef struct GNode GNode;

#include "standard.h"
#include "gnode.h"
#include "pnode.h"
#include "sequence.h"
#include "list.h"
#include "symboltable.h"

// Macros that simplify creating union value and program value constants.
#define PV(x) ((VUnion) { x })
#define PVALUE(type, ufield, uvalue) (PValue){type, PV(.ufield = uvalue)}

// PVType enumerates the types of PValues.
typedef enum {
	PVNull = 0, PVInt, PVFloat, PVBool, PVString, PVGNode, PVPerson,
	PVFamily, PVSource, PVEvent, PVOther, PVList, PVTable, PVSequence
} PVType;



bool isGNodeType(PVType type);

// VUnion is the union that holds PValue values.
typedef union VUnion {
	bool uBool;
	GNode* uGNode;
	long uInt;
	double uFloat;
	String uString;
	List* uList;
	PValueTable* uTable;
	Sequence* uSequence;
	void* uWord;
} VUnion;

// PValue is the type of the script language expressions. It is a value type whenever possible.
typedef struct PValue {
	PVType type;
	VUnion value;
} PValue;

// User Interface to PValues.
PValue createStringPValue(String);
void showPValue(PValue value);
bool isPValue(PValue value);
bool isRecordType(PVType type); // PValue is a GNode root.
bool isGNodeType(PVType type);  // PValue is any GNode.

// Useful PValue constants.
extern const PValue nullPValue;
extern const PValue truePValue;
extern const PValue falsePValue;

extern int Perrors;
extern int nobuiltins;

#define UNBUFFERED 2    //  Output unbuffered to a file.
#define BUFFERED   3    //  Output buffered to a file.
#define PAGEMODE   4    //  Output page buffered to a file.

#define TYPE_CHECK(t, v) \
if (ptype(v) != t) {\
*eflg = true;\
return NULL;\
}

// PValue Arithmetic and Logic Functions.
PValue addPValues(PValue, PValue, bool* eflag);
PValue subPValues(PValue, PValue, bool* eflag);
PValue mulPValues(PValue, PValue, bool* eflag);
PValue divPValues(PValue, PValue, bool* eflag);
PValue modPValues(PValue, PValue, bool* eflag);
PValue negPValue(PValue, bool* eflag);
PValue decrPValue(PValue, bool* eflag);
PValue incrPValue(PValue, bool* eflag);
PValue expPValues(PValue, PValue, bool* eflag);
PValue gtPValues(PValue, PValue, bool* eflag);
PValue gePValues(PValue, PValue, bool* eflag);
PValue ltPValues(PValue, PValue, bool* eflag);
PValue lePValues(PValue, PValue, bool* eflag);
PValue nePValues(PValue, PValue, bool* eflag);
PValue eqPValues(PValue, PValue, bool* eflag);

// PValue Functions.
void coercePValue(PValue* pvalue, int newType, bool* errorFlag);
PValue* clonePValue(const PValue* original);
PValue cloneAndReturnPValue(const PValue* original);
//PValue copyPValue(PValue);
//void deletePValue(PValue);
//bool eqv_pvalues(PValue, PValue);

//bool is_record_pvalue(PValue);
bool is_zero(PValue);
//void num_conform_pvalues(PValue, PValue, bool*);
//bool numericPValue(PValue);
//void pvalues_begin(void);
//void pvalues_end(void);
void setPValue(PValue, int, VUnion);
//String pvalue_to_string(PValue);
//void insert_pvtable(Table, String, int, VPTR);
//void remove_pvtable(Table);

String pvalueToString(PValue pvalue, bool showType);

// Functions that allocate and free PValues.
PValue* allocPValue(PVType type, VUnion value);
void freePValue(PValue* pvalue);

#endif // pvalue_h
