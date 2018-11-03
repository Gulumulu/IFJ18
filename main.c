#include <stdio.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "sematnic.h"
#include "prectable.h"

int main() {
    printf("Hello, World!\n");

    // if lexical analysis passed ERROR_TYPE should still be 0
    if (ERROR_TYPE == 0) {
        tASTPointer* AST = malloc(sizeof(struct tAST));
        if (AST == NULL) {
            errorHandling(99);
            //return ERROR_TYPE;
        }
        tASTInit(AST);
        simulatePrecedence("i+i*i$", AST);                      // tokens "i+i*i$" will be rewritten to AST
        // todo: free space
    }

    return ERROR_TYPE;
}