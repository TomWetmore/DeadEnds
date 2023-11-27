//
//  DeadEnds
//
//  functable.c -- Table of the built-in functions in the DeadEnds programming language.
//
//  Created by Thomas Wetmore on 10 January 2023.
//  Last changed on 16 November 2023.
//

#include "standard.h"
#include "symboltable.h"
#include "gedcom.h"
#include "interp.h"

extern PValue __add(PNode*, Context*, bool*);
extern PValue __addnode(PNode*, Context*, bool*);
extern PValue __addtoset(PNode*, Context*, bool*);
extern PValue __alpha(PNode*, Context*, bool*);
extern PValue __ancestorset(PNode*, Context*, bool*);
extern PValue __and(PNode*, Context*, bool*);
extern PValue __baptism(PNode*, Context*, bool*);
extern PValue __birth(PNode*, Context*, bool*);
extern PValue __burial(PNode*, Context*, bool*);
extern PValue __capitalize(PNode*, Context*, bool*);
extern PValue __card(PNode*, Context*, bool*);
extern PValue __child(PNode*, Context*, bool*);
//extern PValue __children(PNode*, Context*, bool*);  // NEW TO DEADENDS.
extern PValue __childset(PNode*, Context*, bool*);
extern PValue __choosechild(PNode*, Context*, bool*);
extern PValue __choosefam(PNode*, Context*, bool*);
extern PValue __chooseindi(PNode*, Context*, bool*);
extern PValue __choosespouse(PNode*, Context*, bool*);
extern PValue __choosesubset(PNode*, Context*, bool*);
extern PValue __col(PNode*, Context*, bool*);
extern PValue __concat(PNode*, Context*, bool*);
extern PValue __copyfile(PNode*, Context*, bool*);
extern PValue __createnode(PNode*, Context*, bool*);
extern PValue __d(PNode*, Context*, bool*);
extern PValue __database(PNode*, Context*, bool*);
extern PValue __date(PNode*, Context*, bool*);
extern PValue __dateformat(PNode*, Context*, bool*);
extern PValue __dayformat(PNode*, Context*, bool*);
extern PValue __death(PNode*, Context*, bool*);
extern PValue __decr(PNode*, Context*, bool*);
extern PValue __deletefromset(PNode*, Context*, bool*);
extern PValue __deletenode(PNode*, Context*, bool*);
extern PValue __dequeue(PNode*, Context*, bool*);
extern PValue __descendentset(PNode*, Context*, bool*);
extern PValue __difference(PNode*, Context*, bool*);
extern PValue __div(PNode*, Context*, bool*);
extern PValue __empty(PNode*, Context*, bool*);
extern PValue __eq(PNode*, Context*, bool*);
extern PValue __eqstr(PNode*, Context*, bool*);
extern PValue __exp(PNode*, Context*, bool*);
extern PValue __extractdate(PNode*, Context*, bool*);
extern PValue __extractnames(PNode*, Context*, bool*);
extern PValue __extractplaces(PNode*, Context*, bool*);
extern PValue __extracttokens(PNode*, Context*, bool*);
extern PValue __f(PNode*, Context*, bool*);
extern PValue __fam(PNode*, Context*, bool*);
extern PValue __father(PNode*, Context*, bool*);
extern PValue __female(PNode*, Context*, bool*);
extern PValue __firstchild(PNode*, Context*, bool*);
extern PValue __firstfam(PNode*, Context*, bool*);
extern PValue __firstindi(PNode*, Context*, bool*);
extern PValue __fnode(PNode*, Context*, bool*);
extern PValue __fullname(PNode*, Context*, bool*);
extern PValue __ge(PNode*, Context*, bool*);
extern PValue __gengedcom(PNode*, Context*, bool*);
extern PValue __genindiset(PNode*, Context*, bool*);
extern PValue __getel(PNode*, Context*, bool*);
extern PValue __getfam(PNode*, Context*, bool*);
extern PValue __getindi(PNode*, Context*, bool*);
extern PValue __getindiset(PNode*, Context*, bool*);
extern PValue __getint(PNode*, Context*, bool*);
extern PValue __getrecord(PNode*, Context*, bool*);
extern PValue __getstr(PNode*, Context*, bool*);
extern PValue __gettoday(PNode*, Context*, bool*);
extern PValue __givens(PNode*, Context*, bool*);
extern PValue __gt(PNode*, Context*, bool*);
extern PValue __husband(PNode*, Context*, bool*);
extern PValue __incr(PNode*, Context*, bool*);
extern PValue __index(PNode*, Context*, bool*);
extern PValue __indi(PNode*, Context*, bool*);
extern PValue __indiset(PNode*, Context*, bool*);
extern PValue __inode(PNode*, Context*, bool*);
extern PValue __insert(PNode*, Context*, bool*);
extern PValue __intersect(PNode*, Context*, bool*);
extern PValue __key(PNode*, Context*, bool*);
extern PValue __keysort(PNode*, Context*, bool*);
extern PValue __lastchild(PNode*, Context*, bool*);
extern PValue __le(PNode*, Context*, bool*);
extern PValue __length(PNode*, Context*, bool*);
extern PValue __lengthset(PNode*, Context*, bool*);
extern PValue __linemode(PNode*, Context*, bool*);
extern PValue __list(PNode*, Context*, bool*);
extern PValue __lock(PNode*, Context*, bool*);
extern PValue __long(PNode*, Context*, bool*);
extern PValue __lookup(PNode*, Context*, bool*);
extern PValue __lower(PNode*, Context*, bool*);
extern PValue __lt(PNode*, Context*, bool*);
extern PValue __male(PNode*, Context*, bool*);
extern PValue __marriage(PNode*, Context*, bool*);
extern PValue __menuchoose(PNode*, Context*, bool*);
extern PValue __mod(PNode*, Context*, bool*);
extern PValue __monthformat(PNode*, Context*, bool*);
extern PValue __mother(PNode*, Context*, bool*);
extern PValue __mul(PNode*, Context*, bool*);
extern PValue __name(PNode*, Context*, bool*);
extern PValue __namesort(PNode*, Context*, bool*);
extern PValue __nchildren(PNode*, Context*, bool*);
extern PValue __ne(PNode*, Context*, bool*);
extern PValue __neg(PNode*, Context*, bool*);
extern PValue __newfile(PNode*, Context*, bool*);
extern PValue __nextfam(PNode*, Context*, bool*);
extern PValue __nextindi(PNode*, Context*, bool*);
extern PValue __nextsib(PNode*, Context*, bool*);
extern PValue __nfamilies(PNode*, Context*, bool*);
extern PValue __nl(PNode*, Context*, bool*);
extern PValue __not(PNode*, Context*, bool*);
extern PValue __nspouses(PNode*, Context*, bool*);
extern PValue __or(PNode*, Context*, bool*);
extern PValue __ord(PNode*, Context*, bool*);
extern PValue __outfile(PNode*, Context*, bool*);
extern PValue __pagemode(PNode*, Context*, bool*);
extern PValue __pageout(PNode*, Context*, bool*);
extern PValue __parent(PNode*, Context*, bool*);
extern PValue __parents(PNode*, Context*, bool*);
extern PValue __parentset(PNode*, Context*, bool*);
extern PValue __place(PNode*, Context*, bool*);
extern PValue __pn(PNode*, Context*, bool*);
extern PValue __pop(PNode*, Context*, bool*);
extern PValue __pos(PNode*, Context*, bool*);
extern PValue __prevfam(PNode*, Context*, bool*);
extern PValue __previndi(PNode*, Context*, bool*);
extern PValue __prevsib(PNode*, Context*, bool*);
extern PValue __print(PNode*, Context*, bool*);
extern PValue __push(PNode*, Context*, bool*);
extern PValue __qt(PNode*, Context*, bool*);
extern PValue __reference(PNode*, Context*, bool*);
extern PValue __requeue(PNode*, Context*, bool*);
extern PValue __rjustify(PNode*, Context*, bool*);
extern PValue __roman(PNode*, Context*, bool*);
extern PValue __rot(PNode*, Context*, bool*);
extern PValue __row(PNode*, Context*, bool*);
extern PValue __save(PNode*, Context*, bool*);
extern PValue __savenode(PNode*, Context*, bool*);
extern PValue __set(PNode*, Context*, bool*);
extern PValue __setel(PNode*, Context*, bool*);
extern PValue __sex(PNode*, Context*, bool*);
extern PValue __short(PNode*, Context*, bool*);
extern PValue __sibling(PNode*, Context*, bool*);
extern PValue __siblingset(PNode*, Context*, bool*);
extern PValue __soundex(PNode*, Context*, bool*);
extern PValue __space(PNode*, Context*, bool*);
extern PValue __spouseset(PNode*, Context*, bool*);
extern PValue __stddate(PNode*, Context*, bool*);
extern PValue __strcmp(PNode*, Context*, bool*);
extern PValue __strlen(PNode*, Context*, bool*);
extern PValue __strsoundex(PNode*, Context*, bool*);
extern PValue __strtoint(PNode*, Context*, bool*);
extern PValue __sub(PNode*, Context*, bool*);
extern PValue __substring(PNode*, Context*, bool*);
extern PValue __surname(PNode*, Context*, bool*);
extern PValue __system(PNode*, Context*, bool*);
extern PValue __table(PNode*, Context*, bool*);
extern PValue __tag(PNode*, Context*, bool*);
extern PValue __title(PNode*, Context*, bool*);
extern PValue __trim(PNode*, Context*, bool*);
extern PValue __trimname(PNode*, Context*, bool*);
extern PValue __union(PNode*, Context*, bool*);
extern PValue __uniqueset(PNode*, Context*, bool*);
extern PValue __unlock(PNode*, Context*, bool*);
extern PValue __upper(PNode*, Context*, bool*);
extern PValue __value(PNode*, Context*, bool*);
extern PValue __valuesort(PNode*, Context*, bool*);
extern PValue __version(PNode*, Context*, bool*);
extern PValue __wife(PNode*, Context*, bool*);
extern PValue __xref(PNode*, Context*, bool*);
extern PValue __year(PNode*, Context*, bool*);

