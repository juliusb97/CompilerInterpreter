#define MAX_LEN_OF_CODE 81
#define ENoMem -2

extern FILE* pOFile;
extern int IdxProc;
extern char* pCode;
extern char* vCode;
extern int LenCode;
extern tProc* pCurrPr;

typedef struct tLABL{
    tKz Kz;
    long iJmp;
    struct tLABL* nxt;
}tLabl;

extern tLabl* LabelList;

int pr1();

int bl1();
int bl2();
int bl3();
int bl4();
int bl5();
int bl6();

int st1();
int st2();
int st3();
int st4();
int st5();
int st6();
int st7();
int st8();
int st9();
int st10();

int ex1();
int ex2();
int ex3();

int te1();
int te2();

int fa1();
int fa2();

int co1();
int co2();
int co3();
int co4();
int co5();
int co6();
int co7();
int co8();

typedef enum TCode{
	puValVrLocl,/*	(short Displ)				Push lokale Variable	*/ //0x00
	puValVrMain,/*	(short Displ)				Push Main Variable		*/ //0x01
	puValVrGlob,/*	(short Displ)				Push globale Variable	*/ //0x02
	puAdrVrLocl,/*	(short Displ)				Push lokale Adresse		*/ //0x03
	puAdrVrMain,/*	(short Displ)				Push Main Adresse		*/ //0x04
	puAdrVrGlob,/*	(short Displ, short Proc)	Push globale Adresse	*/ //0x05
	puConst,/*		(short Index)				Push Konstante			*/ //0x06
	storeVal,/*		()							Wert -> Adresse			*/ //0x07
	putVal,/*		()							Ausgabe Wert ->stdout	*/ //0x08
	getVal,/*		()							Eingabe eines Wertes 	*/ //0x09
	/* Arithmethik 														*/ 
	vzMinus,/*		()							Vorzeichenminus			*/ //0x0A
	odd,/*			()							ungerade Zahl -> 0/1	*/ //0x0B
	/* binaere Operatoren 												*/
	OpAdd,/*		()							Addition				*/ //0x0C
	OpSub,/*		()							Subtraktion				*/ //0x0D
	OpMult,/*		()							Multiplikation			*/ //0x0E
	OpDiv,/*		()							Division				*/ //0x0F
	cmpEQ,/*		()							==						*/ //0x10
	cmpNE,/*		()							!=						*/ //0x11
	cmpLT,/*		()							<						*/ //0x12
	cmpGT,/*		()							>						*/ //0x13
	cmpLE,/*		()							<=						*/ //0x14
	cmpGE,/*		()							>=						*/ //0x15
	/* Sprungbefehle 													*/
	call,/*			(short ProcNr)				Prozeduraufruf			*/ //0x16
	retProc,/*		()							Rueckkehren				*/ //0x17
	jmp,/*			(short RelAdresse)			SPZZ innerhalb Fkt.		*/ //0x18
	jnot,/*			(short RelAdresse)			SPZZ i. Fkt., Bed Stack */ //0x19
	entryProc,/*	(short lenCode, short ProcIdx, short lenVar)		*/ //0x1A
	putStrg,/*		(char[])					nicht implementiert		*/ //0x1B
	EndOfCode															   //0x1C
} tCode;

void wr2ToCode(short x);
void wr2ToCodeAtP(short x, char* pD);
int code(tCode Code,...);
int CodeOut();
int openOFile(char* arg);
int closeOFile();
extern int cmpSymb;
