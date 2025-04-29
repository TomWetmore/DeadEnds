#include <stdlib.h>
#include <string.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
#define YYPREFIX "yy"
#line 8 "grammar.y"
#include "lexer.h"
#include "symboltable.h"
#include "functiontable.h"
#include "list.h"
#include "interp.h"
#include <stdlib.h>

/* Global variables that form the interface between the lexer, parser and interpreter.*/
extern SymbolTable *globalTable; /* Global variables.*/
extern FunctionTable *procedureTable;/* User procedures.*/
extern FunctionTable *functionTable; /* User functions.*/
extern List *pendingFiles; /* Pending list of included files.*/
extern int curLine; /* Line number in current file.*/

static PNode *this, *prev, *tnode;;

/* Functions defined in the third section.*/
static void join(PNode* list, PNode* last);
static void yyerror(String str);

#define YYSTYPE SemanticValue
#line 35 "y.tab.c"
#define ICONS 257
#define FCONS 258
#define SCONS 259
#define IDEN 260
#define PROC 261
#define FUNC_TOK 262
#define CHILDREN 263
#define SPOUSES 264
#define IF 265
#define ELSE 266
#define ELSIF 267
#define FAMILIES 268
#define WHILE 269
#define CALL 270
#define FORINDISET 271
#define FORINDI 272
#define FORNOTES 273
#define TRAVERSE 274
#define FORNODES 275
#define FORLIST_TOK 276
#define FORFAM 277
#define FORSOUR 278
#define FOREVEN 279
#define FOROTHR 280
#define BREAK 281
#define CONTINUE 282
#define RETURN 283
#define FATHERS 284
#define MOTHERS 285
#define PARENTS 286
#define YYERRCODE 256
const short yylhs[] =
	{                                        -1,
    0,    0,   16,   16,   16,   16,    3,    4,    1,    1,
    2,    2,    6,    6,    5,    5,    5,    5,    5,    5,
    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
    5,    5,    5,    5,    5,    5,    5,   11,   11,   12,
   12,   13,   14,   14,    9,    9,    9,    9,    9,    7,
    7,    8,    8,   10,   10,   15,
};
const short yylen[] =
	{                                         2,
    1,    2,    1,    1,    4,    4,    9,    9,    0,    1,
    1,    3,    1,    2,   12,   14,   14,   14,   14,   12,
   14,   12,   10,   10,   10,   10,   10,   10,   12,   10,
   11,    9,    6,    4,    4,    5,    1,    0,    1,    1,
    2,    8,    0,    4,    1,    5,    1,    1,    1,    0,
    1,    1,    3,    0,    2,    0,
};
const short yydefred[] =
	{                                      0,
    0,   56,   56,    0,    3,    4,    1,    0,    0,    0,
    2,    0,    0,    0,    0,    6,    5,    0,    0,    0,
    0,   10,    0,    0,    0,    0,   12,    0,    0,   48,
   49,   47,    0,   56,   56,   56,   56,   56,    0,   56,
   56,   56,   56,   56,   56,   56,   56,   56,   56,   56,
   56,   56,   56,   56,   56,   13,    0,   37,    0,    0,
    0,    0,    0,    0,    0,   56,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    7,   14,    8,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   51,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   34,   35,    0,
    0,    0,    0,   46,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   36,    0,    0,    0,   53,    0,    0,   55,
    0,    0,    0,   33,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   32,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   39,    0,    0,    0,   23,
   24,    0,   30,    0,   25,   26,   27,   28,    0,    0,
    0,    0,    0,    0,    0,   31,   41,    0,    0,    0,
    0,    0,    0,    0,   15,    0,    0,    0,    0,    0,
   29,   22,    0,    0,   20,    0,    0,    0,    0,    0,
    0,    0,   16,    0,   44,   17,   21,   18,   19,    0,
    0,   42,
};
const short yydgoto[] =
	{                                       4,
   21,   22,    5,    6,   56,   57,  109,  110,   58,  139,
  235,  236,  237,  256,    9,    7,
};
const short yysindex[] =
	{                                   -233,
  -31,    0,    0, -233,    0,    0,    0, -229, -241, -235,
    0,   -8,   -5,   -3,   -1,    0,    0, -220, -220,   -2,
    2,    0,    3, -220,  -82,  -78,    0, -180, -180,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -214,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  187,    0,  217,    7,
    8,    9,   11,   12,   20,    0,   41,   42,   46,   67,
   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,
   78,   80,    0,    0,    0, -242, -242, -242, -242, -242,
 -242,   81, -242, -138, -242, -242, -242, -242, -137, -136,
 -135, -134,   86,   87, -242, -242, -242, -242,   88,    0,
   89,   90,   91,   92,   93,   92, -242,   94,   95,   97,
   99,  100,  101,  102,  103,  104,  105,    0,    0,  109,
  108,  110,  111,    0, -242, -130, -129, -242,  115, -128,
  116,  118, -100,  -99,  -98,  -97,  -96,  -95,  -94,  -93,
  -92,  -90,    0,  -89,  -88,  -83,    0,  131,  138,    0,
   60,  140,   62,    0,  142,  146,  147,  145,  149,  148,
  150,  152,  153,  154,  155,  156,  158,  -64,  -63, -180,
  -62, -180,  -59,   82,   84,  -56,   85,  -54,   96,   98,
  106,  107,  -51,  -50,  -49,  171,  170,  250,  172,  280,
  173, -180, -180,  177, -180,  179, -180, -180, -180, -180,
  180,  181,  182,  112,  -34,  -40,  -32,    0,  -28,  323,
  353,  119,  386,  120,  416,  446,  489,  522,  -27,  -26,
  123, -180,  190,  196,  -19,    0,  -40,  207,  208,    0,
    0, -180,    0, -180,    0,    0,    0,    0,  209,  214,
 -180,  552,  133, -242,  137,    0,    0,  161,  162,  582,
  612,  163,  164,  651,    0, -180,   92, -180, -180, -180,
    0,    0, -180, -180,    0,  688,  247,  718,  748,  778,
  813,  854,    0,  166,    0,    0,    0,    0,    0, -180,
  884,    0,};
