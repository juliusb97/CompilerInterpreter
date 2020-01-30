#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lexer.h"
#include "NameList.h"

tProc* root;
tProc* procList;
int procCounter = 0;
int ConstCounter = 0;
int* ConstBlock = NULL;

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
    tBez* tmp2  = tmp;
	
	while(tmp != NULL){
		if(strcmp(tmp->pName, name) == 0) return tmp;
        if(tmp->Kz == KzProc){
            tProc* procListCopy = procList;
            procList = tmp->pObj;
            tmp2 = tmp;
            tmp2 = Search(name);
            procList = procListCopy;
            if(tmp2 != NOTFOUND) return tmp2;
        }
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
