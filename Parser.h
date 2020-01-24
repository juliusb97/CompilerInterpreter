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

extern tBog gFact[];
extern tBog gProgram[];
extern tBog gCond[];
extern tBog gBlock[];
extern tBog gTerm[];
extern tBog gStmnt[];
extern tBog gExpr[];
extern char* graphNames[];
extern tBog* endStates[];
extern int depth;

int parse(tBog* pGraph);
