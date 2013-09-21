/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* "%code top" blocks.  */


#include <stddef.h>
#include <string.h>
#include "parser.h"
#include "AstUtils.h"
#include "resources.h"
#include "ast.h"
#include "pt.h"
#pragma warning(disable : 4003)
#pragma warning(disable : 4005)

void yyerror(const char *message);
#define YYDEBUG 1



AstNode *astTree;
PtNode *ptTree;
ParserContext Context;





/* Copy the first part of user declarations.  */



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */


#include "parser.h"
#include "common.h"
struct Node;




/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_IDENTIFIER = 258,
     TOK_B_AND = 259,
     TOK_B_OR = 260,
     TOK_B_XOR = 261,
     TOK_B_NOT = 262,
     TOK_AR_PLUS = 263,
     TOK_AR_MINUS = 264,
     TOK_AR_MUL = 265,
     TOK_AR_DIV = 266,
     TOK_AR_MOD = 267,
     TOK_ASSIGN_OP = 268,
     TOK_COMP_OP = 269,
     TOK_IF = 270,
     TOK_ELSE = 271,
     TOK_ENDEXPR = 272,
     TOK_OPENPAR = 273,
     TOK_CLOSEPAR = 274,
     TOK_OPENBR = 275,
     TOK_CLOSEBR = 276,
     TOK_OPENSQ = 277,
     TOK_CLOSESQ = 278,
     TOK_DOT = 279,
     TOK_COMMA = 280,
     TOK_DOUBLEDOT = 281,
     TOK_ROM_DECL = 282,
     TOK_INT_DECL = 283,
     TOK_FLOAT_DECL = 284,
     TOK_FOR_DECL = 285,
     TOK_WHILE_DECL = 286,
     TOK_DO_DECL = 287,
     TOK_ROM_CONST = 288,
     TOK_INT_CONST = 289,
     TOK_FLOAT_CONST = 290,
     TOK_PRINT = 291,
     TOK_READ = 292,
     TOK_BREAK = 293,
     TOK_CONTINUE = 294,
     TOK_GOTO = 295,
     TOK_STRUCT = 296,
     TOK_UNION = 297,
     TOK_SWITCH = 298,
     TOK_CASE = 299,
     TOK_DEFAULT = 300,
     TOK_STRING_LITERAL = 301,
     EXPR_ERROR = 302,
     STMNT_BLOCK_ERROR = 303,
     IF_WITHOUT_ELSE = 304,
     NOTX = 305,
     UMINUS = 306
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	Node *_node;
	AstNode *_AstNode;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


