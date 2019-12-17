typedef enum KZs{
    KzProc, KzConst, KzVar
} Kzs;

typedef int tKz;

typedef struct TList{
    void*   content;
    struct tLIST*   nxt;
    struct tLIST*   prv;
} tList;

//Name list entry
typedef struct{
    tKz     Kz;
    short   IdxProc;
    void*   pObj;
    int     Len;
    char*   pName;
} tBez;


//Variable description
typedef struct tVAR{
    tKz     Kz;
    int     Dspl;
} tVar;

//Constant description
typedef struct tCONST{
    tKz     Kz;
    long    Val;
    int     Idx;
} tConst;

//Procedure description
typedef struct tPROC{
    tKz     Kz;
    short   IdxProc;
    struct  tPROC* pParent;
    tList*  pLBez;
    int     SpzzVar;
} tProc;

tBez*   createBez(char*pBez);
tConst* createConst(long Val);
tConst* searchConst(long Val);
int     createVar(void);
tProc*  createProc(tProc*pParent);
tBez*   searchBez(tProc*pProc,char* pBez);
tBez*   searchBezGlobal(char* pBez);
static  tList*   ProcList;
static  int procCounter = 0;
extern int ConstCounter;
static int* ConstBlock;
