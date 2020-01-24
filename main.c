#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"
#include "NameList.h"
#include "CodeGen.h"
#include "Parser.h"

int main(int argc, char* argv[]){

	if(argc != 2){
		printf("No source file passed\n");
	}

	printf("%s", (initLex(argv[1]))?"InitLex failed.\n":"");
	
	openOFile(argv[1]);	
	printf("Opened outputfile\n");

	newProg();
	parse(gProgram);
    printf("Compiled successfully\n");
    
    closeOFile();
    printf("Written to outputfile\n");

	return 0;
}
