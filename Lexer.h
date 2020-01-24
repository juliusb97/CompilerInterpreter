#define FAIL -1
#define OK 0
#define MAXKEYWORDCLASSLENGTH 3
#define NOKEYWORD -1

extern char* keyWords[];
extern char cLookupTable[];
extern int keywordClasses[][MAXKEYWORDCLASSLENGTH];

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

void        	   finalize (void);
void               readc    (void);
void               writec   (void);
int                initLex  (char* fname);
tMorph*            Lex      (void);

typedef void (*FX)(void);

extern char edgeLookupTable[][8];

extern FILE*    pIF;
extern tMorph   MorphInit;
extern tMorph   Morph;
extern int      c, state, Z, lastZ, wroteLastC;
extern int      line, col;
extern char     vBuf[];
extern char*    pBuf;
extern int      checkKeyword(tMorph* m);
