#include <stdio.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "sematnic.h"
#include "prectable.h"
#include "predict.h"

int main() {
    printf("Hello, World!\n");
    FILE *file = fopen("D:\\Coding\\IFJ18\\test.txt", "r");

    int count = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    rewind(file);
    for (int i = 0; i <= count; i++) {
        token_generate(file);
    }

   // token_generate(file);

    fclose(file);
/*
    // if lexical analysis passed ERROR_TYPE should still be 0
    if (ERROR_TYPE == 0) {
        tASTPointer* AST = malloc(sizeof(struct tAST));
        if (AST == NULL) {
            errorHandling(99);
            //return ERROR_TYPE;
        }
        tASTInit(AST);
        //simulatePredictive("i+i*i", AST);                     // predictive SA
        simulatePrecedence("i+i*i$", AST);                      // tokens "i+i*i$" will be rewritten to AST
        tASTDispose(AST);
    }
*/
    return ERROR_TYPE;
}