#include <stdio.h>
#include <stdlib.h>

typedef unsigned long ul;

typedef unsigned long tIdxGr;

typedef enum BOGEN_DESC
{   
    BgNl= 0,  // NIL     
    BgSy= 1,  // Symbol  
    BgMo= 2,  // Morphem 
    BgGr= 4,  // Graph   
    BgEn= 8,  // Graphende 
}tBg;

typedef struct BOGEN
{
    tBg BgD;  
    union BGX 
    {
        unsigned long X; 
        int           S;    //Symbol
        tMC           M;    //Morphem
        tIdxGr        G;    //Graph
    }   BgX;
    int (*fx)(void); 
    int iNext; 
    int iAlt; 
}tBog;

static tBog gFact[];
static tBog gProgram[];
static tBog gCond[];
static tBog gBlock[];
static tBog gTerm[];
static tBog gStmnt[];
static tBog gExpr[];

static tBog gFact[]=
{
/* 0*/ {BgMo, {(ul)mcIdent	}, NULL, 5,		1},/*(0)---ident--->(E)*/
/* 1*/ {BgMo, {(ul)mcNum 	}, NULL, 5,		2},/* +---number--->(E)*/
/* 2*/ {BgSy, {(ul)'('    	}, NULL, 3,		0},/*(+)----'('---->(3)*/
/* 3*/ {BgGr, {(ul)gExpr  	}, NULL, 4,		0},/*(1)---express->(4)*/
/* 4*/ {BgSy, {(ul)')'    	}, NULL, 5,		0},/*(0)----')'---->(E)*/
/* 5*/ {BgEn, {(ul)0      	}, NULL, 0,		0} /*(E)--------(ENDE) */
};

static tBog gProgram[] =
{
/* 0 */ {BgGr, {(ul)gBlock  }, NULL, 1,		0}, /*(0)---Block--->('.')*/
/* 1 */ {BgMo, {(ul)'.'     }, NULL, 2,		0}, /*('.')--------->Ende*/
/* 2 */ {BgEn, {(ul)0       }, NULL, 0,		0}
};

static tBog gCond[] =
{
/* 0  */ {BgSy, {(ul)zODD	}, NULL, 1, 	2},/* (0)---------'odd'--------> expression	*/
/* 1  */ {BgGr, {(ul)gExpr	}, NULL,10, 	0},/* 'odd'-------expression---> (E)		*/
/* 2  */ {BgGr, {(ul)gExpr	}, NULL, 3,		0},/* (0)---------expression---> '='		*/
/* 3  */ {BgSy, {(ul)'='	}, NULL, 9,		4},/* expression--'='----------> expression	*/
/* 4  */ {BgSy, {(ul)'#'	}, NULL, 9,		5},/* expression--'#'----------> expression	*/
/* 5  */ {BgSy, {(ul)'<'	}, NULL, 9,		6},/* expression--'<'----------> expression */
/* 6  */ {BgSy, {(ul)zLE	}, NULL, 9,		7},/* expression--'<='---------> expression */
/* 7  */ {BgSy, {(ul)'>'	}, NULL, 9,		8},/* expression--'>'----------> expression */
/* 8  */ {BgSy, {(ul)zGE	}, NULL, 9,		0},/* expression--'>='---------> expression */
/* 9  */ {BgGr,	{(ul)gExpr	}, NULL,10,		0},/* mcSymb------expr---------> (E)		*/
/* 10 */ {BgEn,	{(ul)0		}, NULL, 0,		0},/* (E)----------------------> Ende		*/
};

static tBog gExpr[] =
{
/* 0  */ {BgSy, {(ul)'-'	}, NULL, 1,		3},/* (0)----------'-'---------> expression	*/
/* 1  */ {BgGr,	{(ul)gTerm	}, NULL, 2,		0},/* '-'----------term--------> 2			*/
/* 2  */ {BgNl, {(ul)0		}, NULL, 6,		0},/* (0)---------'odd'--------> expression	*/
/* 3  */ {BgGr,	{(ul)gTerm	}, NULL, 2,		0},/* (0)----------term--------> 2			*/
/* 4  */ {BgGr, {(ul)gTerm	}, NULL, 2,		0},/* '+'----------term--------> 2			*/
/* 5  */ {BgGr, {(ul)gTerm	}, NULL, 2,		0},/* '+'----------term--------> 2			*/ //Perhaps obsolete?
/* 6  */ {BgSy,	{(ul)'+'	}, NULL, 4,		7},/* NL-----------'+'---------> term		*/
/* 7  */ {BgSy,	{(ul)'-'	}, NULL, 4,		8},/* NL-----------'-'---------> term		*/
/* 8  */ {BgEn, {(ul)0		}, NULL, 0,		0} /* NL-----------------------> Ende		*/
/* 9  */ 
/* 10 */ 
};