extern PValue __noop(PNode*, Context*, bool*);

BuiltIn builtIns[] = {
    "add",        2,    32,    __add,
//    "addnode",    3,    3,    __addnode,
    "addtoset",    3,    3,    __addtoset,
    "alpha",    1,    1,    __alpha,
    "ancestorset",    1,    1,    __ancestorset,
    "and",        2,    32,    __and,
//    "atoi",        1,    1,    __strtoint,
    "baptism",    1,    1,    __baptism,
    "birth",      1,    1,    __birth,
    "burial",     1,    1,    __burial,
    "capitalize", 1,    1,    __capitalize,
    "card",       1,    1,    __card,
    "child",      1,    1,    __child,
//    "children", 1,  1,  __children,
    "childset",    1,    1,    __childset,
//    "choosechild",    1,    1,    __choosechild,
//    "choosefam",    1,    1,    __choosefam,
//    "chooseindi",    1,    1,    __chooseindi,
//    "choosespouse",    1,    1,    __choosespouse,
//    "choosesubset",    1,    1,    __choosesubset,
//    "col",        1,    1,    __col,
//    "concat",    2,    32,    __concat,
    "copyfile",    1,    1,     __copyfile,
    "createnode",    2,    2,   __createnode,
    "d",            1,    1,    __d,
    "database",     0,    1,    __noop,
//    "date",        1,    1,    __date,
    "dateformat",   1,    1,    __dateformat,
    "dayformat",    1,    1,    __dayformat,
    "death",        1,    1,    __death,
    "decr",         1,    1,    __decr,
//    "deletefromset",3,    3,    __deletefromset,
//    "deletenode",    1,    1,    __deletenode,
    "dequeue",    1,    1,    __dequeue,
//    "dereference",    1,    1,    __getrecord,
    "descendantset",1,    1,    __descendentset,
    "descendentset",1,    1,    __descendentset,
    "difference",    2,    2,    __difference,
     "div",        2,    2,    __div,
     "empty",    1,    1,    __empty,
    "enqueue",    2,    2,    __push,
    "eq",        2,    2,    __eq,
    "eqstr",    2,    2,    __eqstr,
    "exp",        2,    2,    __exp,
//    "extractdate",    4,    4,    __extractdate,
//    "extractnames",    4,    4,    __extractnames,
//    "extractplaces",3,    3,    __extractplaces,
//    "extracttokens",4,    4,    __extracttokens,
    "f",      1,    1,  __f,
    "fam",       1,    1,    __fam,
    "father",    1,    1,    __father,
    "female",    1,    1,    __female,
    "firstchild",    1,    1,    __firstchild,
//    "firstfam",    1,    1,    __firstfam,
    "firstindi",    0,    0,    __firstindi,
//    "fnode",    1,    1,    __fnode,
    "fullname",    4,    4,    __fullname,
    "ge",        2,    2,    __ge,
    "gengedcom",    1,    1,    __gengedcom,
//    "genindiset",    2,    2,    __genindiset,
    "getel",    2,    2,    __getel,
//    "getfam",    1,    1,    __getfam,
//    "getindi",    1,    2,    __getindi,
//    "getindimsg",    2,    2,    __getindi,
//    "getindiset",    1,    2,    __getindiset,
//    "getint",    1,    2,    __getint,
//    "getintmsg",    2,    2,    __getint,
//    "getrecord",    1,    1,    __getrecord,
//    "getstr",    1,    2,    __getstr,
//    "getstrmsg",    2,    2,    __getstr,
//    "gettoday",    0,    0,    __gettoday,
    "givens",     1,    1,    __givens,
    "gt",         2,    2,    __gt,
    "husband",    1,    1,    __husband,
    "incr",       1,    1,    __incr,
//    "index",    3,    3,    __index,
    "indi",       1,    1,    __indi,
    "indiset",    1,    1,    __indiset,
    "inode",      1,    1,    __inode,
    "insert",    3,    3,    __insert,
    "intersect",  2,    2,    __intersect,
    "key",        1,    2,    __key,
    "keysort",    1,    1,    __keysort,
    "lastchild",  1,    1,    __lastchild,
    "le",         2,    2,    __le,
    "length",    1,    1,    __length,
    "lengthset",   1,    1,    __lengthset,
//    "linemode",    0,    0,    __linemode,
    "list",        1,    1,    __list,
    "lock",        1,    1,    __noop,
    "long",        1,    1,    __long,
    "lookup",    2,    2,    __lookup,
    "lower",    1,    1,    __lower,
    "lt",        2,    2,    __lt,
    "male",        1,    1,    __male,
    "marriage",    1,    1,    __marriage,
//    "menuchoose",    1,    2,    __menuchoose,
    "mod",        2,    2,    __mod,
    "monthformat",    1,    1,    __monthformat,
    "mother",    1,    1,    __mother,
    "mul",        2,    32,    __mul,
    "name",        1,    2,    __name,
    "namesort",    1,    1,    __namesort,
    "nchildren",    1,    1,    __nchildren,
    "ne",        2,    2,    __ne,
    "neg",        1,    1,    __neg,
//    "nestr",    2,    2,    __strcmp,
//    "newfile",    2,    2,    __newfile,
//    "nextfam",    1,    1,    __nextfam,
    "nextindi",    1,    1,    __nextindi,
    "nextsib",    1,    1,    __nextsib,
//    "nfamilies",    1,    1,    __nfamilies,
    "nl",        0,    0,    __nl,
    "not",        1,    1,    __not,
    "nspouses",    1,    1,    __nspouses,
    "or",        2,    32,    __or,
    "ord",        1,    1,    __ord,
//    "outfile",    0,    0,    __outfile,
//    "pagemode",    2,    2,    __pagemode,
//    "pageout",    0,    0,    __pageout,
    "parent",    1,    1,    __parent,
//    "parents",    1,    1,    __parents,
    "parentset",    1,    1,    __parentset,
    "place",    1,    1,    __place,
    "pn",        2,    2,    __pn,  // Outputs pronouns
    "pop",        1,    1,    __pop,
//    "pos",        2,    2,    __pos,
//    "prevfam",    1,    1,    __prevfam,
//    "previndi",    1,    1,    __previndi,
     "prevsib",    1,    1,    __prevsib,
//    "print",    1,    32,    __print,
    "push",        2,    2,    __push,
    "qt",        0,    0,    __qt,
//    "reference",    1,    1,    __reference,
//    "requeue",    2,    2,    __requeue,
//    "rjustify",    2,    2,    __rjustify,
    "roman",    1,    1,    __roman,
//    "root",        1,    1,    __rot,
//    "row",        1,    1,    __row,
//    "save",        1,    1,    __save,
//    "savenode",    1,    1,    __savenode,
    "set",        2,    2,    __set,
    "setel",    3,    3,    __setel,
    "sex",        1,    1,    __sex,
    "short",    1,    1,    __short,  // Short form of an event.
    "sibling",    1,    1,    __sibling,
    "siblingset",    1,    1,    __siblingset,
//    "soundex",    1,    1,    __soundex,
    "sp",        0,    0,    __space,
    "spouseset",    1,    1,    __spouseset,
    "stddate",    1,    1,    __stddate,
    "strcmp",    2,    2,    __strcmp,
//    "strconcat",    2,    32,    __concat,
    "strlen",    1,    1,    __strlen,
//    "strsave",    1,    1,    __save,
    "strsoundex",    1,    1,    __strsoundex,
    "strtoint",    1,    1,    __strtoint,
    "sub",        2,    2,    __sub,
//    "substring",    3,    3,    __substring,
    "surname",    1,    1,    __surname,
//    "system",    1,    1,    __system,
    "table",    1,    1,    __table,
    "tag",        1,    1,    __tag,
    "title",    1,    1,    __title,
//    "trim",        2,    2,    __trim,
    "trimname",    2,    2,    __trimname,
    "union",    2,    2,    __union,
    "uniqueset",    1,    1,    __uniqueset,
    "unlock",    1,    1,    __noop,
    "upper",    1,    1,    __upper,
    "value",    1,    1,    __value,
//    "valuesort",    1,    1,    __valuesort,
    "version",    0,    0,    __version,
    "wife",        1,    1,    __wife,
    "xref",        1,    1,    __xref,
    "year",        1,    1,    __year,
};

int nobuiltins = sizeof(builtIns)/sizeof(builtIns[0]);
