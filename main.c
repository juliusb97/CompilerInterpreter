#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "Lexer.h"
#include "NameList.h"
#include "CodeGen.h"
#include "Parser.h"

#define NOTFOUND 0
#define VCODE_DEFAULT_LEN 1024

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

//	if(Morph.Val.Symb == mcEmpty)
	if(Morph.MC == mcEmpty)
		return Lex();
	else return &Morph;
}

static void finalize(void){
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

int checkKeyword(tMorph* m){
	int c, idx;
	for(c = 0; c < MAXKEYWORDCLASSLENGTH; c++){
		if((idx = keywordClasses[m->MLen][c]) >= 0 && !strcmp(m->Val.pStr, keyWords[idx])){
			m->MC = mcSymb;
			m->Val.Symb = (tZS)idx+131;
			return idx;
		}
	}
	return NOKEYWORD;
}

int depth = 0;

void LookupGraph(tBog* pBog){
	int i, j;

	for(i = 0; i < 13; i+=2){
		if(pBog >= endStates[i] && pBog <= endStates[i+1]){
            printf("==================================================\n");
			printf("Depth: %d Graph: %s.\n", depth, graphNames[i]);
            printf("==================================================\n\n");
			return;
		}
	}
}

void PrintMorph(tMorph* m){
    int keywordIdx = 0;
    
    switch(m->MC){
		case mcSymb:
			printf("Morphem %s: %s\n", "mcSymb", vBuf);
			break;
		case mcNum:
			printf("Morphem %s: %ld\n", "mcNum", m->Val.Num);
			break;
		case mcIdent:
			printf("Morphem %s: %s\n", "mcIdent", m->Val.pStr);
			if((keywordIdx = checkKeyword(m)) != -1)
				printf("Keyword %s\n", keyWords[keywordIdx]);
			break;
		case mcEOF:
        case mcEmpty:
            break;
    }
}

int parse(tBog* pGraph)
{
	depth++;
	tBog* pBog=pGraph;
	int succ=0;

	#ifdef DEBUG_PARSE
	LookupGraph(pBog);
	#endif

	if (Morph.MC==mcEmpty)  Lex();
	while(1)
	{
		switch(pBog->BgD) {
			case BgNl:succ=1;                            break;
			case BgSy:succ=(Morph.Val.Symb==pBog->BgX.S);break;
			case BgMo:succ=(Morph.MC==(tMC)pBog->BgX.M); break;
			case BgGr:succ=parse((tBog*)(pBog->BgX.G));           break;
			case BgEn:depth--; return 1;  /* Ende erreichet - Erfolg */
		}

		
		if (succ && pBog->fx!=NULL) succ=pBog->fx();
		if (!succ)/* Alternativbogen probieren */
			if (pBog->iAlt != 0)
				pBog=pGraph+pBog->iAlt;
			else return FAIL;
		else /* Morphem formal akzeptiert (eaten) */
		{
			#ifdef DEBUG_PARSE
			LookupGraph(pBog);
			PrintMorph(&Morph);
			#endif
			if (pBog->BgD & BgSy || pBog->BgD & BgMo) Lex();
			pBog=pGraph+pBog->iNext;
		}
	}
	depth--;
}

tVar* CreateVar(void){
    tVar* newVar 		= malloc(sizeof(tVar));
    
    newVar->Kz 			= KzVar;
    newVar->Dspl 		= procList->SpzzVar;
    
	procList->SpzzVar	+=4;
    
    return newVar;
}

tConst* createConst(long Val){
    tConst* newConst = (tConst*)malloc(sizeof(tConst));
    
    newConst->Kz = KzConst;
    newConst->Val = Val;
    newConst->Idx = ConstCounter++;
    
    //Not sure if correct
    void* ret = realloc(ConstBlock, sizeof(int) * ConstCounter);
    if(ret == NULL) printf("Realloc in const-Creation failed\n");
    ConstBlock[ConstCounter-1] = Val;
    
    //Frage: siehe Algorithmus in Folie
	
	return newConst;
}

tBez* createBez(char* pBez){
    
    tBez* newBez = malloc(sizeof(tBez));
    
	newBez->nxt 	= procList->pLBez;
    newBez->IdxProc = procCounter-1;
    newBez->Len 	= strlen(pBez);
    newBez->pName 	= (char*)malloc(newBez->Len + 1);
    strcpy(newBez->pName, pBez);
    newBez->pObj	= NULL;
    
    procList->pLBez = newBez;
    
    return newBez;
}

tProc* createProc(tProc* pParent){
    tProc* newProc = (tProc*)malloc(sizeof(tProc));
    
    ((tProc*)(newProc))->Kz 		= KzProc;
    ((tProc*)(newProc))->IdxProc 	= procCounter++;
    ((tProc*)(newProc))->pParent 	= pParent;
    ((tProc*)(newProc))->pLBez 		= NULL;
    ((tProc*)(newProc))->SpzzVar 	= 0;
    
    return newProc;
}

tBez* Search(char* name){

	tBez* start = procList->pLBez;
	tBez* tmp 	= start;
	
	while(tmp != NULL){
		if(strcmp(tmp->pName, name) == 0) return tmp;
		tmp = tmp->nxt;
	}
	
	return NOTFOUND;
}

tBez* SearchGlobal(char* name){
	tBez* tmp = NOTFOUND;
	tProc* procListCopy = procList;
    int isRoot = 0;	

    do{
		tmp = Search(name);
        if(isRoot) break;		
        procList = procList->pParent;
        isRoot = (procList == root);
	}while(tmp == NOTFOUND);
	
	procList = procListCopy;
	
	return tmp;
}

tBez* SearchByVal(int val){
   	tBez* start = procList->pLBez;
	tBez* tmp 	= start;
	
	while(tmp != NULL){
		if(((tConst*)(tmp->pObj))->Val == val && tmp->Kz == KzConst) return tmp;
		tmp = tmp->nxt;
	}
	
	return NOTFOUND;
}

int NewVar(){
	#ifdef DEBUG_NAMES
	printf("New Variable\n");
	#endif
	
	if(Search(Morph.Val.pStr) != NOTFOUND){
		printf("Variable identifier '%s' already exists (Error in line %d)\n", Morph.Val.pStr, Morph.PosLine + 1);
		exit(FAIL);
	}
	
	tBez* newBezeichner = createBez(Morph.Val.pStr);
	newBezeichner->Kz = KzVar;
	
	#ifdef DEBUG_NAMES
	printf("Created Bezeichner\n");
	#endif
	
	procList->pLBez->pObj = CreateVar();
	
	#ifdef DEBUG_NAMES
	printf("Created new Variable %s with relative address %d\n\n\n", procList->pLBez->pName, ((tVar*)(procList->pLBez->pObj))->Dspl);
	#endif
	
	return 1;
}

int NewConstBez(char* name){
	#ifdef DEBUG_NAMES
	printf("New Constant\n");
	#endif
	
	if(name == NULL && Search(Morph.Val.pStr) != NOTFOUND){
		printf("Constant identifier '%s' already exists (Error in line %d)\n", Morph.Val.pStr, Morph.PosLine + 1);
		exit(FAIL);
	}
	
    tBez* newBezeichner = NULL;
    if(name == NULL)
    	newBezeichner = createBez(Morph.Val.pStr);
    else
        newBezeichner = createBez(name); 
	newBezeichner->Kz = KzConst;
	
	#ifdef DEBUG_NAMES
	printf("Created Bezeichner\n");
	#endif
	
	return 1;
}

int NewConst(){
	procList->pLBez->pObj = createConst(Morph.Val.Num);
	
	#ifdef DEBUG_NAMES
	printf("Created new Constant %s with value: %ld\n\n\n", procList->pLBez->pName, ((tConst*)(procList->pLBez->pObj))->Val);
	#endif
	
	return 1;
}

int newProc(){
    #ifdef DEBUG_NAMES
    printf("New Procedure\n");
    #endif
    
    //procList = procList->pParent;
	if(Search(Morph.Val.pStr) != NOTFOUND){
		printf("Procedure identifier '%s' already exists (Error in line %d)\n", Morph.Val.pStr, Morph.PosLine + 1);
		exit(FAIL);
	}
    
    tBez* newBezeichner = createBez(Morph.Val.pStr);
    newBezeichner->Kz = KzProc;
    
    #ifdef DEBUG_NAMES
    printf("Created Bezeichner\n");
    #endif
    
    tProc* newProcedure = createProc(root);

	newBezeichner->pObj = newProcedure;
	procList = newProcedure;
    
    #ifdef DEBUG_NAMES
    printf("Created Procedure-Block with no. %d and name %s\n\n\n", procList->IdxProc, newBezeichner->pName);
    #endif
    
    //On error: return false
    return 1;
}

void newProg(){

	#ifdef DEBUG_NAMES
	printf("New Program\n");
	#endif
	
	tProc* newProcedure = root = createProc(NULL);
	procList = newProcedure;
	
	//char* name = "Program";
	//tBez* newBezeichner = createBez(name);
	
	//printf("Created Bezeichner\n");
	
	#ifdef DEBUG_NAMES
	printf("Created Procedure-Block with no. %d\n\n\n", newProcedure->IdxProc);
	#endif
}

int FreeDescriptions(){
	tBez* start = procList->pLBez;
	tBez* tmp = start;
	tBez* tmpOld = start;
	
	while(tmp != NULL){
		tmp = tmp->nxt;
		free(tmpOld->pObj);
		free(tmpOld);
		tmpOld = tmp;
	}
	
	tProc* oldProc = procList;
	procList = procList->pParent;
	
	//free(oldProc);		//Muss noch erhalten bleiben?
	
	return 1;
}

void wr2ToCode(short x)
{
  *pCode++=(unsigned char)(x & 0xff);
  *pCode++=(unsigned char)(x >> 8);
}
void wr2ToCodeAtP(short x,char*pD)
{
  *pD    =(unsigned char)(x & 0xff);
  *(pD+1)=(unsigned char)(x >> 8);
}

int pr1(){
	fseek(pOFile, 0, SEEK_END);
	fwrite(ConstBlock, sizeof(int), ConstCounter, pOFile);
	
	#ifdef DEBUG_CODEGEN
	printf("Wrote constants block\n");
	#endif
	
	return 1;
}

int bl1(){
	NewConstBez(NULL);
	
	#ifdef DEBUG_CODEGEN
	printf("Created new constant Bezeichner\n");
	#endif
	
	return 1;
}

int bl2(){
	NewConst();
	
	#ifdef DEBUG_CODEGEN
	printf("Created new constant description\n");
	#endif
	
	return 1;
}

int bl3(){
	NewVar();
	
	#ifdef DEBUG_CODEGEN
	printf("Created new variable description\n");
	#endif
	
	return 1;
}

int bl4(){
	newProc();
	
	#ifdef DEBUG_CODEGEN
	printf("Created new procedure description\n");
	#endif
	
	return 1;
}

int bl5(){
	code(retProc);

	CodeOut();
	FreeDescriptions(); //TODO: reevaluate
	
	#ifdef DEBUG_CODEGEN
	printf("Wrote procedure\n");
	#endif
	
	return 1;
}

int bl6(){
	vCode = (char*)malloc(VCODE_DEFAULT_LEN);
	
	if(vCode == NULL){
		printf("Code buffer memory could not be allocated.\n");
		exit(ENoMem);
	}
	
	pCode = vCode;
	code(entryProc, 0, procList->IdxProc, procList->SpzzVar);
	
	#ifdef DEBUG_CODEGEN
	printf("Started on procedure\n");
	#endif
	
	return 1;
}

int st1(){
	tBez* tmp = SearchGlobal(Morph.Val.pStr);
	
	if(tmp == NOTFOUND){
		printf("Identifier %s not found (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}
	
	if(tmp->Kz != KzVar){
		printf("Identifier %s is a procedure or constant, needed: variable (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}

	if(procList == root)
		code(puAdrVrMain, ((tVar*)(tmp->pObj))->Dspl);
    else if(procList->IdxProc == tmp->IdxProc)
        code(puAdrVrLocl, ((tVar*)(tmp->pObj))->Dspl);
	else
		code(puAdrVrGlob, ((tVar*)(tmp->pObj))->Dspl, tmp->IdxProc);
	
	return 1;
}

int st2(){
	code(storeVal);
	return 1;
}

int st3(){
    tLabl* newLabel = (tLabl*)malloc(sizeof(tLabl));
    
    newLabel->nxt = LabelList;
    newLabel->iJmp = (void*)(pCode) + 1;
    LabelList = newLabel;

    code(jnot, 0);

    return 1;
}

int st4(){
	tLabl* nxt = LabelList->nxt;
    long target = (long)(LabelList->iJmp);
    
    free(LabelList);
    LabelList = nxt;

    wr2ToCodeAtP(pCode - target - 2, target);

    return 1;
}

int st5(){
	tLabl* newLabel = (tLabl*)malloc(sizeof(tLabl));
	
	newLabel->nxt = LabelList;
	newLabel->iJmp = (void*)(pCode) + 1;
	LabelList = newLabel;
	
	return 1;
}

int st6(){
    tLabl* newLabel = (tLabl*)malloc(sizeof(tLabl));
    
    newLabel->nxt = LabelList;
    newLabel->iJmp = (void*)(pCode) + 1;
    LabelList = newLabel;

    code(jnot, 0);

    return 1;
}

int st7(){

	tLabl* nxt = LabelList->nxt;
    long target1 = (long)(LabelList->iJmp);
    
    free(LabelList);
    LabelList = nxt;
    
    wr2ToCodeAtP(pCode - target1 + 1, target1);
    
    nxt = LabelList->nxt;
    long target2 = (long)(LabelList->iJmp);
    
    free(LabelList);
    LabelList = nxt;
    
    code(jmp, ~((long)pCode - target2) - 3);

	return 1;
}

int st8(){
	tBez* tmp = SearchGlobal(Morph.Val.pStr);
	
	if(tmp == NOTFOUND){
		printf("Identifier %s not found (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}
	
	if(tmp->Kz != KzProc){
		printf("Identifier %s is a variable or constant, needed: procedure (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}
	
	code(call, ((tProc*)(tmp->pObj))->IdxProc);
	
	return 1;
}

int st9(){ // TODO: reevaluate
	
	tBez* tmp = SearchGlobal(Morph.Val.pStr);
	
	if(tmp == NOTFOUND){
		printf("Identifier %s not found (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}
	
	if(tmp->Kz != KzProc){
		printf("Identifier %s is a procedure or constant, needed: variable (line: %d)\n", Morph.Val.pStr, Morph.PosLine);
		exit(FAIL);
	}
	
	if(procList == root)
		code(puAdrVrMain, ((tVar*)(tmp->pObj))->Dspl);
	else if(tmp->IdxProc == procList->IdxProc)
        code(puAdrVrLocl, ((tVar*)(tmp->pObj))->Dspl);
    else
		code(puAdrVrGlob, ((tVar*)(tmp->pObj))->Dspl, tmp->IdxProc);
	
	code(getVal);
	
	return 1;
}

int st10(){
	code(putVal);
	
	return 1;
}

int ex1(){
	code(vzMinus);
	return 1;
}

int ex2(){
	code(OpAdd);
	return 1;
}

int ex3(){
	code(OpSub);
	return 1;
}

int te1(){
	code(OpMult);
	return 1;
}

int te2(){
	code(OpDiv);
	return 1;
}

int fa1(){
    printf("here\n");

    tBez* tmp = SearchByVal(Morph.Val.Num);

    char name[1024];
    sprintf(name, "c%ld", Morph.Val.Num);

    if(tmp == NOTFOUND){
        NewConstBez(name);
        NewConst();
        tmp = procList->pLBez;
    }

    code(puConst, ((tConst*)(tmp->pObj))->Idx);
	return 1;
}

int fa2(){
	tBez* bez = SearchGlobal(Morph.Val.pStr);
	
	if(bez == NOTFOUND){
		printf("Identifier %s not found (line: %d)\n", Morph.Val.pStr, Morph.PosLine + 1);
		exit(FAIL);
	} else if(bez->Kz == KzProc){
		printf("Identifier %s is a procedure, needed: variable or constant (line: %d)\n", Morph.Val.pStr, Morph.PosLine + 1);
        exit(FAIL);
	}
	
	if(bez->Kz == KzConst){
		code(puConst, ((tConst*)(bez->pObj))->Idx);
	} else{
		if(procList == root){
			code(puValVrMain, ((tVar*)(bez->pObj))->Dspl);
		} else if(procList->IdxProc == bez->IdxProc){
            code(puValVrLocl, ((tVar*)(bez->pObj))->Dspl);
        } else{
			code(puValVrGlob, ((tVar*)(bez->pObj))->Dspl, bez->IdxProc);
		}
	}
	
	return 1;
}

int co1(){
	code(odd);
	return 1;
}

int co2(){
	cmpSymb = '=';

	return 1;
}

int co3(){
	cmpSymb = '#';
	
	return 1;
}

int co4(){
	cmpSymb = '<';
	
	return 1;
}

int co5(){
	cmpSymb = zLE;
	
	return 1;
}

int co6(){
	cmpSymb = '>';
	
	return 1;
}

int co7(){
	cmpSymb = zGE;
}

int co8(){
	switch(cmpSymb){
		case '=': code(cmpEQ); break;
		case '#': code(cmpNE); break;
		case '<': code(cmpLT); break;
		case zLE: code(cmpLE); break;
		case '>': code(cmpGT); break;
		case zGE: code(cmpGE); break;
	}
	
	return 1;
}

int code(tCode Code,...)
{
  va_list ap;
  short sarg;

  if (pCode-vCode+MAX_LEN_OF_CODE>=LenCode)
  {
    char* xCode=realloc(vCode,(LenCode+=1024));
	 if (xCode==NULL) exit(ENoMem);
    pCode=xCode+(pCode-vCode);
    vCode=xCode;
  }
  
  *pCode++=(char)Code; //TODO: Mistake?
  va_start(ap,Code);
  
  switch (Code)
  {
    /* Befehle mit 3 Parametern */
    case entryProc:
	       sarg=va_arg(ap,int);
	       wr2ToCode(sarg);
    /* Befehle mit 2 Parametern */
    case puValVrGlob:
    case puAdrVrGlob:
	       sarg=va_arg(ap,int);
	       wr2ToCode(sarg);
    /* Befehle mit 1 Parameter */
    case puValVrMain:
    case puAdrVrMain:
    case puValVrLocl:
    case puAdrVrLocl:
    case puConst:
    case jmp :
    case jnot:
    case call:
 	       sarg=va_arg(ap,int); /* Prozedurnummer               */
	       wr2ToCode(sarg);
	       break;
     /* keine Parameter */
     default     : break;
 }
  va_end  (ap);
  return OK;
}

int CodeOut(void)
{
  unsigned short Len=(unsigned short)(pCode-vCode);
  wr2ToCodeAtP((short)Len,vCode+1);
  wr2ToCodeAtP((short)procList->SpzzVar,vCode+5);
  if (Len==fwrite(vCode,sizeof(char),Len,pOFile)) return OK;
  else                                            return FAIL;
}


int openOFile(char* arg)
{
  long  i=0L;
  char vName[128+1];

  strcpy(vName,arg);
  if (strstr(vName,".pl0")==NULL) strcat(vName,".cl0");
  else *(strchr(vName,'.')+1)='c';

  if ((pOFile=fopen(vName,"wb"))!=NULL) 
  {
    fwrite(&i,sizeof(int32_t),1,pOFile);
    return OK;
  }
  else                                 return FAIL;
  
}

int closeOFile(void)
{
  char vBuf2[2];
  fseek(pOFile,0,SEEK_SET);
  wr2ToCodeAtP(procCounter,vBuf2);
  if (fwrite(vBuf2,2,1,pOFile)==2) return OK;
  else                             return FAIL;
}

int main(int argc, char* argv[]){

	if(argc != 2){
		printf("No source file passed");
	}

	tMorph* tmp;
	int debugCounter = 0, keywordIdx = 0;
	ConstBlock = (int*)malloc(sizeof(int));
	ConstCounter = 0;

	printf("%s\n", (initLex(argv[1]))?"InitLex failed.\n":"");
	
	openOFile(argv[1]);	
	printf("Opened outputfile\n");

	newProg();
	parse(gProgram);
    printf("Compiled successfully\n");
    
    closeOFile();
    printf("Done.\n");

	return 0;
}