const short yyrindex[] =
	{                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  249,  249,  251,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   -6,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  252,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  252,    0,    0,    0,    0,    0,
  253,    0,    0,  254,    0,  254,  252,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  113,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  914,    0,  157,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  254,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,};
const short yygindex[] =
	{                                      0,
  272,  273,    0,    0,   83,  -29,  -91,  165,  -85, -109,
    0,   59,    0,    0,   21,  294,
};
#define YYTABLESIZE 1200
const short yytable[] =
	{                                      59,
  111,  112,  113,  114,  115,  116,  141,  118,    8,  120,
  121,  122,  123,  130,   30,   31,   32,   33,   14,  111,
  131,  132,  133,   10,   15,  142,    1,    2,    3,   12,
   13,  111,   16,   56,   45,   17,   18,   45,   19,   20,
   28,   24,   25,   26,   29,   66,   86,   87,   88,  111,
   89,   90,  160,   60,   61,   62,   63,   64,   65,   91,
   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,
   77,   78,   79,   80,   81,   82,   30,   31,   32,   33,
   93,   94,   34,   35,   36,   95,   92,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,   96,   97,   98,   99,
  100,  101,  102,  103,  104,  105,  106,  107,   45,  108,
  117,  119,  124,  125,  126,  127,  128,  129,  134,  158,
  159,  162,  135,  136,  137,  138,  140,  143,  144,   84,
  145,   84,  146,  147,  148,  149,  150,  151,  152,  153,
  198,  154,  200,  155,  156,  161,  163,  277,  164,  165,
  166,  167,  168,  169,  170,  171,  172,  173,  267,  174,
  175,  176,  220,  221,  178,  223,  177,  225,  226,  227,
  228,  179,  180,  181,  182,  183,  184,  185,  186,  187,
  189,  188,  190,  191,  192,  196,  197,  199,  193,  194,
  201,  195,  252,  204,  202,  206,  203,  205,  211,  212,
  213,  214,  260,  215,  261,  217,  219,  222,  207,  224,
  208,  264,  231,  229,  230,  233,  234,  238,  209,  210,
  253,  239,  249,  250,  232,  254,  276,   38,  278,  279,
  280,  242,  244,  281,  282,  251,  255,  258,  259,  262,
   45,   45,   45,   45,  263,  266,   45,   45,   45,  268,
  291,   45,   45,   45,   45,   45,   45,   45,   45,   45,
   45,   45,   45,   45,   45,   45,   45,   45,   45,   45,
   84,   40,   84,  269,  270,  273,  274,  284,  290,    9,
   23,   11,   50,   52,   54,  257,   27,   11,    0,  157,
    0,    0,   84,   84,    0,   84,    0,   84,   84,   84,
   84,   83,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   84,    0,    0,    0,    0,    0,
    0,   85,   84,   84,    0,    0,   84,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   84,    0,
   84,   84,   84,   84,   84,    0,    0,    0,    0,   38,
   38,   38,   38,   84,  216,   38,   38,   38,   38,    0,
   38,   38,   38,   38,   38,   38,   38,   38,   38,   38,
   38,   38,   38,   38,   38,   38,   38,   38,   38,    0,
    0,    0,    0,    0,  218,    0,    0,    0,    0,    0,
    0,    0,    0,   40,   40,   40,   40,    0,    0,   40,
   40,   40,   40,    0,   40,   40,   40,   40,   40,   40,
   40,   40,   40,   40,   40,   40,   40,   40,   40,   40,
   40,   40,   40,   30,   31,   32,   33,  240,    0,   34,
   35,   36,    0,    0,   37,   38,   39,   40,   41,   42,
   43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
   53,   54,   55,   30,   31,   32,   33,  241,    0,   34,
   35,   36,    0,    0,   37,   38,   39,   40,   41,   42,
   43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
   53,   54,   55,    0,    0,    0,   30,   31,   32,   33,
  243,    0,   34,   35,   36,    0,    0,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,   30,   31,   32,   33,
  245,    0,   34,   35,   36,    0,    0,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,    0,    0,    0,    0,
  246,    0,    0,    0,    0,    0,    0,    0,    0,   30,
   31,   32,   33,    0,    0,   34,   35,   36,    0,    0,
   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,
   47,   48,   49,   50,   51,   52,   53,   54,   55,   30,
   31,   32,   33,  247,    0,   34,   35,   36,    0,    0,
   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,
   47,   48,   49,   50,   51,   52,   53,   54,   55,    0,
    0,    0,   30,   31,   32,   33,  248,    0,   34,   35,
   36,    0,    0,   37,   38,   39,   40,   41,   42,   43,
   44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
   54,   55,   30,   31,   32,   33,  265,    0,   34,   35,
   36,    0,    0,   37,   38,   39,   40,   41,   42,   43,
   44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
   54,   55,   30,   31,   32,   33,  271,    0,   34,   35,
   36,    0,    0,   37,   38,   39,   40,   41,   42,   43,
   44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
   54,   55,    0,    0,    0,    0,  272,    0,    0,    0,
    0,    0,    0,    0,    0,   30,   31,   32,   33,    0,
    0,   34,   35,   36,    0,    0,   37,   38,   39,   40,
   41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
   51,   52,   53,   54,   55,  275,    0,    0,   30,   31,
   32,   33,    0,    0,   34,   35,   36,    0,    0,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   30,   31,
   32,   33,  283,    0,   34,   35,   36,    0,    0,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   30,   31,
   32,   33,  285,    0,   34,   35,   36,    0,    0,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   30,   31,
   32,   33,  286,    0,   34,   35,   36,    0,    0,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,    0,    0,
    0,    0,  287,    0,    0,    0,    0,   30,   31,   32,
   33,    0,    0,   34,   35,   36,    0,    0,   37,   38,
   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,   53,   54,   55,  288,    0,    0,
    0,    0,    0,    0,   30,   31,   32,   33,    0,    0,
   34,   35,   36,    0,    0,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   54,   55,   30,   31,   32,   33,  289,    0,
   34,   35,   36,    0,    0,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   54,   55,   30,   31,   32,   33,  292,    0,
   34,   35,   36,    0,    0,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   54,   55,   30,   31,   32,   33,   43,    0,
   34,   35,   36,    0,    0,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   54,   55,    0,    0,    0,    0,    0,   30,
   31,   32,   33,    0,    0,   34,   35,   36,    0,    0,
   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,
   47,   48,   49,   50,   51,   52,   53,   54,   55,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   30,   31,   32,   33,    0,    0,   34,   35,   36,    0,
    0,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   54,   55,
   30,   31,   32,   33,    0,    0,   34,   35,   36,    0,
    0,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   54,   55,
   43,   43,   43,   43,    0,    0,   43,   43,   43,    0,
    0,   43,   43,   43,   43,   43,   43,   43,   43,   43,
   43,   43,   43,   43,   43,   43,   43,   43,   43,   43,
};
const short yycheck[] =
	{                                      29,
   86,   87,   88,   89,   90,   91,  116,   93,   40,   95,
   96,   97,   98,  105,  257,  258,  259,  260,  260,  105,
  106,  107,  108,    3,  260,  117,  260,  261,  262,  259,
  260,  117,   41,   40,   41,   41,   40,   44,   40,  260,
  123,   44,   41,   41,  123,  260,   40,   40,   40,  135,
   40,   40,  138,   33,   34,   35,   36,   37,   38,   40,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,  257,  258,  259,  260,
   40,   40,  263,  264,  265,   40,   66,  268,  269,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,  283,  284,  285,  286,   40,   40,   40,   40,
   40,   40,   40,   40,   40,   40,   40,   40,  125,   40,
   40,  260,  260,  260,  260,  260,   41,   41,   41,  260,
  260,  260,   44,   44,   44,   44,   44,   44,   44,   57,
   44,   59,   44,   44,   44,   44,   44,   44,   44,   41,
  180,   44,  182,   44,   44,   41,   41,  267,   41,  260,
  260,  260,  260,  260,  260,  260,  260,  260,  254,  260,
  260,  260,  202,  203,   44,  205,  260,  207,  208,  209,
  210,   44,  123,   44,  123,   44,   41,   41,   44,   41,
   41,   44,   41,   41,   41,  260,  260,  260,   44,   44,
  260,   44,  232,  260,  123,  260,  123,  123,  260,  260,
  260,   41,  242,   44,  244,   44,   44,   41,  123,   41,
  123,  251,   41,   44,   44,  260,  267,  260,  123,  123,
   41,  260,  260,  260,  123,   40,  266,  125,  268,  269,
  270,  123,  123,  273,  274,  123,  266,   41,   41,   41,
  257,  258,  259,  260,   41,  123,  263,  264,  265,  123,
  290,  268,  269,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  198,  125,  200,  123,  123,  123,  123,   41,  123,   41,
   19,   41,   41,   41,   41,  237,   24,    4,   -1,  135,
   -1,   -1,  220,  221,   -1,  223,   -1,  225,  226,  227,
  228,  125,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  252,   -1,   -1,   -1,   -1,   -1,
   -1,  125,  260,  261,   -1,   -1,  264,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  276,   -1,
  278,  279,  280,  281,  282,   -1,   -1,   -1,   -1,  257,
  258,  259,  260,  291,  125,  263,  264,  265,  266,   -1,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  283,  284,  285,  286,   -1,
   -1,   -1,   -1,   -1,  125,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,  258,  259,  260,   -1,   -1,  263,
  264,  265,  266,   -1,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,  283,
  284,  285,  286,  257,  258,  259,  260,  125,   -1,  263,
  264,  265,   -1,   -1,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,  283,
  284,  285,  286,  257,  258,  259,  260,  125,   -1,  263,
  264,  265,   -1,   -1,  268,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,  283,
  284,  285,  286,   -1,   -1,   -1,  257,  258,  259,  260,
  125,   -1,  263,  264,  265,   -1,   -1,  268,  269,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,  283,  284,  285,  286,  257,  258,  259,  260,
  125,   -1,  263,  264,  265,   -1,   -1,  268,  269,  270,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,  283,  284,  285,  286,   -1,   -1,   -1,   -1,
  125,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,
  258,  259,  260,   -1,   -1,  263,  264,  265,   -1,   -1,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  283,  284,  285,  286,  257,
  258,  259,  260,  125,   -1,  263,  264,  265,   -1,   -1,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  283,  284,  285,  286,   -1,
   -1,   -1,  257,  258,  259,  260,  125,   -1,  263,  264,
  265,   -1,   -1,  268,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  257,  258,  259,  260,  125,   -1,  263,  264,
  265,   -1,   -1,  268,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  257,  258,  259,  260,  125,   -1,  263,  264,
  265,   -1,   -1,  268,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,   -1,   -1,   -1,   -1,  125,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  257,  258,  259,  260,   -1,
   -1,  263,  264,  265,   -1,   -1,  268,  269,  270,  271,
  272,  273,  274,  275,  276,  277,  278,  279,  280,  281,
  282,  283,  284,  285,  286,  125,   -1,   -1,  257,  258,
  259,  260,   -1,   -1,  263,  264,  265,   -1,   -1,  268,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,  282,  283,  284,  285,  286,  257,  258,
  259,  260,  125,   -1,  263,  264,  265,   -1,   -1,  268,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,  282,  283,  284,  285,  286,  257,  258,
  259,  260,  125,   -1,  263,  264,  265,   -1,   -1,  268,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,  282,  283,  284,  285,  286,  257,  258,
  259,  260,  125,   -1,  263,  264,  265,   -1,   -1,  268,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  281,  282,  283,  284,  285,  286,   -1,   -1,
   -1,   -1,  125,   -1,   -1,   -1,   -1,  257,  258,  259,
  260,   -1,   -1,  263,  264,  265,   -1,   -1,  268,  269,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,  283,  284,  285,  286,  125,   -1,   -1,
   -1,   -1,   -1,   -1,  257,  258,  259,  260,   -1,   -1,
  263,  264,  265,   -1,   -1,  268,  269,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
  283,  284,  285,  286,  257,  258,  259,  260,  125,   -1,
  263,  264,  265,   -1,   -1,  268,  269,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
  283,  284,  285,  286,  257,  258,  259,  260,  125,   -1,
  263,  264,  265,   -1,   -1,  268,  269,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
  283,  284,  285,  286,  257,  258,  259,  260,  125,   -1,
  263,  264,  265,   -1,   -1,  268,  269,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
  283,  284,  285,  286,   -1,   -1,   -1,   -1,   -1,  257,
  258,  259,  260,   -1,   -1,  263,  264,  265,   -1,   -1,
  268,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  283,  284,  285,  286,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  257,  258,  259,  260,   -1,   -1,  263,  264,  265,   -1,
   -1,  268,  269,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  257,  258,  259,  260,   -1,   -1,  263,  264,  265,   -1,
   -1,  268,  269,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  257,  258,  259,  260,   -1,   -1,  263,  264,  265,   -1,
   -1,  268,  269,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 286
#if YYDEBUG
const char * const yyname[] =
	{
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"ICONS",
"FCONS","SCONS","IDEN","PROC","FUNC_TOK","CHILDREN","SPOUSES","IF","ELSE",
"ELSIF","FAMILIES","WHILE","CALL","FORINDISET","FORINDI","FORNOTES","TRAVERSE",
"FORNODES","FORLIST_TOK","FORFAM","FORSOUR","FOREVEN","FOROTHR","BREAK",
"CONTINUE","RETURN","FATHERS","MOTHERS","PARENTS",
};
const char * const yyrule[] =
	{"$accept : defns",
"defns : defn",
"defns : defns defn",
"defn : proc",
"defn : func",
"defn : IDEN '(' IDEN ')'",
"defn : IDEN '(' SCONS ')'",
"proc : PROC m IDEN '(' idenso ')' '{' states '}'",
"func : FUNC_TOK m IDEN '(' idenso ')' '{' states '}'",
"idenso :",
"idenso : idens",
"idens : IDEN",
"idens : IDEN ',' idens",
"states : state",
"states : states state",
"state : CHILDREN m '(' expr ',' IDEN ',' IDEN ')' '{' states '}'",
"state : SPOUSES m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FAMILIES m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FATHERS m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}'",
"state : MOTHERS m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}'",
"state : PARENTS m '(' expr ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FORINDISET m '(' expr ',' IDEN ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FORLIST_TOK m '(' expr ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FORINDI m '(' IDEN ',' IDEN ')' '{' states '}'",
"state : FORNOTES m '(' expr ',' IDEN ')' '{' states '}'",
"state : FORFAM m '(' IDEN ',' IDEN ')' '{' states '}'",
"state : FORSOUR m '(' IDEN ',' IDEN ')' '{' states '}'",
"state : FOREVEN m '(' IDEN ',' IDEN ')' '{' states '}'",
"state : FOROTHR m '(' IDEN ',' IDEN ')' '{' states '}'",
"state : TRAVERSE m '(' expr ',' IDEN ',' IDEN ')' '{' states '}'",
"state : FORNODES m '(' expr ',' IDEN ')' '{' states '}'",
"state : IF m '(' expr secondo ')' '{' states '}' elsifso elseo",
"state : WHILE m '(' expr secondo ')' '{' states '}'",
"state : CALL IDEN m '(' exprso ')'",
"state : BREAK m '(' ')'",
"state : CONTINUE m '(' ')'",
"state : RETURN m '(' exprso ')'",
"state : expr",
"elsifso :",
"elsifso : elsifs",
"elsifs : elsif",
"elsifs : elsif elsifs",
"elsif : ELSIF '(' expr secondo ')' '{' states '}'",
"elseo :",
"elseo : ELSE '{' states '}'",
"expr : IDEN",
"expr : IDEN m '(' exprso ')'",
"expr : SCONS",
"expr : ICONS",
"expr : FCONS",
"exprso :",
"exprso : exprs",
"exprs : expr",
"exprs : expr ',' exprs",
"secondo :",
"secondo : ',' expr",
"m :",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
/* LINTUSED */
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
unsigned int yystacksize;
int yyparse(void);
#line 298 "grammar.y"

// join Joins a list of PNodes to another PNode (which may be the start of another list).
// TODO: Isn't this generic enough to be moved to the pnode.[hc] files?
void join(PNode* list, PNode* last) {
	PNode* prev = null;
	while (list) {
		prev = list;
		list = list->next;
	}
	ASSERT(prev);
	prev->next = last;
}

void yyerror(String str) {
	extern String curFileName;
	printf("Syntax Error (%s): %s: line %d\n", str, curFileName, curLine);
	Perrors++;
}
#line 556 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    unsigned int newsize;
    long sslen;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    sslen = yyssp - yyss;
#ifdef SIZE_MAX
#define YY_SIZE_MAX SIZE_MAX
#else
#define YY_SIZE_MAX 0xffffffffU
#endif
    if (newsize && YY_SIZE_MAX / newsize < sizeof *newss)
        goto bail;
    newss = (short *)realloc(yyss, newsize * sizeof *newss);
    if (newss == NULL)
        goto bail;
    yyss = newss;
    yyssp = newss + sslen;
    if (newsize && YY_SIZE_MAX / newsize < sizeof *newvs)
        goto bail;
    newvs = (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs);
    if (newvs == NULL)
        goto bail;
    yyvs = newvs;
    yyvsp = newvs + sslen;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
bail:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return -1;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif /* YYDEBUG */

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yyvsp[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 5:
#line 58 "grammar.y"
{  /* Interested in "global".*/
        if (eqstr("global", yyvsp[-3].identifier))
            assignValueToSymbol(globalTable, yyvsp[-1].identifier, (PValue) {PVNull});
    }
break;
case 6:
#line 62 "grammar.y"
{  /* Interested in "include".*/
        if (eqstr("include", yyvsp[-3].identifier))
			prependToList(pendingFiles, yyvsp[-1].string);
    }
break;
case 7:
#line 69 "grammar.y"
{
		yyval.pnode = procDefPNode(yyvsp[-6].identifier, yyvsp[-4].pnode, yyvsp[-1].pnode);
		yyval.pnode->lineNumber = (int) yyvsp[-7].integer;
        addToFunctionTable(procedureTable, yyvsp[-6].identifier, yyval.pnode);
    }
break;
case 8:
#line 77 "grammar.y"
{
		yyval.pnode = funcDefPNode(yyvsp[-6].identifier, yyvsp[-4].pnode, yyvsp[-1].pnode);
		yyval.pnode->lineNumber = (int) yyvsp[-7].integer;
        addToFunctionTable(functionTable, yyvsp[-6].identifier, yyval.pnode);
    }
break;
case 9:
#line 84 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 10:
#line 87 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 11:
#line 94 "grammar.y"
{
        yyval.pnode = iden_node(yyvsp[0].identifier);
    }
break;
case 12:
#line 97 "grammar.y"
{
        yyval.pnode = iden_node(yyvsp[-2].identifier);
        yyval.pnode->next = yyvsp[0].pnode;
    }
break;
case 13:
#line 104 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 14:
#line 107 "grammar.y"
{
        join(yyvsp[-1].pnode, yyvsp[0].pnode);
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 15:
#line 113 "grammar.y"
{
        yyval.pnode = childrenPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 16:
#line 117 "grammar.y"
{
        yyval.pnode = spousesPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 17:
#line 121 "grammar.y"
{
        yyval.pnode = familiesPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 18:
#line 125 "grammar.y"
{
        yyval.pnode = fathersPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 19:
#line 129 "grammar.y"
{
        yyval.pnode = mothersPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 20:
#line 133 "grammar.y"
{
        yyval.pnode = parentsPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 21:
#line 137 "grammar.y"
{
        yyval.pnode = forindisetPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 22:
#line 141 "grammar.y"
{
        yyval.pnode = forlistPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 23:
#line 145 "grammar.y"
{
        yyval.pnode = forindiPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 24:
#line 149 "grammar.y"
{
        yyval.pnode = fornotesPNode(yyvsp[-6].pnode, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 25:
#line 153 "grammar.y"
{
        yyval.pnode = forfamPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 26:
#line 157 "grammar.y"
{
        yyval.pnode = forsourPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 27:
#line 161 "grammar.y"
{
        yyval.pnode = forevenPNode(yyvsp[-6].identifier, yyvsp[-4].identifier,yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 28:
#line 165 "grammar.y"
{
        yyval.pnode = forothrPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 29:
#line 169 "grammar.y"
{
        yyval.pnode = traversePNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 30:
#line 173 "grammar.y"
{
        yyval.pnode = fornodesPNode(yyvsp[-6].pnode, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 31:
#line 177 "grammar.y"
{
        yyvsp[-7].pnode->next = yyvsp[-6].pnode;  /* In case there is an identifier first.*/
        prev = null;  this = yyvsp[-1].pnode;
        while (this) {
            prev = this;
            this = this->elseState;
        }
        if (prev) {
            prev->elseState = yyvsp[0].pnode;
            yyval.pnode = ifPNode(yyvsp[-7].pnode, yyvsp[-3].pnode, yyvsp[-1].pnode);
        } else
        yyval.pnode = ifPNode(yyvsp[-7].pnode, yyvsp[-3].pnode, yyvsp[0].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-9].integer;
    }
break;
case 32:
#line 191 "grammar.y"
{
        yyvsp[-5].pnode->next = yyvsp[-4].pnode;
        yyval.pnode = whilePNode(yyvsp[-5].pnode, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-7].integer;
    }
break;
case 33:
#line 196 "grammar.y"
{
        yyval.pnode = procCallPNode(yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 34:
#line 200 "grammar.y"
{
        yyval.pnode = breakPNode();
        yyval.pnode->lineNumber = (int)yyvsp[-2].integer;
    }
break;
case 35:
#line 204 "grammar.y"
{
        yyval.pnode = continuePNode();
        yyval.pnode->lineNumber = (int)yyvsp[-2].integer;
    }
break;
case 36:
#line 208 "grammar.y"
{
        yyval.pnode = returnPNode(yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 37:
#line 212 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 38:
#line 216 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 39:
#line 219 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 40:
#line 223 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 41:
#line 226 "grammar.y"
{
        yyvsp[-1].pnode->elseState = yyvsp[0].pnode;
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 42:
#line 231 "grammar.y"
{
        yyvsp[-5].pnode->next = yyvsp[-4].pnode;
        yyval.pnode = ifPNode(yyvsp[-5].pnode, yyvsp[-1].pnode, null);
    }
break;
case 43:
#line 238 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 44:
#line 241 "grammar.y"
{
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 45:
#line 247 "grammar.y"
{
        yyval.pnode = iden_node((String)yyvsp[0].identifier);
        yyval.pnode->arguments = null;
    }
break;
case 46:
#line 251 "grammar.y"
{
        yyval.pnode = funcCallPNode(yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 47:
#line 255 "grammar.y"
{
        yyval.pnode = sconsPNode(yyvsp[0].string);
    }
break;
case 48:
#line 258 "grammar.y"
{
        yyval.pnode = iconsPNode(yyvsp[0].integer);
    }
break;
case 49:
#line 261 "grammar.y"
{
        yyval.pnode = fconsPNode(yyvsp[0].floating);
    }
break;
case 50:
#line 267 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 51:
#line 270 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 52:
#line 276 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 53:
#line 279 "grammar.y"
{
        yyvsp[-2].pnode->next = yyvsp[0].pnode;
        yyval.pnode = yyvsp[-2].pnode;
    }
break;
case 54:
#line 284 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 55:
#line 287 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 56:
#line 293 "grammar.y"
{
        yyval.integer = curLine;
    }
break;
#line 1106 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (1);
yyaccept:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (0);
}
