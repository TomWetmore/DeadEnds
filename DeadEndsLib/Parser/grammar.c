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

/* Global variables that form the channel between the lexer, parser and interpreter.*/
extern SymbolTable *globalTable; /* Global variables.*/
extern FunctionTable *procedureTable;/* User procedures.*/
extern FunctionTable *functionTable; /* User functions.*/
extern List *pendingFiles; /* Pending list of included files.*/
extern int curLine; /* Line number in current file.*/

static PNode *this, *prev;

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
    0,    0,   14,   14,   14,   14,   15,   16,    1,    1,
    2,    2,    4,    4,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    9,    9,   10,
   10,   11,   12,   12,    7,    7,    7,    7,    7,    5,
    5,    6,    6,    8,    8,   13,
};
const short yylen[] =
	{                                         2,
    1,    2,    1,    1,    4,    4,    8,    8,    0,    1,
    1,    3,    1,    2,   12,   14,   14,   14,   14,   12,
   14,   12,   10,   10,   10,   10,   10,   10,   12,   10,
   11,    9,    6,    4,    4,    5,    1,    0,    1,    1,
    2,    8,    0,    4,    1,    5,    1,    1,    1,    0,
    1,    1,    3,    0,    2,    0,
};
const short yydefred[] =
	{                                      0,
    0,    0,    0,    0,    1,    3,    4,    0,    0,    0,
    2,    0,    0,    0,    0,    6,    5,    0,    0,   10,
    0,    0,    0,    0,   12,    0,    0,   48,   49,   47,
    0,   56,   56,   56,   56,   56,    0,   56,   56,   56,
   56,   56,   56,   56,   56,   56,   56,   56,   56,   56,
   56,   56,   56,   13,    0,   37,    0,    0,    0,    0,
    0,    0,    0,   56,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    7,   14,    8,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   51,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   34,   35,    0,    0,    0,
    0,   46,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   36,    0,    0,    0,   53,    0,    0,   55,    0,    0,
    0,   33,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   32,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   39,    0,    0,    0,   23,   24,    0,
   30,    0,   25,   26,   27,   28,    0,    0,    0,    0,
    0,    0,    0,   31,   41,    0,    0,    0,    0,    0,
    0,    0,   15,    0,    0,    0,    0,    0,   29,   22,
    0,    0,   20,    0,    0,    0,    0,    0,    0,    0,
   16,    0,   44,   17,   21,   18,   19,    0,    0,   42,
};
const short yydgoto[] =
	{                                       4,
   19,   20,   54,   55,  107,  108,   56,  137,  233,  234,
  235,  254,   58,    5,    6,    7,
};
const short yysindex[] =
	{                                   -233,
  -31, -241, -230, -233,    0,    0,    0, -235,   -9,   -7,
    0,   -5,   -4, -221, -221,    0,    0,   -3,   -1,    0,
    1, -221,  -80,  -79,    0, -181, -181,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -215,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  187,    0,  217,    6,    7,    8,
    9,   11,   12,    0,   19,   40,   41,   45,   66,   67,
   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,
    0,    0,    0, -242, -242, -242, -242, -242, -242,   78,
 -242, -140, -242, -242, -242, -242, -139, -138, -137, -136,
   84,   86, -242, -242, -242, -242,   87,    0,   82,   88,
   89,   90,   91,   90, -242,   92,   93,   94,   95,   97,
   99,  100,  101,  102,  103,    0,    0,  107,  105,  106,
  108,    0, -242, -131, -130, -242,  114, -129,  115,  116,
 -106, -101, -100,  -99,  -98,  -97,  -96,  -95,  -94,  -93,
    0,  -92,  -90,  -88,    0,  127,  131,    0,   54,  138,
   60,    0,  140,  144,  145,  143,  147,  146,  148,  150,
  151,  152,  153,  154,  155,  -66,  -65, -181,  -64, -181,
  -60,   79,   81,  -59,   83,  -55,   96,   98,  104,  109,
  -53,  -52,  -51,  169,  167,  250,  168,  280,  170, -181,
 -181,  175, -181,  176, -181, -181, -181, -181,  174,  179,
  183,  110,  -40,  -42,  -34,    0,  -32,  323,  353,  111,
  386,  112,  416,  446,  489,  522,  -30,  -29,  119, -181,
  188,  196,  -23,    0,  -42,  205,  206,    0,    0, -181,
    0, -181,    0,    0,    0,    0,  207,  208, -181,  552,
  132, -242,  133,    0,    0,  137,  161,  582,  612,  162,
  163,  651,    0, -181,   90, -181, -181, -181,    0,    0,
 -181, -181,    0,  688,  209,  718,  748,  778,  813,  854,
    0,  164,    0,    0,    0,    0,    0, -181,  884,    0,};
