#define MAX_LEN_OF_CODE 4096
#define ENoMem -2

static FILE* pOFile;
static int IdxProc;
static char* pCode;
static char* vCode;
static int LenCode;
static tProc* pCurrPr;

void pr1();

void bl1();
void bl2();
void bl3();
void bl4();
void bl5();
void bl6();

void st1();
void st2();
void st3();
void st4();
void st5();
void st6();
void st7();
void st8();
void st9();
void st10();

void ex1();
void ex2();
void ex3();

void te1();
void te2();

void fa1();
void fa2();

void co1();
void co2();
void co3();
void co4();
void co5();
void co6();
void co7();

typedef enum TCode{
	puValVrLocl,/*	(short Displ)				Push lokale Variable	*/
	puValVrMain,/*	(short Displ)				Push Main Variable		*/
	puValVrGlob,/*	(short Displ)				Push globale Variable	*/
	puAdrVrLocl,/*	(short Displ)				Push lokale Adresse		*/
	puAdrVrMain,/*	(short Displ)				Push Main Adresse		*/
	puAdrVrGlob,/*	(short Displ, short Proc)	Push globale Adresse	*/
	puConst,/*		()							Push Konstante			*/
	storeVal,/*		()							Wert -> Adresse			*/
	getVal,/*		()							Ausgabe eines Wertes 	*/
	/* Arithmethik 														*/
	vzMinus,/*		()							Vorzeichenminus			*/
	odd,/*			()							ungerade Zahl -> 0/1	*/
	/* binaere Operatoren 												*/
	OpAdd,/*		()							Addition				*/
	OpSub,/*		()							Subtraktion				*/
	OpMult,/*		()							Multiplikation			*/
	OpDiv,/*		()							Division				*/
	cmpEQ,/*		()							==						*/
	cmpNE,/*		()							!=						*/
	cmpLT,/*		()							<						*/
	cmpGT,/*		()							>						*/
	cmpLE,/*		()							<=						*/
	cmpGE,/*		()							>=						*/
	/* Sprungbefehle 													*/
	call,/*			(short ProcNr)				Prozeduraufruf			*/
	retProc,/*		()							Rueckkehren				*/
	jmp,/*			(short RelAdresse)			SPZZ innerhalb Fkt.		*/
	jnot,/*			(short RelAdresse)			SPZZ i. Fkt., Bed Stack */
	entryProc,/*	(short lenCode, short ProcIdx, short lenVar)		*/
	EndOfCode
} tCode;

int code(tCode Code,...);
int CodeOut();
