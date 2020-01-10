#define MAX_LEN_OF_CODE 4096
#define ENoMem -2

static FILE* pOFile;
static int IdxProc;
static char* pCode;
static char* vCode;
static int LenCode;
static tProc* pCurrPr; 

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
	/* Arithmethik */
	vzMinus,/*		()							Vorzeichenminus			*/
	odd,/*			()							ungerade Zahl -> 0/1	*/
	/* binaere Operatoren */
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
	/* Sprungbefehle */
	call,/*			(short ProcNr)				Prozeduraufruf			*/
	retProc,/*		()							Rueckkehren				*/
	jmp,/*			(short RelAdresse)			SPZZ innerhalb Fkt.		*/
	jnot,/*			(short RelAdresse)			SPZZ i. Fkt., Bed Stack */
	entryProc,/*	(short lenCode, short ProcIdx, short lenVar)		*/
	EndOfCode
} tCode;