const short yyrindex[] =
	{                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  247,  247,    0,    0,  248,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   -6,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  249,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  249,    0,    0,    0,    0,    0,  251,    0,
    0,  252,    0,  252,  249,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  113,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  914,    0,  157,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  252,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,};
const short yygindex[] =
	{                                      0,
  276,  272,   85,  -27,  -89,  165,  -83, -107,    0,   61,
    0,    0,   22,  291,    0,    0,
};
#define YYTABLESIZE 1200
const short yytable[] =
	{                                      57,
  109,  110,  111,  112,  113,  114,  139,  116,    8,  118,
  119,  120,  121,  128,   28,   29,   30,   31,    9,  109,
  129,  130,  131,   12,   13,  140,    1,    2,    3,   10,
   14,  109,   15,   56,   45,   16,   17,   45,   18,   23,
   22,   24,   26,   27,   64,   84,   85,   86,   87,  109,
   88,   89,  158,   59,   60,   61,   62,   63,   91,   65,
   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
   76,   77,   78,   79,   80,   28,   29,   30,   31,   92,
   93,   32,   33,   34,   94,   90,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,   53,   95,   96,   97,   98,   99,
  100,  101,  102,  103,  104,  105,  106,  115,   45,  117,
  122,  123,  124,  125,  126,  133,  127,  132,  156,  157,
  160,  134,  135,  136,  138,  141,  142,  143,  144,   82,
  145,   82,  146,  147,  148,  149,  150,  151,  152,  153,
  196,  154,  198,  163,  159,  161,  162,  275,  164,  165,
  166,  167,  168,  169,  170,  171,  172,  173,  265,  174,
  176,  175,  218,  219,  177,  221,  178,  223,  224,  225,
  226,  179,  180,  181,  182,  183,  184,  185,  187,  186,
  188,  189,  190,  194,  195,  197,  191,  192,  193,  199,
  202,  200,  250,  201,  204,  203,  209,  210,  211,  212,
  213,  215,  258,  217,  259,  220,  222,  227,  205,  231,
  206,  262,  228,  229,  232,  236,  207,  237,  251,  247,
  248,  208,  230,  240,  242,  252,  274,   38,  276,  277,
  278,  249,  253,  279,  280,  256,  257,  260,  261,  282,
   45,   45,   45,   45,  264,  266,   45,   45,   45,  267,
  289,   45,   45,   45,   45,   45,   45,   45,   45,   45,
   45,   45,   45,   45,   45,   45,   45,   45,   45,   45,
   82,   40,   82,  268,  271,  272,  288,    9,   11,   50,
   21,   52,   54,   25,   11,  255,    0,  155,    0,    0,
    0,    0,   82,   82,    0,   82,    0,   82,   82,   82,
   82,   81,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   82,    0,    0,    0,    0,    0,
    0,   83,   82,   82,    0,    0,   82,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   82,    0,
   82,   82,   82,   82,   82,    0,    0,    0,    0,   38,
   38,   38,   38,   82,  214,   38,   38,   38,   38,    0,
   38,   38,   38,   38,   38,   38,   38,   38,   38,   38,
   38,   38,   38,   38,   38,   38,   38,   38,   38,    0,
    0,    0,    0,    0,  216,    0,    0,    0,    0,    0,
    0,    0,    0,   40,   40,   40,   40,    0,    0,   40,
   40,   40,   40,    0,   40,   40,   40,   40,   40,   40,
   40,   40,   40,   40,   40,   40,   40,   40,   40,   40,
   40,   40,   40,   28,   29,   30,   31,  238,    0,   32,
   33,   34,    0,    0,   35,   36,   37,   38,   39,   40,
   41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
   51,   52,   53,   28,   29,   30,   31,  239,    0,   32,
   33,   34,    0,    0,   35,   36,   37,   38,   39,   40,
   41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
   51,   52,   53,    0,    0,    0,   28,   29,   30,   31,
  241,    0,   32,   33,   34,    0,    0,   35,   36,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   28,   29,   30,   31,
  243,    0,   32,   33,   34,    0,    0,   35,   36,   37,
   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,    0,    0,    0,    0,
  244,    0,    0,    0,    0,    0,    0,    0,    0,   28,
   29,   30,   31,    0,    0,   32,   33,   34,    0,    0,
   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,   28,
   29,   30,   31,  245,    0,   32,   33,   34,    0,    0,
   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,    0,
    0,    0,   28,   29,   30,   31,  246,    0,   32,   33,
   34,    0,    0,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   28,   29,   30,   31,  263,    0,   32,   33,
   34,    0,    0,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,   28,   29,   30,   31,  269,    0,   32,   33,
   34,    0,    0,   35,   36,   37,   38,   39,   40,   41,
   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
   52,   53,    0,    0,    0,    0,  270,    0,    0,    0,
    0,    0,    0,    0,    0,   28,   29,   30,   31,    0,
    0,   32,   33,   34,    0,    0,   35,   36,   37,   38,
   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,   53,  273,    0,    0,   28,   29,
   30,   31,    0,    0,   32,   33,   34,    0,    0,   35,
   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   28,   29,
   30,   31,  281,    0,   32,   33,   34,    0,    0,   35,
   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   28,   29,
   30,   31,  283,    0,   32,   33,   34,    0,    0,   35,
   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   28,   29,
   30,   31,  284,    0,   32,   33,   34,    0,    0,   35,
   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,    0,    0,
    0,    0,  285,    0,    0,    0,    0,   28,   29,   30,
   31,    0,    0,   32,   33,   34,    0,    0,   35,   36,
   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,
   47,   48,   49,   50,   51,   52,   53,  286,    0,    0,
    0,    0,    0,    0,   28,   29,   30,   31,    0,    0,
   32,   33,   34,    0,    0,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   28,   29,   30,   31,  287,    0,
   32,   33,   34,    0,    0,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   28,   29,   30,   31,  290,    0,
   32,   33,   34,    0,    0,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   28,   29,   30,   31,   43,    0,
   32,   33,   34,    0,    0,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,    0,    0,    0,    0,    0,   28,
   29,   30,   31,    0,    0,   32,   33,   34,    0,    0,
   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   28,   29,   30,   31,    0,    0,   32,   33,   34,    0,
    0,   35,   36,   37,   38,   39,   40,   41,   42,   43,
   44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
   28,   29,   30,   31,    0,    0,   32,   33,   34,    0,
    0,   35,   36,   37,   38,   39,   40,   41,   42,   43,
   44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
   43,   43,   43,   43,    0,    0,   43,   43,   43,    0,
    0,   43,   43,   43,   43,   43,   43,   43,   43,   43,
   43,   43,   43,   43,   43,   43,   43,   43,   43,   43,
};
const short yycheck[] =
	{                                      27,
   84,   85,   86,   87,   88,   89,  114,   91,   40,   93,
   94,   95,   96,  103,  257,  258,  259,  260,  260,  103,
  104,  105,  106,  259,  260,  115,  260,  261,  262,  260,
   40,  115,   40,   40,   41,   41,   41,   44,  260,   41,
   44,   41,  123,  123,  260,   40,   40,   40,   40,  133,
   40,   40,  136,   32,   33,   34,   35,   36,   40,   38,
   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
   49,   50,   51,   52,   53,  257,  258,  259,  260,   40,
   40,  263,  264,  265,   40,   64,  268,  269,  270,  271,
  272,  273,  274,  275,  276,  277,  278,  279,  280,  281,
  282,  283,  284,  285,  286,   40,   40,   40,   40,   40,
   40,   40,   40,   40,   40,   40,   40,   40,  125,  260,
  260,  260,  260,  260,   41,   44,   41,   41,  260,  260,
  260,   44,   44,   44,   44,   44,   44,   44,   44,   55,
   44,   57,   44,   44,   44,   44,   44,   41,   44,   44,
  178,   44,  180,  260,   41,   41,   41,  265,  260,  260,
  260,  260,  260,  260,  260,  260,  260,  260,  252,  260,
   44,  260,  200,  201,   44,  203,  123,  205,  206,  207,
  208,   44,  123,   44,   41,   41,   44,   41,   41,   44,
   41,   41,   41,  260,  260,  260,   44,   44,   44,  260,
  260,  123,  230,  123,  260,  123,  260,  260,  260,   41,
   44,   44,  240,   44,  242,   41,   41,   44,  123,  260,
  123,  249,   44,   41,  267,  260,  123,  260,   41,  260,
  260,  123,  123,  123,  123,   40,  264,  125,  266,  267,
  268,  123,  266,  271,  272,   41,   41,   41,   41,   41,
  257,  258,  259,  260,  123,  123,  263,  264,  265,  123,
  288,  268,  269,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  196,  125,  198,  123,  123,  123,  123,   41,   41,   41,
   15,   41,   41,   22,    4,  235,   -1,  133,   -1,   -1,
   -1,   -1,  218,  219,   -1,  221,   -1,  223,  224,  225,
  226,  125,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  250,   -1,   -1,   -1,   -1,   -1,
   -1,  125,  258,  259,   -1,   -1,  262,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  274,   -1,
  276,  277,  278,  279,  280,   -1,   -1,   -1,   -1,  257,
  258,  259,  260,  289,  125,  263,  264,  265,  266,   -1,
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
"proc : PROC IDEN '(' idenso ')' '{' states '}'",
"func : FUNC_TOK IDEN '(' idenso ')' '{' states '}'",
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
#line 287 "grammar.y"

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
#line 553 "y.tab.c"
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
#line 57 "grammar.y"
{  /* Interested in "global".*/
        if (eqstr("global", yyvsp[-3].identifier))
            assignValueToSymbol(globalTable, yyvsp[-1].identifier, (PValue) {PVAny});
    }
break;
case 6:
#line 61 "grammar.y"
{  /* Interested in "include".*/
        if (eqstr("include", yyvsp[-3].identifier))
			prependToList(pendingFiles, yyvsp[-1].string);
    }
break;
case 7:
#line 68 "grammar.y"
{
        addToFunctionTable(procedureTable, yyvsp[-6].identifier, (Word)procDefPNode(yyvsp[-6].identifier, yyvsp[-4].pnode, yyvsp[-1].pnode));
    }
break;
case 8:
#line 74 "grammar.y"
{
        addToFunctionTable(functionTable, yyvsp[-6].identifier, (Word)funcDefPNode(yyvsp[-6].identifier, yyvsp[-4].pnode, yyvsp[-1].pnode));
    }
break;
case 9:
#line 79 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 10:
#line 82 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 11:
#line 89 "grammar.y"
{
        yyval.pnode = iden_node(yyvsp[0].identifier);
    }
break;
case 12:
#line 92 "grammar.y"
{
        yyval.pnode = iden_node(yyvsp[-2].identifier);
        yyval.pnode->next = yyvsp[0].pnode;
    }
break;
case 13:
#line 100 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 14:
#line 103 "grammar.y"
{
        join(yyvsp[-1].pnode, yyvsp[0].pnode);
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 15:
#line 110 "grammar.y"
{
        yyval.pnode = childrenPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 16:
#line 114 "grammar.y"
{
        yyval.pnode = spousesPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 17:
#line 118 "grammar.y"
{
        yyval.pnode = familiesPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 18:
#line 122 "grammar.y"
{
        yyval.pnode = fathersPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 19:
#line 126 "grammar.y"
{
        yyval.pnode = mothersPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 20:
#line 130 "grammar.y"
{
        yyval.pnode = parentsPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 21:
#line 134 "grammar.y"
{
        yyval.pnode = forindisetPNode(yyvsp[-10].pnode, yyvsp[-8].identifier, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-12].integer;
    }
break;
case 22:
#line 138 "grammar.y"
{
        yyval.pnode = forlistPNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 23:
#line 142 "grammar.y"
{
        yyval.pnode = forindiPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 24:
#line 146 "grammar.y"
{
        yyval.pnode = fornotesPNode(yyvsp[-6].pnode, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 25:
#line 150 "grammar.y"
{
        yyval.pnode = forfamPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 26:
#line 154 "grammar.y"
{
        yyval.pnode = forsourPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 27:
#line 158 "grammar.y"
{
        yyval.pnode = forevenPNode(yyvsp[-6].identifier, yyvsp[-4].identifier,yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 28:
#line 162 "grammar.y"
{
        yyval.pnode = forothrPNode(yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 29:
#line 166 "grammar.y"
{
        yyval.pnode = traversePNode(yyvsp[-8].pnode, yyvsp[-6].identifier, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-10].integer;
    }
break;
case 30:
#line 170 "grammar.y"
{
        yyval.pnode = fornodesPNode(yyvsp[-6].pnode, yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-8].integer;
    }
break;
case 31:
#line 174 "grammar.y"
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
#line 188 "grammar.y"
{
        yyvsp[-5].pnode->next = yyvsp[-4].pnode;
        yyval.pnode = whilePNode(yyvsp[-5].pnode, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-7].integer;
    }
break;
case 33:
#line 193 "grammar.y"
{
        yyval.pnode = procCallPNode(yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 34:
#line 197 "grammar.y"
{
        yyval.pnode = breakPNode();
        yyval.pnode->lineNumber = (int)yyvsp[-2].integer;
    }
break;
case 35:
#line 201 "grammar.y"
{
        yyval.pnode = continuePNode();
        yyval.pnode->lineNumber = (int)yyvsp[-2].integer;
    }
break;
case 36:
#line 205 "grammar.y"
{
        yyval.pnode = returnPNode(yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 37:
#line 209 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 38:
#line 213 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 39:
#line 216 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 40:
#line 220 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 41:
#line 223 "grammar.y"
{
        yyvsp[-1].pnode->elseState = yyvsp[0].pnode;
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 42:
#line 228 "grammar.y"
{
        yyvsp[-5].pnode->next = yyvsp[-4].pnode;
        yyval.pnode = ifPNode(yyvsp[-5].pnode, yyvsp[-1].pnode, null);
    }
break;
case 43:
#line 235 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 44:
#line 238 "grammar.y"
{
        yyval.pnode = yyvsp[-1].pnode;
    }
break;
case 45:
#line 242 "grammar.y"
{
        yyval.pnode = iden_node((String)yyvsp[0].identifier);
        yyval.pnode->arguments = null;
    }
break;
case 46:
#line 246 "grammar.y"
{
        yyval.pnode = funcCallPNode(yyvsp[-4].identifier, yyvsp[-1].pnode);
        yyval.pnode->lineNumber = (int)yyvsp[-3].integer;
    }
break;
case 47:
#line 250 "grammar.y"
{
        yyval.pnode = sconsPNode(yyvsp[0].string);
    }
break;
case 48:
#line 253 "grammar.y"
{
        yyval.pnode = iconsPNode(yyvsp[0].integer);
    }
break;
case 49:
#line 256 "grammar.y"
{
        yyval.pnode = fconsPNode(yyvsp[0].floating);
    }
break;
case 50:
#line 260 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 51:
#line 263 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 52:
#line 267 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 53:
#line 270 "grammar.y"
{
        yyvsp[-2].pnode->next = yyvsp[0].pnode;
        yyval.pnode = yyvsp[-2].pnode;
    }
break;
case 54:
#line 275 "grammar.y"
{
        yyval.pnode = 0;
    }
break;
case 55:
#line 278 "grammar.y"
{
        yyval.pnode = yyvsp[0].pnode;
    }
break;
case 56:
#line 282 "grammar.y"
{
        yyval.integer = curLine;
    }
break;
#line 1099 "y.tab.c"
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
