#define FAIL -1
#define OK 0
#define MAXKEYWORDCLASSLENGTH 3
#define NOKEYWORD -1

static char* keyWords[] =
{
	"BEGIN",    "CALL",     "CONST",    "DO",   "END",  "IF",
	"ODD",      "PROCEDURE","THEN",     "VAR",  "WHILE"
};

/*
Character classes:
 0 - continue
 1 - special character (in Z0) except '='
 2 - Alpha
 3 - Numeral
 4 - ':'
 5 - '<'
 6 - '>'
 7 - '='
*/

static char cLookupTable[] =
{
/*         0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F*/
/*0 */     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*10*/     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*20*/     0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
/*30*/     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 1, 5, 7, 6, 1,
/*40*/     0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*50*/     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
/*60*/     0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*70*/     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0
};

static int keywordClasses[][MAXKEYWORDCLASSLENGTH] =
{
/*0*/	{-1, -1, -1},
/*1*/   {-1, -1, -1},
/*2*/   { 5,  3, -1},
/*3*/   { 4,  6,  9},
/*4*/   { 1,  8, -1},
/*5*/   { 0,  2, 10},
/*6*/   {-1, -1, -1},
/*7*/   {-1, -1, -1},
/*8*/   {-1, -1, -1},
/*9*/   { 7, -1, -1}
};

typedef enum T_MC{mcEmpty, mcSymb, mcNum, mcIdent, mcEOF} tMC;
typedef enum T_ZS{
	zNIL,
	zERG = 128, zLE, zGE,
	zBGN, zCLL, zCST, zDO, zEND, zIF, zODD, zPRC, zTHN, zVAR, zWHL
}tZS;

typedef struct{
	tMC MC;                     //Morphemcode
	int PosLine;                //Line
	int PosCol;                 //Column
	union VAL{
		long Num;
		char* pStr;
		unsigned long Symb;
	}Val;
	int MLen;
} tMorph;

static void        finalize (void);
void               readc    (void);
void               writec   (void);
int                initLex  (char* fname);
tMorph*            Lex      (void);

typedef void (*FX)(void);

/*
Character classes:
 0 - continue
 1 - special character (in Z0) except '='
 2 - Alpha
 3 - Numeral
 4 - ':'
 5 - '<'
 6 - '>'
 7 - '='
*/

static char edgeLookupTable[][8] =
{
/*      Ct Sp Al Nm :  <  >  = */
/* 0 */ 0, 0, 1, 2, 3, 4, 5, 0,
/* 1 */ 0, 0, 1, 1, 0, 0, 0, 0,
/* 2 */ 0, 0, 0, 2, 0, 0, 0, 0,
/* 3 */ 0, 0, 0, 0, 0, 0, 0, 6,
/* 4 */ 0, 0, 0, 0, 0, 0, 0, 7,
/* 5 */ 0, 0, 0, 0, 0, 0, 0, 8,
/* 6 */ 0, 0, 0, 0, 0, 0, 0, 0,
/* 7 */ 0, 0, 0, 0, 0, 0, 0, 0,
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0
};

static FILE*    pIF;
static tMorph   MorphInit;
static tMorph   Morph;
static int      c, state, Z, lastZ, wroteLastC = 1;
static int      line, col;
static char     vBuf[1024+1];
static char*    pBuf;
static int      checkKeyword(tMorph* m);