static tBog gTerm[] =
{
/* 0  */ {BgGr, {(ul)gFact  }, NULL, 1,		0},/* (0)----------term---------> 2			*/
/* 1  */ {BgNl, {(ul)0		}, NULL, 4,		0},/* factor--------------------> (E)		*/
/* 2  */ {BgGr, {(ul)gFact  }, NULL, 1,		0},/* '*'----------factor-------> factor	*/
/* 3  */ {BgGr, {(ul)gFact  }, NULL, 1,		0},/* '/'----------factor-------> factor	*/ //Perhaps obsolete?
/* 4  */ {BgSy,	{(ul)'*'	}, NULL, 2,		5},/* NL-----------'*'----------> factor	*/
/* 5  */ {BgSy,	{(ul)'/'	}, NULL, 2,		6},/* NL-----------'/'----------> factor	*/
/* 6  */ {BgEn, {(ul)0		}, NULL, 0,		0} /* (E)-----------------------> Ende		*/
};

static tBog gStmnt[] = {
/* 0  */ {BgMo, {(ul)mcIdent}, NULL, 1,		3},
/* 1  */ {BgSy, {(ul)zERG	}, NULL, 2,		0},
/* 2  */ {BgGr, {(ul)gExpr	}, NULL,22,		0},
/* 3  */ {BgSy, {(ul)zIF	}, NULL, 4,		7},
/* 4  */ {BgGr, {(ul)gCond	}, NULL, 5,		0},
/* 5  */ {BgSy, {(ul)zTHN	}, NULL, 6,		0},
/* 6  */ {BgGr, {(ul)gStmnt	}, NULL,22,		0},
/* 7  */ {BgSy, {(ul)zWHL	}, NULL, 8,	   11},
/* 8  */ {BgGr, {(ul)gCond	}, NULL, 9,		0},
/* 9  */ {BgSy, {(ul)zDO	}, NULL,10,		0},
/* 10 */ {BgGr, {(ul)gStmnt	}, NULL,22,		0},
/* 11 */ {BgSy, {(ul)zBGN	}, NULL,12,	   15},
/* 12 */ {BgGr, {(ul)gStmnt	}, NULL,13,	    0},
/* 13 */ {BgSy, {(ul)zEND	}, NULL,22,	   14},
/* 14 */ {BgSy, {(ul)';'	}, NULL,12,		0},
/* 15 */ {BgSy, {(ul)zCLL	}, NULL,16,	   17},
/* 16 */ {BgMo, {(ul)mcIdent}, NULL,22,		0},
/* 17 */ {BgSy, {(ul)'?'	}, NULL,18,	   19},
/* 18 */ {BgMo, {(ul)mcIdent}, NULL,22,		0},
/* 19 */ {BgSy, {(ul)'!'	}, NULL,20,	   21},
/* 20 */ {BgGr, {(ul)gExpr	}, NULL,22,		0},
/* 21 */ {BgNl, {(ul)0		}, NULL,22,		0},
/* 22 */ {BgEn, {(ul)0		}, NULL, 0,		0}
};

static tBog gBlock[] = {
/* 0  */ {BgSy, {(ul)zCST	}, NULL, 1,		6},
/* 1  */ {BgMo, {(ul)mcIdent}, &NewConstBez, 2,		0},
/* 2  */ {BgSy, {(ul)'='	}, NULL, 3,		0},
/* 3  */ {BgMo, {(ul)mcNum	}, &NewConst, 4,		0},
/* 4  */ {BgSy, {(ul)','	}, NULL, 1,		5},
/* 5  */ {BgSy, {(ul)';'	}, NULL, 7,		0},
/* 6  */ {BgNl, {(ul)0		}, NULL, 7,		0},
/* 7  */ {BgSy, {(ul)zVAR	}, NULL, 8,	   11},
/* 8  */ {BgMo, {(ul)mcIdent}, &NewVar, 9,	    0},
/* 9  */ {BgSy, {(ul)','	}, NULL, 8,	   10},
/* 10 */ {BgSy, {(ul)';'	}, NULL,12,		0},
/* 11 */ {BgNl, {(ul)0		}, NULL,12,		0},
/* 12 */ {BgSy, {(ul)zPRC	}, NULL,13,	   17},
/* 13 */ {BgMo, {(ul)mcIdent}, &newProc,14,		0},
/* 14 */ {BgSy, {(ul)';'	}, NULL,15,		0},
/* 15 */ {BgGr, {(ul)gBlock	}, NULL,16,		0},
/* 16 */ {BgSy, {(ul)';'	}, NULL,12,		0},
/* 17 */ {BgNl, {(ul)0		}, NULL,18,		0},
/* 18 */ {BgGr, {(ul)gStmnt	}, NULL,19,		0},
/* 19 */ {BgEn, {(ul)0		}, NULL, 0,		0}
};

static char* graphNames[] = {
		"Program",          "Program",          "Block",        "Block",
		"Statement",        "Statement",        "Expression",   "Expression",
		"Term",             "Term",             "Factor",       "Factor",
		"Condition",        "Condition"
};

static tBog* endStates[] = {
		gProgram,           &(gProgram[2]),     gBlock,         &(gBlock[19]),
		gStmnt,             &(gStmnt[22]),      gExpr,          &(gExpr[8]),
		gTerm,              &(gTerm[6]),        gFact,          &(gFact[5]),
		gCond,              &(gCond[10])
};