Node* CreateExpressionNode(Node *op, bool isBooleanOp, Node *left, Node *right, const YYLTYPE left_loc, const YYLTYPE right_loc)
{
	BaseTypeInfo *type = (isBooleanOp ? new BoolType() : left->astNode->GetResultType()->Clone());
	AstNode *rightOpAst = (right != nullptr ? right->astNode : nullptr);
	PtNode *ptNode;

 	AssertOneOfTypes(left, left_loc, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
	if (right != nullptr)
	{
		AssertOneOfTypes(right, right_loc, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
		ptNode = createPtNodeWithChildren("expr", 3, left->ptNode, op->ptNode, right->ptNode);
	}
	else
	{
		ptNode = createPtNodeWithChildren("expr", 2, op->ptNode, left->ptNode);
	}

	return createNode(new OperatorAstNode(op->ptNode->text, left->astNode, rightOpAst, new VarAstNode(true, Context.GenerateNewTmpVar(type))), 
				ptNode);
}



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  71
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   569

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  46
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNRULES -- Number of states.  */
#define YYNSTATES  187

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      52,    53,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    14,    18,    21,
      23,    26,    27,    30,    31,    36,    40,    41,    46,    47,
      53,    55,    57,    59,    62,    65,    67,    70,    74,    76,
      78,    80,    82,    84,    87,    90,    93,    96,    99,   101,
     105,   108,   110,   112,   115,   117,   123,   126,   131,   137,
     143,   149,   157,   162,   166,   170,   176,   180,   185,   191,
     193,   195,   197,   199,   201,   204,   206,   208,   210,   213,
     215,   217,   221,   223,   225,   229,   233,   237,   240,   243,
     247,   251,   255,   259,   263,   267,   271,   275,   276,   281,
     284,   288,   292,   295,   297,   299,   301,   303,   308,   312,
     318,   320,   323,   326,   330,   332,   334,   337
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    -1,    56,    57,    -1,    56,    -1,    57,
      -1,    67,    17,    -1,    56,    67,    17,    -1,    67,     1,
      -1,    66,    -1,    57,    66,    -1,    -1,    20,    59,    -1,
      -1,    58,    57,    21,    61,    -1,    58,    57,     1,    -1,
      -1,    58,    56,    21,    62,    -1,    -1,    58,    56,    57,
      21,    63,    -1,    87,    -1,    86,    -1,    17,    -1,    64,
      17,    -1,    87,     1,    -1,    65,    -1,     3,    26,    -1,
      40,     3,    17,    -1,    75,    -1,    76,    -1,    92,    -1,
      73,    -1,    74,    -1,    38,    17,    -1,    38,     1,    -1,
      39,    17,    -1,    39,     1,    -1,    83,     3,    -1,    68,
      -1,    69,    71,    72,    -1,    70,     3,    -1,    41,    -1,
      42,    -1,    20,    56,    -1,    21,    -1,    36,    18,    87,
      19,    17,    -1,    36,     1,    -1,    36,    52,    87,     1,
      -1,    36,    52,    87,    53,     1,    -1,    37,    18,    85,
      19,    17,    -1,    15,    18,    87,    19,    82,    -1,    15,
      18,    87,    19,    82,    16,    82,    -1,    15,    18,    87,
       1,    -1,    79,    78,    82,    -1,    80,    77,    82,    -1,
      81,    82,    31,    77,    17,    -1,    18,    87,    19,    -1,
      18,    65,    65,    19,    -1,    18,    65,    65,    64,    19,
      -1,    30,    -1,    31,    -1,    32,    -1,    60,    -1,    66,
      -1,    84,    88,    -1,    27,    -1,    29,    -1,    28,    -1,
      70,     3,    -1,    90,    -1,    89,    -1,    85,    13,    87,
      -1,    85,    -1,    91,    -1,    87,     4,    87,    -1,    87,
       5,    87,    -1,    87,     6,    87,    -1,     7,    87,    -1,
       9,    87,    -1,    87,     8,    87,    -1,    87,     9,    87,
      -1,    87,    10,    87,    -1,    87,    11,    87,    -1,    87,
      12,    87,    -1,    87,    14,    87,    -1,    18,    87,     1,
      -1,    18,    87,    19,    -1,    -1,    22,    87,    23,    88,
      -1,    22,     1,    -1,    22,    87,     1,    -1,    90,    24,
       3,    -1,     3,    88,    -1,    34,    -1,    33,    -1,    35,
      -1,    46,    -1,    93,    94,    98,    21,    -1,    93,    94,
      21,    -1,    43,    18,    87,    19,    20,    -1,    95,    -1,
      95,    94,    -1,    96,    97,    -1,    44,    87,    26,    -1,
      82,    -1,    95,    -1,    99,    82,    -1,    45,    26,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   140,   140,   144,   152,   157,   167,   171,   175,   182,
     186,   194,   194,   202,   202,   208,   214,   214,   220,   220,
     228,   233,   240,   246,   252,   260,   265,   289,   304,   309,
     314,   319,   324,   329,   358,   364,   389,   397,   434,   440,
     445,   467,   473,   479,   484,   497,   503,   509,   515,   523,
     531,   539,   548,   556,   566,   576,   588,   595,   602,   611,
     621,   631,   642,   647,   654,   695,   701,   707,   713,   736,
     741,   748,   774,   779,   784,   789,   794,   799,   805,   811,
     816,   821,   826,   831,   836,   841,   847,   861,   866,   877,
     883,   890,   931,   956,   964,   972,   980,   989,   996,  1004,
    1029,  1033,  1039,  1045,  1068,  1072,  1077,  1083
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_IDENTIFIER", "TOK_B_AND",
  "TOK_B_OR", "TOK_B_XOR", "TOK_B_NOT", "TOK_AR_PLUS", "TOK_AR_MINUS",
  "TOK_AR_MUL", "TOK_AR_DIV", "TOK_AR_MOD", "TOK_ASSIGN_OP", "TOK_COMP_OP",
  "TOK_IF", "TOK_ELSE", "TOK_ENDEXPR", "TOK_OPENPAR", "TOK_CLOSEPAR",
  "TOK_OPENBR", "TOK_CLOSEBR", "TOK_OPENSQ", "TOK_CLOSESQ", "TOK_DOT",
  "TOK_COMMA", "TOK_DOUBLEDOT", "TOK_ROM_DECL", "TOK_INT_DECL",
  "TOK_FLOAT_DECL", "TOK_FOR_DECL", "TOK_WHILE_DECL", "TOK_DO_DECL",
  "TOK_ROM_CONST", "TOK_INT_CONST", "TOK_FLOAT_CONST", "TOK_PRINT",
  "TOK_READ", "TOK_BREAK", "TOK_CONTINUE", "TOK_GOTO", "TOK_STRUCT",
  "TOK_UNION", "TOK_SWITCH", "TOK_CASE", "TOK_DEFAULT",
  "TOK_STRING_LITERAL", "EXPR_ERROR", "STMNT_BLOCK_ERROR",
  "IF_WITHOUT_ELSE", "NOTX", "UMINUS", "'('", "')'", "$accept", "start",
  "declaration_list", "stmnt_list", "stmnt_block_start", "$@1",
  "stmnt_block", "$@2", "$@3", "$@4", "expr_or_assignment",
  "expression_statement", "stmnt", "declaration_stmt", "struct_def",
  "struct_head", "struct_type", "struct_body", "struct_tail", "print_stmt",
  "read_stmt", "if_stmt", "loop_decl", "loop_while_expr", "loop_for_expr",
  "for_decl", "while_decl", "do_while_decl", "instruction_body", "type",
  "type_name", "left_assign_expr", "assignment", "expr", "array",
  "struct_item", "identifier", "const", "switch_stmt", "switch_head",
  "case_list", "case_stmt", "case_head", "case_body", "default",
  "default_head", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    55,    55,    56,    56,    56,    57,
      57,    59,    58,    61,    60,    60,    62,    60,    63,    60,
      64,    64,    65,    65,    65,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    67,    67,    68,
      69,    70,    70,    71,    72,    73,    73,    73,    73,    74,
      75,    75,    75,    76,    76,    76,    77,    78,    78,    79,
      80,    81,    82,    82,    83,    84,    84,    84,    84,    85,
      85,    86,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    88,    88,    88,
      88,    89,    90,    91,    91,    91,    91,    92,    92,    93,
      94,    94,    95,    96,    97,    97,    98,    99
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     2,     3,     2,     1,
       2,     0,     2,     0,     4,     3,     0,     4,     0,     5,
       1,     1,     1,     2,     2,     1,     2,     3,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     1,     3,
       2,     1,     1,     2,     1,     5,     2,     4,     5,     5,
       5,     7,     4,     3,     3,     5,     3,     4,     5,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     2,     1,
       1,     3,     1,     1,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     0,     4,     2,
       3,     3,     2,     1,     1,     1,     1,     4,     3,     5,
       1,     2,     2,     3,     1,     1,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,    87,     0,     0,     0,    22,     0,    65,    67,    66,
      59,    60,    61,    94,    93,    95,     0,     0,     0,     0,
       0,    41,    42,     0,    96,     0,     4,     5,     0,    25,
       9,     0,    38,     0,     0,    31,    32,    28,    29,     0,
       0,     0,     0,    87,    72,    21,     0,    70,    69,    73,
      30,     0,     0,    26,    92,    87,    72,    77,    78,     0,
       0,    46,     0,     0,     0,    34,    33,    36,    35,     0,
       0,     1,     3,     0,    10,    23,     8,     6,     0,     0,
      68,     0,     0,     0,     0,    11,     0,    62,    63,     0,
      37,    64,     0,    24,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   100,     0,    89,     0,
       0,    85,    86,     0,     0,     0,    27,     0,     7,    43,
      44,    39,     0,    53,     0,    54,    12,     0,     0,     0,
      71,    74,    75,    76,    79,    80,    81,    82,    83,    84,
      91,     0,    98,     0,     0,     0,   101,   104,   105,   102,
      90,    87,    52,     0,     0,    47,     0,     0,     0,     0,
      56,    16,     0,    15,    13,     0,   103,   107,    97,   106,
      88,    50,    45,    48,    49,    99,    57,     0,    20,    17,
      18,    14,    55,     0,    58,    19,    51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    25,    26,    27,    86,   126,    87,   181,   179,   185,
      28,    29,    88,    31,    32,    33,    34,    79,   121,    35,
      36,    37,    38,    84,    82,    39,    40,    41,    89,    42,
      43,    56,    45,    46,    54,    47,    48,    49,    50,    51,
     105,   106,   107,   149,   144,   145
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -76
static const yytype_int16 yypact[] =
{
     233,    -8,   429,   429,    -7,   -76,   429,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,     3,    -2,     7,    14,
      19,   -76,   -76,    18,   -76,    25,   233,   388,    20,   -76,
     -76,    16,   -76,    15,    31,   -76,   -76,   -76,   -76,    22,
      27,   312,    35,    32,    40,   -76,   512,   -76,    33,   -76,
     -76,    12,    59,   -76,   -76,    32,   -76,   -76,   -76,   429,
     488,   -76,   429,   429,    55,   -76,   -76,   -76,   -76,    42,
     429,   -76,   388,    46,   -76,   -76,   -76,   -76,    96,    44,
      47,   132,   312,   429,   312,   -76,   233,   -76,   -76,    54,
     -76,   -76,   429,   -76,   429,   429,   429,   429,   429,   429,
     429,   429,   429,    68,   429,   -15,    12,   274,   -76,   472,
     500,   -76,   -76,   526,    38,    56,   -76,   538,   -76,    96,
     -76,   -76,   132,   -76,   550,   -76,   -76,   192,   154,    27,
     403,   -76,    79,    79,    79,    79,   -76,   -76,   248,   291,
     -76,   444,   -76,    69,    52,   312,   -76,   -76,   -76,   -76,
     -76,    32,   -76,   312,    80,   -76,    97,    82,    81,   426,
     -76,   -76,   350,   -76,   -76,    86,   -76,   -76,   -76,   -76,
     -76,    90,   -76,   -76,   -76,   -76,   -76,    91,   403,   -76,
     -76,   -76,   -76,   312,   -76,   -76,   -76
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -76,   -73,   -25,   -76,   -76,   -76,   -76,   -76,   -76,
     -33,   -71,     2,   -23,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,     4,   -76,   -76,   -76,   -76,   -75,   -76,
     -76,     0,   -76,    17,   -31,   -76,   -76,   -76,   -76,   -76,
      26,    24,   -76,   -76,   -76,   -76
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -41
static const yytype_int16 yytable[] =
{
      44,    72,    30,    73,    61,   119,   142,   123,    65,   125,
     122,    59,    91,   127,    52,    67,    64,    76,    53,    57,
      58,    62,    69,    60,    66,    71,    44,    44,    30,    74,
     143,    68,   147,    77,    80,    78,    70,    75,    90,   155,
      81,    44,    94,    95,    96,    83,    97,    98,    99,   100,
     101,   159,   102,    92,    52,    63,   104,   103,    55,   116,
     108,   128,    55,   118,   115,   120,     2,   -40,     3,   109,
     169,   140,    44,   168,    74,   157,   110,     6,   171,   113,
     114,    44,    44,    94,    44,   129,    44,   117,    30,    99,
     100,   156,    13,    14,    15,   167,    73,   172,   173,   174,
     124,   175,   162,   182,    73,    24,   183,    44,   186,   130,
     184,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     170,   141,    44,     7,     8,     9,   177,    44,    44,    30,
      74,   148,   146,   165,     0,    55,     0,    21,    22,     2,
       0,     3,     0,     0,     0,    44,     0,     0,     0,     5,
       6,     0,     0,    44,     0,   163,     0,     1,     0,    44,
       0,     2,    44,     3,    74,    13,    14,    15,     0,     4,
       0,     5,     6,     0,     0,   164,   178,     0,    24,     0,
       0,     0,     0,    44,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     1,     0,    23,     0,     2,
      24,     3,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     0,     0,   161,     0,     0,     0,     0,     0,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,     1,     0,    24,     0,
       2,     0,     3,     0,     0,     0,     0,     0,     4,     0,
       5,     6,    94,    95,    96,     0,    97,    98,    99,   100,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,     1,     0,    24,
       0,     2,     0,     3,     0,     0,     0,     0,     0,     4,
       0,     5,     6,     0,    85,    94,    95,    96,     0,    97,
      98,    99,   100,   101,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     1,     0,    23,   104,     2,
      24,     3,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     0,    85,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,     1,     0,    23,     0,     2,    24,     3,
       0,     0,     0,     0,     0,     4,     0,     5,     6,     0,
       0,   180,     0,     0,     0,     0,     0,     0,     0,     0,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,     1,     0,    23,     0,     2,    24,     3,     0,     0,
       0,     0,     0,     4,     0,     5,     6,    94,    95,    96,
       0,    97,    98,    99,   100,   101,     0,   102,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    55,
       0,    23,    55,     2,    24,     3,     2,     0,     3,     0,
       0,     0,     0,     0,     6,   176,     0,     6,    94,    95,
      96,     0,    97,    98,    99,   100,   101,     0,   102,    13,
      14,    15,    13,    14,    15,     0,     0,     0,     0,     0,
     166,     0,    24,   150,     0,    24,    94,    95,    96,     0,
      97,    98,    99,   100,   101,     0,   102,     0,     0,   111,
       0,     0,    94,    95,    96,   151,    97,    98,    99,   100,
     101,   152,   102,     0,    94,    95,    96,   112,    97,    98,
      99,   100,   101,    93,   102,     0,    94,    95,    96,   153,
      97,    98,    99,   100,   101,     0,   102,     0,     0,   -20,
      94,    95,    96,     0,    97,    98,    99,   100,   101,     0,
     102,     0,    94,    95,    96,   154,    97,    98,    99,   100,
     101,     0,   102,     0,    94,    95,    96,   158,    97,    98,
      99,   100,   101,     0,   102,     0,     0,     0,     0,   160
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-76))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,    26,     0,    26,     1,    78,    21,    82,     1,    84,
      81,    18,    43,    86,    22,     1,    18,     1,    26,     2,
       3,    18,     3,     6,    17,     0,    26,    27,    26,    27,
      45,    17,   107,    17,     3,    20,    18,    17,     3,     1,
      18,    41,     4,     5,     6,    18,     8,     9,    10,    11,
      12,   122,    14,    13,    22,    52,    44,    24,     3,    17,
       1,    86,     3,    17,    64,    21,     7,    20,     9,    52,
     145,     3,    72,    21,    72,    19,    59,    18,   153,    62,
      63,    81,    82,     4,    84,    31,    86,    70,    86,    10,
      11,    53,    33,    34,    35,    26,   119,    17,     1,    17,
      83,    20,   127,    17,   127,    46,    16,   107,   183,    92,
      19,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     151,   104,   122,    27,    28,    29,   159,   127,   128,   127,
     128,   107,   106,   129,    -1,     3,    -1,    41,    42,     7,
      -1,     9,    -1,    -1,    -1,   145,    -1,    -1,    -1,    17,
      18,    -1,    -1,   153,    -1,     1,    -1,     3,    -1,   159,
      -1,     7,   162,     9,   162,    33,    34,    35,    -1,    15,
      -1,    17,    18,    -1,    -1,    21,   159,    -1,    46,    -1,
      -1,    -1,    -1,   183,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     3,    -1,    43,    -1,     7,
      46,     9,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,
      18,    -1,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,     3,    -1,    46,    -1,
       7,    -1,     9,    -1,    -1,    -1,    -1,    -1,    15,    -1,
      17,    18,     4,     5,     6,    -1,     8,     9,    10,    11,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,     3,    -1,    46,
      -1,     7,    -1,     9,    -1,    -1,    -1,    -1,    -1,    15,
      -1,    17,    18,    -1,    20,     4,     5,     6,    -1,     8,
       9,    10,    11,    12,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     3,    -1,    43,    44,     7,
      46,     9,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,
      18,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     3,    -1,    43,    -1,     7,    46,     9,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    18,    -1,
      -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     3,    -1,    43,    -1,     7,    46,     9,    -1,    -1,
      -1,    -1,    -1,    15,    -1,    17,    18,     4,     5,     6,
      -1,     8,     9,    10,    11,    12,    -1,    14,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     3,
      -1,    43,     3,     7,    46,     9,     7,    -1,     9,    -1,
      -1,    -1,    -1,    -1,    18,    19,    -1,    18,     4,     5,
       6,    -1,     8,     9,    10,    11,    12,    -1,    14,    33,
      34,    35,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    46,     1,    -1,    46,     4,     5,     6,    -1,
       8,     9,    10,    11,    12,    -1,    14,    -1,    -1,     1,
      -1,    -1,     4,     5,     6,    23,     8,     9,    10,    11,
      12,     1,    14,    -1,     4,     5,     6,    19,     8,     9,
      10,    11,    12,     1,    14,    -1,     4,     5,     6,    19,
       8,     9,    10,    11,    12,    -1,    14,    -1,    -1,    17,
       4,     5,     6,    -1,     8,     9,    10,    11,    12,    -1,
      14,    -1,     4,     5,     6,    19,     8,     9,    10,    11,
      12,    -1,    14,    -1,     4,     5,     6,    19,     8,     9,
      10,    11,    12,    -1,    14,    -1,    -1,    -1,    -1,    19
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     7,     9,    15,    17,    18,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    46,    55,    56,    57,    64,    65,
      66,    67,    68,    69,    70,    73,    74,    75,    76,    79,
      80,    81,    83,    84,    85,    86,    87,    89,    90,    91,
      92,    93,    22,    26,    88,     3,    85,    87,    87,    18,
      87,     1,    18,    52,    18,     1,    17,     1,    17,     3,
      18,     0,    57,    67,    66,    17,     1,    17,    20,    71,
       3,    18,    78,    18,    77,    20,    58,    60,    66,    82,
       3,    88,    13,     1,     4,     5,     6,     8,     9,    10,
      11,    12,    14,    24,    44,    94,    95,    96,     1,    87,
      87,     1,    19,    87,    87,    85,    17,    87,    17,    56,
      21,    72,    65,    82,    87,    82,    59,    56,    57,    31,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
       3,    87,    21,    45,    98,    99,    94,    82,    95,    97,
       1,    23,     1,    19,    19,     1,    53,    19,    19,    65,
      19,    21,    57,     1,    21,    77,    26,    26,    21,    82,
      88,    82,    17,     1,    17,    20,    19,    64,    87,    62,
      21,    61,    17,    16,    19,    63,    82
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    { 
		(yyval._node) = NULL; 
	}
    break;

  case 3:

    {
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, (yyvsp[(2) - (2)]._node)->ptNode);
		astTree = (yyvsp[(2) - (2)]._node)->astNode;
		ptTree = ptNode;
	}
    break;

  case 4:

    {
		(yyval._node) = NULL;
	}
    break;

  case 5:

    {
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, (yyvsp[(1) - (1)]._node)->ptNode);
		astTree = (yyvsp[(1) - (1)]._node)->astNode;
		ptTree = ptNode;
	}
    break;

  case 6:

    {
		(yyval._node) = NULL;
	}
    break;

  case 7:

    {
		(yyval._node) = NULL;
	}
    break;

  case 8:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, (yylsp[(2) - (2)])),
				nullptr);
	}
    break;

  case 9:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 10:

    {
		(yyval._node) = addStmntToBlock((yyvsp[(1) - (2)]._node), (yyvsp[(2) - (2)]._node));
	}
    break;

  case 11:

    {TBlockContext::Push();}
    break;

  case 12:

    {
        (yyval._node) = (yyvsp[(1) - (2)]._node);
      }
    break;

  case 13:

    {TBlockContext::Pop();}
    break;

  case 14:

    {
		(yyval._node) = createNode((yyvsp[(2) - (4)]._node)->astNode, 
				createPtNodeWithChildren("stmnt_block", 3, (yyvsp[(1) - (4)]._node)->ptNode, (yyvsp[(2) - (4)]._node)->ptNode, (yyvsp[(3) - (4)]._node)->ptNode));
	}
    break;

  case 15:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACE, (yylsp[(3) - (3)])), 
				nullptr);
	}
    break;

  case 16:

    {TBlockContext::Pop();}
    break;

  case 17:

    {
		(yyval._node) = createNode(nullptr, 
				createPtNodeWithChildren("stmnt_block", 2, (yyvsp[(1) - (4)]._node)->ptNode, (yyvsp[(3) - (4)]._node)->ptNode));
	}
    break;

  case 18:

    {TBlockContext::Pop();}
    break;

  case 19:

    {
		(yyval._node) = createNode((yyvsp[(3) - (5)]._node)->astNode, 
				createPtNodeWithChildren("stmnt_block", 3, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode));
	}
    break;

  case 20:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 21:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 22:

    {
		(yyval._node) = createNode(nullptr, 
				createPtNodeWithChildren("expression_statement", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 23:

    {
		(yyval._node) = createNode((yyvsp[(1) - (2)]._node)->astNode, 
				createPtNodeWithChildren("expression_statement", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 24:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, (yylsp[(2) - (2)])), 
				nullptr);
	}
    break;

  case 25:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 26:

    {
		AstNode *astNode;
		char *labelName = (yyvsp[(1) - (2)]._node)->ptNode->text;

		if(Context.IsLabelDeclared(labelName))
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, LABEL_IS_ALREADY_DECLARED, (yylsp[(1) - (2)]));
		}
		else
		{
			TLabel* label = Context.GetLabel(labelName);
			if(label == NULL)
			{
				label = Context.MakeLabel(labelName);
			}
			label->SetDeclaredLine(((yylsp[(1) - (2)])).first_line);
			astNode = new LabelAstNode(label);
		}

		(yyval._node) = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 27:

    {
		char *labelName = (yyvsp[(2) - (3)]._node)->ptNode->text;
		TLabel* label =  Context.GetLabel(labelName);
		
		if(label == NULL)
		{
			label = Context.MakeLabel(labelName);
		}
		label->SetUsedLine(((yylsp[(1) - (3)])).first_line);

		(yyval._node) = createNode(new OperatorAstNode(OP_GOTO, new LabelAstNode(label)), 
				createPtNodeWithChildren("stmnt", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
	}
    break;

  case 28:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 29:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 30:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 31:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 32:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 33:

    {
		AstNode *astNode;
		 
		if(!Context.CanUseBreak())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_BREAK_ERROR, (yylsp[(1) - (2)]));
		}
		else
		{
			TOperator *op = Context.OperatorStackTop();
			TLabel *EndLabel;
			switch(op->GetType())
			{
			case OT_FOR:
			case OT_WHILE:
			case OT_DO_WHILE:
				EndLabel = ((TSimpleOperator *)op)->GetOutLabel(); 
				break;
			case OT_SWITCH:
				EndLabel = ((TSwitchOperator *)op)->GetEndLabel();
				break;
			}
			astNode = new OperatorAstNode(OP_BREAK, new LabelAstNode(EndLabel));
		}
		(yyval._node) = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 34:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, (yylsp[(2) - (2)])), 
				nullptr);
	}
    break;

  case 35:

    {
		AstNode *astNode;
		if(!Context.CanUseContinue())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_CONTINUE_ERROR, (yylsp[(1) - (2)]));
		}
		else
		{
			TOperator *op = Context.OperatorStackTop();
			TLabel *StartLabel;
			switch(op->GetType())
			{
			case OT_FOR:
			case OT_WHILE:
			case OT_DO_WHILE:
				StartLabel = ((TSimpleOperator *)op)->GetEntranceLabel(); 
				break;
			}
			astNode = new OperatorAstNode(OP_CONTINUE, new LabelAstNode(StartLabel));
		}
		(yyval._node) = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 36:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, (yylsp[(2) - (2)])), 
				nullptr);
	}
    break;

  case 37:

    {
		char *typeName = (yyvsp[(2) - (2)]._node)->ptNode->text;
		(yyval._node) = nullptr;
		if(Context.OnUserTypeDefinition())
		{
			//if(!Context.IsBaseType(typeName))
			//{
			//	$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, USER_TYPE_STRUCT_FIELD, @id),
			//			nullptr);
			//}
			//else
			{
				auto userType = dynamic_cast<StructType*>(Context.TopUserType());
				if(userType->IsFieldDefined(std::string(typeName)))
				{
					(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, STRUCT_FIELD_REDEFINITION, (yylsp[(2) - (2)])),
							nullptr);
				}
				else
				{
					userType->AddField((yyvsp[(1) - (2)]._node)->astNode->GetResultType()->Clone(), typeName);
				}
			}
		}
		else
		{
			Context.DeclVar(typeName, (yyvsp[(1) - (2)]._node)->astNode->GetResultType()->Clone(), (yylsp[(2) - (2)]));
		}
		
		if ((yyval._node) == nullptr)
		{
			(yyval._node) = createNode(new DeclIDAstNode((yyvsp[(1) - (2)]._node)->astNode->GetResultType()->Clone()), 
					createPtNodeWithChildren("stmnt", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
		}
	}
    break;

  case 38:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 39:

    {
		(yyval._node) = (yyvsp[(1) - (3)]._node);
	}
    break;

  case 40:

    {
		AstNode *verboseNode = nullptr;
		// Вложенное определение пользовательских типов - ошибка
		if(Context.OnUserTypeDefinition())
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EMBEDDED_USER_TYPE_DEFINITION, (yylsp[(1) - (2)]));
		// Проверка на переопределение 
		else if(Context.IsTypeDefined((yyvsp[(2) - (2)]._node)->ptNode->text))
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, TYPE_REDEFINITION_ERROR, (yylsp[(2) - (2)]));
		else
		{
			// Помещение структуры в стек
			auto t = (StructType*)(yyvsp[(1) - (2)]._node);
			t->SetTypeName((yyvsp[(2) - (2)]._node)->ptNode->text);
			Context.PushUserType(t);
		}
		// NOTE: $_struct does NOT return Node* variable!
		(yyval._node) = createNode(verboseNode, 
				createPtNodeWithChildren("struct", 1, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 41:

    {
		// NOTE: No actual Nodes here!
		(yyval._node) = (Node*)new StructType();
	}
    break;

  case 42:

    {
		// NOTE: No actual Nodes here!
		(yyval._node) = (Node*)new UnionType();
	}
    break;

  case 43:

    {
		(yyval._node) = NULL;
	}
    break;

  case 44:

    {
		// Выталкивание структуры из стека
		if(Context.OnUserTypeDefinition())
		{
			// Добавление в таблицу типов
			Context.AddUserTypeToTable(Context.PopUserType());
		}
		(yyval._node) = NULL;
	}
    break;

  case 45:

    {
		(yyval._node) = createNode(new OperatorAstNode(OP_OUTPUT, (yyvsp[(3) - (5)]._node)->astNode), 
				createPtNodeWithChildren("stmnt", 5, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode, (yyvsp[(5) - (5)]._node)->ptNode)); 
	}
    break;

  case 46:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_OPEN_PARANTHESIS, (yylsp[(2) - (2)])),
				nullptr);
	}
    break;

  case 47:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, (yylsp[(4) - (4)])),
				nullptr);
	}
    break;

  case 48:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, (yylsp[(5) - (5)])),
				nullptr);
	}
    break;

  case 49:

    {
		(yyval._node) = createNode(new OperatorAstNode(OP_INPUT, (yyvsp[(2) - (5)]._node)->astNode),
				createPtNodeWithChildren("stmnt", 5, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode, (yyvsp[(5) - (5)]._node)->ptNode)); 
	}
    break;

  case 50:

    {
		AssertOneOfTypes((yyvsp[(3) - (5)]._node), (yylsp[(3) - (5)]), 1, BOOL_TYPE);

		(yyval._node) = createNode(new ConditionalAstNode((yyvsp[(3) - (5)]._node)->astNode, (yyvsp[(5) - (5)]._node)->astNode), 
				createPtNodeWithChildren("stmnt", 5, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode, (yyvsp[(5) - (5)]._node)->ptNode));
	}
    break;

  case 51:

    {
		AssertOneOfTypes((yyvsp[(3) - (7)]._node), (yylsp[(3) - (7)]), 1, BOOL_TYPE);

		(yyval._node) = createNode(new ConditionalAstNode((yyvsp[(3) - (7)]._node)->astNode, (yyvsp[(5) - (7)]._node)->astNode, (yyvsp[(7) - (7)]._node)->astNode), 
				createPtNodeWithChildren("stmnt", 7, (yyvsp[(1) - (7)]._node)->ptNode, (yyvsp[(2) - (7)]._node)->ptNode, (yyvsp[(3) - (7)]._node)->ptNode, (yyvsp[(4) - (7)]._node)->ptNode,
					(yyvsp[(5) - (7)]._node)->ptNode, (yyvsp[(6) - (7)]._node)->ptNode, (yyvsp[(7) - (7)]._node)->ptNode));
	}
    break;

  case 52:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, (yylsp[(4) - (4)])),
				nullptr);
	}
    break;

  case 53:

    {
		auto condition = dynamic_cast<LoopConditionAstNode *>((yyvsp[(2) - (3)]._node)->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator*>(Context.OperatorStackPop());
		
		(yyval._node) = createNode(new LoopAstNode(condition, (yyvsp[(3) - (3)]._node)->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
		delete OperatorData;
	}
    break;

  case 54:

    {
		auto condition = dynamic_cast<LoopConditionAstNode *>((yyvsp[(2) - (3)]._node)->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator *>(Context.OperatorStackPop());
		
		(yyval._node) = createNode(new LoopAstNode(condition, (yyvsp[(3) - (3)]._node)->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
		delete OperatorData;
	}
    break;

  case 55:

    {
		auto condition = dynamic_cast<LoopConditionAstNode *>((yyvsp[(4) - (5)]._node)->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator *>(Context.OperatorStackPop());
		
		(yyval._node) = createNode(new LoopAstNode(condition, (yyvsp[(2) - (5)]._node)->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel(), true), 
				createPtNodeWithChildren("loop", 5, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode, (yyvsp[(5) - (5)]._node)->ptNode));
		delete OperatorData;
	}
    break;

  case 56:

    {
		(yyval._node) = createNode(new LoopConditionAstNode(nullptr, (yyvsp[(2) - (3)]._node)->astNode, nullptr), 
				createPtNodeWithChildren("loop_statements", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
	}
    break;

  case 57:

    {
		(yyval._node) = createNode(new LoopConditionAstNode((yyvsp[(2) - (4)]._node)->astNode, (yyvsp[(3) - (4)]._node)->astNode, nullptr), 
				createPtNodeWithChildren("loop_statements", 4, (yyvsp[(1) - (4)]._node)->ptNode, (yyvsp[(2) - (4)]._node)->ptNode,
					(yyvsp[(3) - (4)]._node)->ptNode, (yyvsp[(4) - (4)]._node)->ptNode));
	}
    break;

  case 58:

    {
		(yyval._node) = createNode(new LoopConditionAstNode((yyvsp[(2) - (5)]._node)->astNode, (yyvsp[(3) - (5)]._node)->astNode, (yyvsp[(4) - (5)]._node)->astNode), 
				createPtNodeWithChildren("loop_statements", 5, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode,
					(yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode, (yyvsp[(5) - (5)]._node)->ptNode));
	}
    break;

  case 59:

    {
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto forOp = new TSimpleOperator(OT_FOR, controlFLowLabel, end);
		Context.OperatorStackPush(forOp);
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 60:

    {
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto whileOp = new TSimpleOperator(OT_WHILE, controlFLowLabel, end);
		Context.OperatorStackPush(whileOp);
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 61:

    {
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto whileOp = new TSimpleOperator(OT_DO_WHILE, controlFLowLabel, end);
		Context.OperatorStackPush(whileOp);
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 62:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 63:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 64:

    {
		DimensionAstNode *dimNode = dynamic_cast<DimensionAstNode*>((yyvsp[(2) - (2)]._node)->astNode);
		if (dimNode->GetExpr() == nullptr)
		{
			(yyval._node) = (yyvsp[(1) - (2)]._node);
		}
		else
		{
			(yyval._node) = nullptr;

			std::vector<int> sizes;
			for (DimensionAstNode *cur = dimNode; cur != nullptr && cur->GetNextDim() != nullptr; )
			{
				auto numValueNode = dynamic_cast<NumValueAstNode*>(cur->GetExpr());
				if (numValueNode == nullptr && (yyval._node) == nullptr)
				{
					(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_ARRAY_DECLARATION, (yylsp[(2) - (2)])),
							nullptr);
				}
				sizes.emplace_back(numValueNode->ToInt());
				
				// we will now delete the DimensionAstNode's as we no more need them
				// as we converted the info to a more comfortable vector format
				AstNode *del = cur;
				cur = cur->GetNextDim();
				delete del;
			}	

			if ((yyval._node) == nullptr) // IF there were no errors...
			{
				(yyval._node) = createNode(new DeclIDAstNode(new ArrayType((yyvsp[(1) - (2)]._node)->astNode->GetResultType()->Clone(), sizes)), 
						createPtNodeWithChildren("array decl", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
			}

			// we will also delete the original DeclIDAstNode as we only need type info
			delete (yyvsp[(1) - (2)]._node)->astNode;
		}
	}
    break;

  case 65:

    {
        (yyval._node) = createNode(new DeclIDAstNode(new RomanType()), 
				createPtNodeWithChildren("type", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 66:

    {
        (yyval._node) = createNode(new DeclIDAstNode(new FloatType()), 
				createPtNodeWithChildren("type", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 67:

    {
        (yyval._node) = createNode(new DeclIDAstNode(new IntType()), 
				createPtNodeWithChildren("type", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 68:

    {
		// Token for the user-defined type
		
		// NOTE: Deleting the allocated type var as we do not define it here - we use it.	
		delete (yyvsp[(1) - (2)]._node); 
		
		StructType *typeRef = Context.GetUserType((yyvsp[(2) - (2)]._node)->ptNode->text);
		if(typeRef == nullptr)
		{
			(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_TYPE, (yylsp[(2) - (2)])),
					nullptr);
		}
		else
		{
			(yyval._node) = createNode(new DeclIDAstNode(typeRef), 
					createPtNodeWithChildren("type", 1, (yyvsp[(2) - (2)]._node)->ptNode));
		}
	}
    break;

  case 69:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 70:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 71:

    {
		BaseTypeInfo *type = (yyvsp[(1) - (3)]._node)->astNode->GetResultType();
		AssertOneOfTypes((yyvsp[(3) - (3)]._node), (yylsp[(3) - (3)]), 1, type->getID());
		
		TVariable *var = Context.getVar((yyvsp[(1) - (3)]._node)->ptNode->firstChild->text, 1, NULL, (yylsp[(1) - (3)]));

		auto constValueNode = dynamic_cast<NumValueAstNode*>((yyvsp[(3) - (3)]._node)->astNode);
		if (constValueNode != nullptr)
		{
			auto varNode = dynamic_cast<IValueHolderNode*>((yyvsp[(1) - (3)]._node)->astNode);
			if (varNode != nullptr)
			{
				if (varNode->SetValue(constValueNode) == 0)
				{
					(yyval._node) = (yyvsp[(1) - (3)]._node);	
					break;
				}
			}
		}

		(yyval._node) = createNode(new OperatorAstNode((yyvsp[(2) - (3)]._node)->ptNode->text, (yyvsp[(1) - (3)]._node)->astNode, (yyvsp[(3) - (3)]._node)->astNode), 
				createPtNodeWithChildren("expr", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
	}
    break;

  case 72:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 73:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 74:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), true, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 75:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), true, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 76:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), true, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 77:

    {
		YYLTYPE dummy = {0, 0, 0, 0};
		(yyval._node) = CreateExpressionNode((yyvsp[(1) - (2)]._node), true, (yyvsp[(2) - (2)]._node), nullptr, (yylsp[(2) - (2)]), dummy);
	}
    break;

  case 78:

    {
		YYLTYPE dummy = {0, 0, 0, 0};
		(yyval._node) = CreateExpressionNode((yyvsp[(1) - (2)]._node), false, (yyvsp[(2) - (2)]._node), nullptr, (yylsp[(2) - (2)]), dummy);
	}
    break;

  case 79:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 80:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 81:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 82:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 83:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 84:

    {
		(yyval._node) = CreateExpressionNode((yyvsp[(2) - (3)]._node), false, (yyvsp[(1) - (3)]._node), (yyvsp[(3) - (3)]._node), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
	}
    break;

  case 85:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, (yylsp[(3) - (3)])),
				nullptr);
	}
    break;

  case 86:

    {
		(yyval._node) = createNode((yyvsp[(2) - (3)]._node)->astNode, 
				createPtNodeWithChildren("expr", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
	}
    break;

  case 87:

    {
		(yyval._node) = createNode(new DimensionAstNode(nullptr, nullptr, nullptr), 
				createPtNode("array_end"));
	}
    break;

  case 88:

    {
		AstNode *astNode = new DimensionAstNode(
			(yyvsp[(2) - (4)]._node)->astNode->GetResultType(), 
			(yyvsp[(2) - (4)]._node)->astNode,
			dynamic_cast<DimensionAstNode*>((yyvsp[(4) - (4)]._node)->astNode));
		PtNode *ptNode = createPtNodeWithChildren("array_dimension", 4, (yyvsp[(4) - (4)]._node)->ptNode, (yyvsp[(1) - (4)]._node)->ptNode, (yyvsp[(2) - (4)]._node)->ptNode, (yyvsp[(3) - (4)]._node)->ptNode);
        
		(yyval._node) = createNode(astNode, ptNode);
	}
    break;

  case 89:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_ARRAY_ITEM, (yylsp[(2) - (2)])),
				nullptr);
	}
    break;

  case 90:

    {
		(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACKET, (yylsp[(3) - (3)])),
				nullptr);
	}
    break;

  case 91:

    {
		AssertOneOfTypes((yyvsp[(1) - (3)]._node), (yylsp[(1) - (3)]), 2, STRUCT_TYPE, UNION_TYPE);
		//if(FALSE == CheckNodeTypeByTypeId($1, TYPE_STRUCT) 
		//	&& FALSE == CheckNodeTypeByTypeId($1, TYPE_UNION))
		//{
		//	yyerror(INVALID_STRUCT_FIELD_L_VALUE);
		//	$$ = ErrorNode();
		//}
		//else 

		auto astNode = dynamic_cast<VarAstNode*>((yyvsp[(1) - (3)]._node)->astNode);
		if (astNode == nullptr)
		{
			(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_STRUCT_FIELD, (yylsp[(3) - (3)])),
					nullptr);
		}
		else
		{
			StructType *varType = dynamic_cast<StructType*>(astNode->GetResultType());
			std::string fieldName((yyvsp[(3) - (3)]._node)->ptNode->text);

			if(!varType->IsFieldDefined(fieldName))
			{
				(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_STRUCT_FIELD, (yylsp[(3) - (3)])),
						nullptr);
			}
			else
			{
				StructType *s = varType;
				TVariable *fieldVar = s->GetField(fieldName);
				//AstNode *fieldNode = new VariableAstNode(false, field);

				(yyval._node) = createNode(new StructAddressAstNode(astNode, fieldVar),
						createPtNodeWithChildren("identifier", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
			}
		}
	}
    break;

  case 92:

    {
		char *id_name = (yyvsp[(1) - (2)]._node)->ptNode->text;
		auto dimNode = dynamic_cast<DimensionAstNode*>((yyvsp[(2) - (2)]._node)->astNode);
		TVariable *val = Context.getVar(id_name, 1, dimNode, (yylsp[(1) - (2)]));
		AstNode *astNode;
		if (val)
		{
			astNode = new VarAstNode(false, val);
			// TODO [SV] 15.08.13 12:18: possible checks for array\non-array type equality
			if (dimNode->GetExpr() != nullptr)
				astNode = new ArrayAddressAstNode(static_cast<VarAstNode*>(astNode), dimNode);				
		}
		else
		{
			print_error("unknown identifier", (yylsp[(1) - (2)])); // I guess this would be better
			//astNode = new VarAstNode(id_name, VOID_TYPE, false, false, $ar_decl->astNode);
		}

		(yyval._node) = createNode(astNode, 
				createPtNodeWithChildren("identifier", 1, (yyvsp[(1) - (2)]._node)->ptNode));
	}
    break;

  case 93:

    {
		char *id = strdup((yyvsp[(1) - (1)]._node)->ptNode->text);

		(yyval._node) = createNode(new NumValueAstNode(id, new IntType()), 
				createPtNodeWithChildren("const", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 94:

    {
		char *id = strdup((yyvsp[(1) - (1)]._node)->ptNode->text);

		(yyval._node) = createNode(new NumValueAstNode(id, new RomanType()), 
				createPtNodeWithChildren("const", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 95:

    {
		char *id = strdup((yyvsp[(1) - (1)]._node)->ptNode->text);

		(yyval._node) = createNode(new NumValueAstNode(id, new FloatType()), 
				createPtNodeWithChildren("const", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 96:

    {
		char *id = strdup((yyvsp[(1) - (1)]._node)->ptNode->text);

		(yyval._node) = createNode(new NumValueAstNode((yyvsp[(1) - (1)]._node)->ptNode->text, new LiteralType(strlen(id))), 
				createPtNodeWithChildren("const", 1, (yyvsp[(1) - (1)]._node)->ptNode));
	}
    break;

  case 97:

    {
		auto switchOp = dynamic_cast<TSwitchOperator*>(Context.OperatorStackPop());

		(yyval._node) = createNode(new SwitchAstNode((yyvsp[(1) - (4)]._node)->astNode, (yyvsp[(2) - (4)]._node)->astNode, (yyvsp[(3) - (4)]._node)->astNode, switchOp),
				createPtNodeWithChildren("switch_stmt", 4, (yyvsp[(1) - (4)]._node)->ptNode, (yyvsp[(2) - (4)]._node)->ptNode, (yyvsp[(3) - (4)]._node)->ptNode, (yyvsp[(4) - (4)]._node)->ptNode));
	}
    break;

  case 98:

    {
		auto switchOp = dynamic_cast<TSwitchOperator*>(Context.OperatorStackPop());
		
		(yyval._node) = createNode(new SwitchAstNode((yyvsp[(1) - (3)]._node)->astNode, (yyvsp[(2) - (3)]._node)->astNode, nullptr, switchOp),
				createPtNodeWithChildren("switch_stmt", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
	}
    break;

  case 99:

    {
		//AstNode *astNode;
		AssertOneOfTypes((yyvsp[(3) - (5)]._node), (yylsp[(3) - (5)]), 1, INT_TYPE);

		if((yyvsp[(3) - (5)]._node)->astNode->GetResultType()->getID() != INT_TYPE)
		{
			Context.OperatorStackPush(new TSwitchOperator(nullptr, 0, 0));

			(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_SWITCH_KEY_TYPE, (yylsp[(3) - (5)])),
					nullptr);
		}
		else
		{
			TLabel 
				*controlFLowLabel = Context.GenerateNewLabel(),
				*end = Context.GenerateNewLabel();
			TSwitchOperator *switchOp = new TSwitchOperator((yyvsp[(3) - (5)]._node)->astNode, controlFLowLabel, end);
			Context.OperatorStackPush(switchOp);
			(yyval._node) = createNode((yyvsp[(3) - (5)]._node)->astNode,
					createPtNodeWithChildren("switch_head", 4, (yyvsp[(1) - (5)]._node)->ptNode, (yyvsp[(2) - (5)]._node)->ptNode, (yyvsp[(3) - (5)]._node)->ptNode, (yyvsp[(4) - (5)]._node)->ptNode));
		}
	}
    break;

  case 100:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 101:

    {
		(yyval._node) = createNode(new OperatorAstNode(OP_LIST, (yyvsp[(1) - (2)]._node)->astNode, (yyvsp[(2) - (2)]._node)->astNode),
				createPtNodeWithChildren("case_list", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 102:

    {
		(yyval._node) = createNode(new OperatorAstNode(OP_CASE, (yyvsp[(1) - (2)]._node)->astNode, (yyvsp[(2) - (2)]._node)->astNode),
				createPtNodeWithChildren("case_stmt", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 103:

    {
		//TODO: проверка значения ключа! оно не может повторяться
		AssertOneOfTypes((yyvsp[(2) - (3)]._node), (yylsp[(2) - (3)]), 1, INT_TYPE);
		
		if(Context.IsRepeatedCaseKeyVal((yyvsp[(2) - (3)]._node)->astNode))
		{
			(yyval._node) = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, REPEATED_CASE_KEY_VALUE, (yylsp[(2) - (3)])),
					nullptr);
		}
		else
		{
			TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
			
			TLabel *label = Context.GenerateNewLabel();
			TCaseOperator *caseOp = new TCaseOperator((yyvsp[(2) - (3)]._node)->astNode, label);
			switchOp->AddCase(caseOp);
			(yyval._node) = createNode(new LabelAstNode(label),
					createPtNodeWithChildren("case_head", 3, (yyvsp[(1) - (3)]._node)->ptNode, (yyvsp[(2) - (3)]._node)->ptNode, (yyvsp[(3) - (3)]._node)->ptNode));
		}	
	}
    break;

  case 104:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 105:

    {
		(yyval._node) = (yyvsp[(1) - (1)]._node);
	}
    break;

  case 106:

    {
		(yyval._node) = createNode(new OperatorAstNode(OP_DEFAULT, (yyvsp[(1) - (2)]._node)->astNode, (yyvsp[(2) - (2)]._node)->astNode),
				createPtNodeWithChildren("default", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;

  case 107:

    {
		TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
		TLabel *label = Context.GenerateNewLabel();
		TDefaultOperator *defOp = new TDefaultOperator(label);
		switchOp->AddDefaultOp(defOp);

		(yyval._node) = createNode(new LabelAstNode(label),
				createPtNodeWithChildren("default_head", 2, (yyvsp[(1) - (2)]._node)->ptNode, (yyvsp[(2) - (2)]._node)->ptNode));
	}
    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}





void yyerror(const char *message)
{
}

