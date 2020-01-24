#define FAIL -1
#define OK 0
#define NOTFOUND 0

typedef enum KZs{
    KzProc, KzConst, KzVar
} Kzs;

typedef int tKz;
typedef struct TBez tBez;

typedef struct TList{
    void*   content;
    struct tLIST*   nxt;
    struct tLIST*   prv;
} tList;

//Name list entry
typedef struct TBez{
    tKz     Kz;
    tBez*   nxt;
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
    tBez*   pLBez;
    int     SpzzVar;
} tProc;

tBez*   createBez(char*pBez);
tConst* createConst(long Val);
tConst* searchConst(long Val);
int     createVar(void);
tProc*  createProc(tProc*pParent);
tBez*   searchBez(tProc*pProc,char* pBez);
tBez*   searchBezGlobal(char* pBez);
tBez*	SearchGlobal(char* pBez);
tBez*	SearchByVal(int val);

int		NewVar();
int		NewConstBez();
int		NewConst();
int     newProc();
void	newProg();
int		FreeDescriptions();

extern	tProc*	 root;
extern  tProc*   procList;
extern  int procCounter;
extern  int ConstCounter;
extern  int* ConstBlock;
