//
//  DeadEnds
//
//  pvalue.h -- PValues are the values of expressions in the DeadEnds programming language.
//    They are also the values stored in symbol tables and may be stored in the value fields
//    of sequences. Only the values in symbol tables or other structures are allowed to be
//    allocated on the heap. All values used during evaluation must be in the stack or data
//    spaces.
//
//  Created by Thomas Wetmore on 15 December 2022.
//  Last changed on 13 October 2023.
//

#ifndef pvalue_h
#define pvalue_h

//  Forward references.
typedef struct Sequence Sequence;
typedef struct HashTable PValueTable;
typedef struct GNode GNode;

#include "standard.h"
#include "gnode.h"
#include "pnode.h"
#include "sequence.h"
#include "list.h"
#include "symboltable.h"

//  Macros that simplify creating union value and program value constants.
//--------------------------------------------------------------------------------------------------
#define PV(x) ((VUnion) { x })
#define PVALUE(type, ufield, uvalue) (PValue){type, PV(.ufield = uvalue)}

//  PVType -- Enumeration of the types of PValues.
//--------------------------------------------------------------------------------------------------
typedef enum {
	PVNull = 0, PVAny, PVInt, PVFloat, PVBool, PVString, PVGNode, PVPerson,
	PVFamily, PVSource, PVEvent, PVOther, PVList, PVTable, PVSequence
} PVType;

bool isGNodeType(PVType type);

// VUnion -- Union that holds PValue values.
//--------------------------------------------------------------------------------------------------
typedef union VUnion {
	bool uBool;           // Value if PVBool.
	GNode* uGNode;        // Value if PVGNode, PVPerson, PVFamily, PVSource, PVEvent, PVOther.
	long uInt;            // Value if PVInt.
	double uFloat;        // Value if PVFloat.
	String uString;       // Value if PVString.
	List *uList;          // Value if PVList.
	PValueTable *uTable;  // Value if PVTable.
	Sequence *uSequence;  // Value if PVSequence.
	Word uWord;           // Value is anything else?
} VUnion;

//  PValue -- Values of the programming language expressions. This used to be a pointer type. It
//   has been changed to a value type. However some of the fields in a PValue's VUnion value are
//   pointers whose memory must be paid attention to.
//--------------------------------------------------------------------------------------------------
typedef struct PValue {
	PVType type;    // Type of this PValue.
	VUnion value;   // Value of this PValue.
} PValue;

// User Interface to PValues.
//--------------------------------------------------------------------------------------------------
//PValue createPValue(PVType type, VUnion value);
void showPValue(PValue value);
bool isPValue(PValue value);
bool isRecordType(PVType type);  // Return whether a PValue type is a Gedcom record type.
bool isGNodeType(PVType type);  // Return whether a PValue is any kind of Gedcom line.

//extern bool prog_debug;

//extern int Plineno;
//extern FILE *Pinfp;
extern FILE *Poutfp;
extern int Perrors;
extern int nobuiltins;

extern const PValue nullPValue;
extern const PValue truePValue;
extern const PValue falsePValue;

//  Input and output modes.
#define FILEMODE   0    //  Input from a file.
#define STRINGMODE 1    //  Input or output from or to a string.
#define UNBUFFERED 2    //  Output unbuffered to a file.
#define BUFFERED   3    //  Output buffered to a file.
#define PAGEMODE   4    //  Output page buffered to a file.

#define TYPE_CHECK(t, v) \
if (ptype(v) != t) {\
*eflg = true;\
return NULL;\
}

// PValue Arithmetic and Logic Functions.
//--------------------------------------------------------------------------------------------------
PValue addPValues(PValue, PValue, bool* eflag);  //  Add two program values.
PValue subPValues(PValue, PValue, bool* eflag);  //  Subtract two program values.
PValue mulPValues(PValue, PValue, bool* eflag);  //  Multiply two program values.
PValue divPValues(PValue, PValue, bool* eflag);  //  Divide two program values.
PValue modPValues(PValue, PValue, bool* eflag);  //  Modulus operation on two program values.
PValue negPValue(PValue, bool* eflag);           //  Negate a program value.
PValue decrPValue(PValue, bool* eflag);          //  Decrement the value of an identifier.
PValue incrPValue(PValue, bool* eflag);          //  Increment the value of an identifier.
PValue expPValues(PValue, PValue, bool* eflag);
PValue gtPValues(PValue, PValue, bool* eflag);
PValue gePValues(PValue, PValue, bool* eflag);
PValue ltPValues(PValue, PValue, bool* eflag);
PValue lePValues(PValue, PValue, bool* eflag);
PValue nePValues(PValue, PValue, bool* eflag);
PValue eqPValues(PValue, PValue, bool* eflag);

// PValue Functions.
void coercePValue(PValue* pvalue, int newType, bool* errorFlag);
//PValue copyPValue(PValue);
//void deletePValue(PValue);
//void eq_conform_pvalues(PValue, PValue, bool*);
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

//  Functions that allocate and free PValues in the heap. These are intended only for use for
//   symbol table values and possibly sequences.
//--------------------------------------------------------------------------------------------------
PValue* allocPValue(PVType type, VUnion value);
void freePValue(PValue* pvalue);

#endif // pvalue_h
