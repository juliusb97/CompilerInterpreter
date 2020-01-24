#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Lexer.h"

char* keyWords[] =
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

char cLookupTable[] =
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

int keywordClasses[][MAXKEYWORDCLASSLENGTH] =
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

char edgeLookupTable[][8] =
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

int wroteLastC = 1;
FILE* pIF;
tMorph MorphInit;
tMorph Morph;
int c, state, Z, lastZ, line, col;
char* pBuf;
char vBuf[1024+1];

int initLex(char* fname){
	char vName[128+1];

	Z=0;

	strcpy(vName, fname);
	if(strstr(vName, ".pl0")==NULL) strcat(vName, ".pl0");

	pIF = fopen(vName, "r+t");
	if(pIF!=NULL) {return OK;}
	return FAIL;
}

tMorph* Lex(void){
	int cClass;
	Z = state = 0;
	Morph = MorphInit;
	Morph.PosLine = line;
	Morph.PosCol = col;
	Morph.MLen = 0;
	pBuf = vBuf;

	//Check for EOF (temporary)
	if(c == EOF){
		Morph.Val.Symb = mcEOF;
		return &Morph;
	}

	do{
		if(wroteLastC)
			readc();
		cClass = cLookupTable[c];   //Determine character class

		//Handle whitespace
		if(cClass == 0){
			break;
		}

		state = edgeLookupTable[state][cClass]; //Determine state we're currently in

		if(state == 0 && wroteLastC != 0) {
			wroteLastC = 0;
			break;
		} else{
			writec();
			wroteLastC = 1;
		}

		finalize();

	}while(state != 0);

	int keywordIdx = 0;
	if(Morph.MC == mcIdent)
		checkKeyword(&Morph);

	if(Morph.MC == mcSymb && Morph.MLen == 1){
		Morph.Val.Symb = *(Morph.Val.pStr);
	}

	if(Morph.MC == mcEmpty)
		return Lex();
	else return &Morph;
}

void finalize(void){
	switch(state){
		case 0: //Read special character
			Morph.MC        = mcSymb;
			Morph.Val.pStr  = vBuf;
			break;
		case 1: //Read letter
			Morph.MC        = mcIdent;
			Morph.Val.pStr  = vBuf;
			break;
		case 2: //Read numeral
			Morph.MC        = mcNum;
			Morph.Val.Num   = atol(vBuf);
			break;
		case 3: //Read ':'
			Morph.MC        = mcSymb;
			Morph.Val.pStr  = vBuf;
			break;
		case 4: //Read '<'
			Morph.MC        = mcSymb;
			Morph.Val.pStr  = vBuf;
			break;
		case 5: //Read '>'
			Morph.MC        = mcSymb;
			Morph.Val.pStr  = vBuf;
			break;
		case 6: //Read ":="
			Morph.MC        = mcSymb;
			Morph.Val.Symb  = zERG;
			break;
		case 7: //Read "<="
			Morph.MC        = mcSymb;
			Morph.Val.Symb  = zLE;
			break;
		case 8: //Read ">="
			Morph.MC        = mcSymb;
			Morph.Val.Symb  = zGE;
			break;
	}

	Morph.MLen++;
	return;
}

void readc  (void){
	c=fgetc(pIF);

	if (c == '\n') line++, col=0;
	else col++;

	return;
}

void writec(void){
	//Write to buffer
	if(isalpha(c))
		*pBuf = (char)toupper(c);
	else
		*pBuf = (char)c;

	*(++pBuf) = 0; //Move pointer forward

	return;
}
