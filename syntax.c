//
// Created by parek on 11/6/18.
//
/**
 * Source file for syntax driven compilation.
 * Implemented by Gabriel Quirschfeld, xquirs00
 *                Marek Varga, xvarga14
 */
#include <memory.h>
#include <mem.h>
#include "syntax.h"

void doMagic() {

    // call lexical anal
    //...
    // call top down anal
    // if lexical analysis passed ERROR_TYPE should still be 0
    if (ERROR_TYPE == 0) {
        tASTPointer* AST = malloc(sizeof(struct tAST));
        if (AST == NULL) {
            errorHandling(99);
            //return ERROR_TYPE;
        }
        tASTInit(AST);
        char* tmp[14] = {"def", "function-id", "(", "id", ")", "EOL", "id", "=", "id", "*", "2", "EOL", "end", "$"};
        tStackPredictive* predictiveStack;
        predictiveStack = malloc(sizeof(tStackPredictive));
        tStackPredictiveInit(predictiveStack);
        int i = 0;
        do {
            simulatePredictive(tmp[i], AST, predictiveStack);
            if (strcmp(predictiveStack->content[predictiveStack->top-1], "<expr>") == 0) {
                tStackPredictive* tmpStack = malloc(sizeof(tStackPredictive));
                tStackPredictiveInit(tmpStack);
                for (int j = i; strcmp(tmp[i], "EOL") != 0; j++, i++) {
                    tStackPredictivePush(tmpStack, tmp[j]);
                }
                for (int k = 2; k < tmpStack->top; k++) {
                    simulatePrecedence(tmpStack->content[k], AST);
                }
                tStackPredictiveDispose(tmpStack);
            }
            i++;
        } while (strcmp(tmp[i], "$") != 0);
        //simulatePredictive("i+i*i", AST);                     // predictive SA
        //simulatePrecedence("i+i*i$", AST);                      // tokens "i+i*i$" will be rewritten to AST
        tASTDispose(AST);
    }
}