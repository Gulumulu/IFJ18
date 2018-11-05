//
// Created by parek on 11/5/18.
//
/**
 * Source file for predictive syntax analysis.
 * Implemented by Marek Varga, xvarga14
 */
#include <memory.h>
#include "predict.h"

/**
 * Function initializes stack for predictive syntax analysis.
 *
 * @param stack pointer to tStackPredictive structure is initialized stack
 */
void tStackPredictiveInit(tStackPredictive* stack) {
    stack->top = 2;
    stack->content[0] = "$";
    stack->content[1] = "E";
}

/**
 * Function disposes stack.
 *
 * @param stack pointer to tStackPredictive structure is stack that will be disposed
 */
void tStackPredictiveDispose(tStackPredictive* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        while (stack->top >= 0) {
            //free(stack->content[stack->top]);
            stack->content[stack->top] = NULL;
            stack->top--;
        }
    }
}

/**
 * Function pushes new symbol into the stack.
 *
 * @param stack pointer to tStackPredictive structure is stack onto which new symbol will be pushed
 * @param symbol pointer to char is pushed symbol into the stack
 */
void tStackPredictivePush(tStackPredictive* stack, char* symbol) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->content[stack->top] = malloc(sizeof(strlen(symbol)));
        stack->content[stack->top] = symbol;
        stack->top++;
    }
}

char* tStackPredictiveGetTop(tStackPredictive* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        return stack->content[stack->top-1];
    }
}

/**
 * Function calculates row offset.
 *
 * @param symbol pointer to char is sought row
 * @return number row by which to look into the table
 */
int rowOffset(char* symbol) {
    if (strcmp(symbol, "<start>") == 0) {
        return 0;
    } else if (strcmp(symbol, "<function>") == 0) {
        return 1;
    } else if (strcmp(symbol, "<function-head>") == 0) {
        return 2;
    } else if (strcmp(symbol, "<function-tail>") == 0) {
        return 3;
    } else if (strcmp(symbol, "<par>") == 0) {
        return 4;
    } else if (strcmp(symbol, "<next-par>") == 0) {
        return 5;
    } else if (strcmp(symbol, "<type>") == 0) {
        return 6;
    } else if (strcmp(symbol, "<st-list") == 0) {
        return 7;
    } else if (strcmp(symbol, "<stat>") == 0) {
        return 8;
    } else if (strcmp(symbol, "<eval>") == 0) {
        return 9;
    } else if (strcmp(symbol, "<assign>") == 0) {
        return 10;
    } else if (strcmp(symbol, "<call-par>") == 0) {
        return 11;
    } else {
        return 12;
    }
}

/**
 * Function calculates column offset.
 *
 * @param symbol pointer to char is sought col
 * @return column number by which to look into the table
 */
int colOffset(char* symbol) {
    if (strcmp(symbol, "def") == 0) {
        return 0;
    } else if (strcmp(symbol, "EOL") == 0) {
        return 1;
    } else if (strcmp(symbol, "end") == 0) {
        return 2;
    } else if (strcmp(symbol, ",") == 0) {
        return 3;
    } else if (strcmp(symbol, "=") == 0) {
        return 4;
    } else if (strcmp(symbol, "id") == 0) {
        return 5;
    } else if (strcmp(symbol, "int") == 0) {
        return 6;
    } else if (strcmp(symbol, "float") == 0) {
        return 7;
    } else if (strcmp(symbol, "string") == 0) {
        return 8;
    } else if (strcmp(symbol, "expr") == 0) {
        return 9;
    } else if (strcmp(symbol, "(") == 0) {
        return 10;
    } else if (strcmp(symbol, ")") == 0) {
        return 11;
    } else if (strcmp(symbol, "if") == 0) {
        return 12;
    } else if (strcmp(symbol, "then") == 0) {
        return 13;
    } else if (strcmp(symbol, "else") == 0) {
        return 14;
    } else if (strcmp(symbol, "while") == 0) {
        return 15;
    } else if (strcmp(symbol, "do") == 0) {
        return 16;
    } else if (strcmp(symbol, "function-id") == 0) {
        return 17;
    } else {
        return 18;
    }
}

int LLTable[12][18] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 19, 16, 0, 0, 0, 0, 0, 0, 24, 0, 0, 25, 0, 0},
        {0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 21},
        {0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

/**
 *
 * @param inputToken
 * @param AST
 */
void simulatePredictive(char* inputToken, tASTPointer* AST) {
    // allocated needed stacks
    tStackPredictive* predictiveStack;
    predictiveStack = malloc(sizeof(tStackPredictive));
    tStackASTPtr* stackAST;
    stackAST = malloc(sizeof(struct tStackAST));

    if (predictiveStack == NULL || stackAST == NULL) {                        // expendedStack error
        errorHandling(99);
    } else {
        tStackPredictiveInit(predictiveStack);
        tStackASTInit(stackAST);

        tStackPredictivePush(predictiveStack, "def");
        tStackPredictivePush(predictiveStack, "function");

        int tokenOffset = 0;                    // offset by which to look into the input token
        char* a;
        char* b;
        char* emptyString = "";
        //fprintf(stdout, "a is:%s\n", a);
        int rule = 0;
        int end = 0;

        do {
            b = "";
            a = malloc(sizeof(strlen(tStackPredictiveGetTop(predictiveStack))));
            a = tStackPredictiveGetTop(predictiveStack);
            char tmp2 = inputToken[tokenOffset];

            b = appendChar(b, tmp2);

            int row = rowOffset(a);
            int col = colOffset(b);
            if (row > 11 || col > 17) {
                errorHandling(99);                      // symbol doesn't occur in precedence table
            }
            rule = LLTable[row][col];
            if (rule == 0) {
                errorHandling(3);                       // no such rule
                break;
            }
            char *handle;

        } while (rule != 0 && end == 0);
    }

    // assign AST
    if (stackAST != NULL && ERROR_TYPE == 0) {
        *AST = *stackAST->body[stackAST->top];
    }

    // free allocated stacks
    tStackPredictiveDispose(predictiveStack);
    tStackASTDispose(stackAST);
}
