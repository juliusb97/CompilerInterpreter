#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "Lexer.h"
#include "NameList.h"
#include "CodeGen.h"

#define VCODE_DEFAULT_LEN 1024

FILE* pOFile;
int IdxProc;
char* pCode;
char* vCode;
int LenCode;
tProc* pCurrProc;

tLabl* LabelList;
int cmpSymb;

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
	FreeDescriptions();
	
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
    newLabel->iJmp = (long)((void*)(pCode) + 1);
    LabelList = newLabel;

    code(jnot, 0);

    return 1;
}

int st4(){
	tLabl* nxt = LabelList->nxt;
    long target = (long)(LabelList->iJmp);
    
    free(LabelList);
    LabelList = nxt;

    wr2ToCodeAtP((short)(pCode - target - 2), (char*)target);

    return 1;
}

int st5(){
	tLabl* newLabel = (tLabl*)malloc(sizeof(tLabl));
	
	newLabel->nxt = LabelList;
	newLabel->iJmp = (long)((void*)(pCode) + 1);
	LabelList = newLabel;
	
	return 1;
}

int st6(){
    tLabl* newLabel = (tLabl*)malloc(sizeof(tLabl));
    
    newLabel->nxt = LabelList;
    newLabel->iJmp = (long)((void*)(pCode) + 1);
    LabelList = newLabel;

    code(jnot, 0);

    return 1;
}

int st7(){

	tLabl* nxt = LabelList->nxt;
    long target1 = (long)(LabelList->iJmp);
    
    free(LabelList);
    LabelList = nxt;
    
    wr2ToCodeAtP((short)(pCode - target1 + 1), (char*)target1);
    
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
	
	if(tmp->Kz != KzVar){
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
