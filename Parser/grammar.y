//
//  DeadEnds
//
//  interp.y -- Grammar and semantic actions for the DeadEnds programming language.
//
//  Created by Thomas Wetmore on 8 December 2022.
//  Last changed 28 September 2023.
//
%{
//#include "llstdlib.h"
#include "lexer.h"
#include "symboltable.h"
#include "functiontable.h"
//#include "gedcom.h"
//#include "cache.h"
//#include "indiseq.h"
#include "list.h"
#include "interp.h"
#include <stdlib.h>

//  Global variables that form the channel between the lexer, parser and interpreter. They should
//    not be mentioned in header files and used as tunnels.
//--------------------------------------------------------------------------------------------------
extern SymbolTable *globalTable;    // Table of global variables.
extern FunctionTable *procedureTable; // Table of user-defined procedures.
extern FunctionTable *functionTable;  // Table of user-defined functions.
extern List *pendingFileList;  // Pending list of included files.
extern int currentProgramLineNumber;  // Current line number in current file.

static PNode *this, *prev;

// Functions defined in the third section.
//--------------------------------------------------------------------------------------------------
static void join(PNode *list, PNode *last);
static void yyerror(String str);

#define YYSTYPE SemanticValue

%}

%token  <integer> ICONS
%token  <floating> FCONS
%token  <string> SCONS
%token  <identifier> IDEN
%token  PROC FUNC_TOK CHILDREN SPOUSES IF ELSE ELSIF
%token  FAMILIES WHILE CALL FORINDISET FORINDI FORNOTES
%token  TRAVERSE FORNODES FORLIST_TOK FORFAM FORSOUR FOREVEN FOROTHR
%token  BREAK CONTINUE RETURN FATHERS MOTHERS PARENTS

%start defns
%type <pnode> idenso idens
%type <pnode> state states
%type <pnode> exprso exprs expr secondo
%type <pnode> elsifso elsifs elsif elseo
%type <integer> m

%%

    // A DeadEnds program is a list of defns.
    defns	:	defn
    |	defns defn
    ;

    // A defn is a procedure, function, global or include.
    defn 	:	proc
    |	func
    |	IDEN '(' IDEN ')' {  // Only interested in "global".
        if (eqstr("global", $1)) {
            assignValueToSymbol(globalTable, $3, (PValue) {PVAny});
            //insertInHashTable(globalTable, $3, allocPValue(PVAny, PV()));
        }
    }
    |	IDEN '(' SCONS ')' {  // Only interested in "include".
        if (eqstr("include", $1))
        prependListElement(pendingFileList, $3);
    }
    ;

    // A proc is "proc", name, params, and body. They are added to the proc table.
    proc	:	PROC IDEN '(' idenso ')' '{' states '}' {
    	printf("Added procedure %s to the procedure table.\n", $2);
        insertInFunctionTable(procedureTable, $2, (Word)procDefPNode($2, $4, $7));
    }
    ;

    // A func is "func", name, parms, and a body with a return expr. They are added to the func table.
    func	:	FUNC_TOK IDEN '(' idenso ')' '{' states '}' {
        insertInFunctionTable(functionTable, $2, (Word)funcDefPNode($2, $4, $7));
    }
    ;
    // idenso -- null if empty, or it is a list of IIDEN PNodes if not.
    idenso	:	/* empty */ {
        $$ = 0;
    }
    |	idens {
        $$ = $1;
    }
    ;

    // Idens is a list of IIDEN PNodes.
    //--------------------------------------------------------------------------------------------------
    idens	:	IDEN {
        $$ = iden_node($1);
    }
    |	IDEN ',' idens {
        $$ = iden_node($1);
        $$->next = $3;
    }
    ;

    // States is a list of statement Pnodes.
    //--------------------------------------------------------------------------------------------------
    states	:	state {
        $$ = $1;
    }
    |	states  state {
        join($1, $2);
        $$ = $1;
    }
    ;
    // State is a statement PNode.
    //--------------------------------------------------------------------------------------------------
    state	:	CHILDREN m '(' expr ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = childrenPNode($4, $6, $8, $11);
        $$->lineNumber = (int)$2;
    }
    |	SPOUSES m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = spousesPNode($4, $6, $8, $10, $13);
        $$->lineNumber = (int)$2;
    }
    |	FAMILIES m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = familiesPNode($4, $6, $8, $10, $13);
        $$->lineNumber = (int)$2;
    }
    |	FATHERS m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = fathersPNode($4, $6, $8, $10, $13);
        $$->lineNumber = (int)$2;
    }
    |	MOTHERS m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = mothersPNode($4, $6, $8, $10, $13);
        $$->lineNumber = (int)$2;
    }
    |	PARENTS m '(' expr ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = parentsPNode($4, $6, $8, $11);
        $$->lineNumber = (int)$2;
    }
    |	FORINDISET m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forindisetPNode($4, $6, $8, $10, $13);
        $$->lineNumber = (int)$2;
    }
    |	FORLIST_TOK m '(' expr ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forlistPNode($4, $6, $8, $11);
        $$->lineNumber = (int)$2;
    }
    |	FORINDI m '(' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forindiPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |   FORNOTES m '(' expr ',' IDEN ')' '{' states '}' {
        $$ = fornotesPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |	FORFAM m '(' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forfamPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |	FORSOUR m '(' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forsourPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |	FOREVEN m '(' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forevenPNode($4, $6,$9);
        $$->lineNumber = (int)$2;
    }
    |	FOROTHR m '(' IDEN ',' IDEN ')' '{' states '}' {
        $$ = forothrPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |	TRAVERSE m '(' expr ',' IDEN ',' IDEN ')' '{' states '}' {
        $$ = traversePNode($4, $6, $8, $11);
        $$->lineNumber = (int)$2;
    }
    |	FORNODES m '(' expr ',' IDEN ')' '{' states '}' {
        $$ = fornodesPNode($4, $6, $9);
        $$->lineNumber = (int)$2;
    }
    |	IF m '(' expr secondo ')' '{' states '}' elsifso elseo {
        $4->next = $5;  // In case there is an identifier first.
        prev = null;  this = $10;
        while (this) {
            prev = this;
            this = this->elseState;
        }
        if (prev) {
            prev->elseState = $11;
            $$ = ifPNode($4, $8, $10);
        } else
        $$ = ifPNode($4, $8, $11);
        $$->lineNumber = (int)$2;
    }
    |	WHILE m '(' expr secondo ')' '{' states '}' {
        $4->next = $5;
        $$ = whilePNode($4, $8);
        $$->lineNumber = (int)$2;
    }
    |	CALL IDEN m '(' exprso ')' {
        $$ = procCallPNode($2, $5);
        $$->lineNumber = (int)$3;
    }
    |	BREAK m '(' ')' {
        $$ = breakPNode();
        $$->lineNumber = (int)$2;
    }
    |	CONTINUE m '(' ')' {
        $$ = continuePNode();
        $$->lineNumber = (int)$2;
    }
    |	RETURN m '(' exprso ')' {
        $$ = returnPNode($4);
        $$->lineNumber = (int)$2;
    }
    |	expr {
        $$ = $1;
    }
    ;
    elsifso	:	    /* empty */ {
        $$ = 0;
    }
    |	elsifs {
        $$ = $1;
    }
    ;
    elsifs	:	elsif {
        $$ = $1;
    }
    |	elsif  elsifs {
        $1->elseState = $2;
        $$ = $1;
    }
    ;
    elsif	:	ELSIF '(' expr secondo ')' '{' states '}' {
        $3->next = $4;
        $$ = ifPNode($3, $7, null);
    }
    ;

    // An elseo is an optional else clause.
    elseo	:	/* empty */ {
        $$ = 0;
    }
    |	ELSE '{' states '}' {
        $$ = $3;
    }
    ;
    expr	:	IDEN {
        $$ = iden_node((String)$1);
        $$->arguments = null;
    }
    |	IDEN m '(' exprso ')' {
        $$ = funcCallPNode($1, $4);
        $$->lineNumber = (int)$2;
    }
    |	SCONS {
        $$ = sconsPNode($1);
    }
    |	ICONS {
        $$ = iconsPNode($1);
    }
    |	FCONS {
        $$ = fconsPNode($1);
    }
    ;
    exprso	:	/* empty */ {
        $$ = 0;
    }
    |	exprs {
        $$ = $1;
    }
    ;
    exprs	:	expr {
        $$ = $1;
    }
    |	expr ',' exprs {
        $1->next = $3;
        $$ = $1;
    }
    ;
    secondo	:	/* empty */ {
        $$ = 0;
    }
    |	',' expr {
        $$ = $2;
    }
    ;
    m	:	/* empty */ {
        $$ = currentProgramLineNumber;
    }

%%

// join -- Join a list of PNodes to another PNode (which may be the start of another list).
//--------------------------------------------------------------------------------------------------
void join (PNode *list, PNode *last)
{
	PNode *prev = null;
	while (list) {
		prev = list;
		list = list->next;
	}
	ASSERT(prev);
	prev->next = last;
}

void yyerror (String str)
{
	extern String currentProgramFileName;

	printf("Syntax Error (%s): %s: line %d\n", str, currentProgramFileName, currentProgramLineNumber);
	Perrors++;
}
