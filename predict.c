//
// Created by parek on 11/5/18.
//
/**
 * Source file for predictive syntax analysis.
 * Implemented by Marek Varga, xvarga14
 */
#include <string.h>
#include "predict.h"

/**
 * Function initializes stack for predictive syntax analysis.
 *
 * @param stack pointer to tStackPredictive structure is initialized stack
 */
void tStackPredictiveInit(tStackPredictive* stack) {
    stack->top = 2;
    stack->content[0] = "$";
    stack->content[1] = "<start>";
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
        // todo: malloc overrides globalToken.content
        //stack->content[stack->top] = malloc(sizeof(strlen(symbol)));
        //checkMalloc(stack->content[stack->top]);
        stack->content[stack->top] = symbol;
        stack->top++;
    }
}

/**
 * Function gets top symbol from stack.
 *
 * @param stack pointer to tStackPredictive structure is stack from which symbol is returned
 * @return pointer to char is top-most symbol in stack
 */
char* tStackPredictiveGetTop(tStackPredictive* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        return stack->content[stack->top-1];
    }
}

/**
 * Function pop symbol from stack.
 *
 * @param stack pointer to tStackPredictive structure is stack from which symbol is popped
 */
void tStackPredictivePop(tStackPredictive* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->content[stack->top-1] = NULL;
        stack->top--;
    }
}

char* rightSides[20][10] = {
        /*1. <start> -> */    {"<function>", "<st-list>", "", "", "", "", "", "", "", ""},
        /*2. <function> -> */ {"def", "<function-head>", "<st-list>", "<function-tail>", "<function>", "", "", "", "", ""},
        /*3. <function> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*4. <function-head> -> */ {"function-id", "(", "<par>", ")", "EOL", "", "", "", "", ""},
        /*5. <function-tail> -> */ {"end", "EOL", "", "", "", "", "", "", "", ""},
        /*6. <par> ->*/ {"id", "<next-par>", "", "", "", "", "", "", "", ""},
        /*7. <par> ->*/ {"", "", "", "", "", "", "", "", "", ""},
        /*8. <next-par> -> */ {",", "<par>", "", "", "", "", "", "", "", ""},
        /*9. <next-par> -> */ {"", "", "", "", "", "", "", "", "", ""},
        // {"integer", "", "", "", "", "", "", "", "", ""},
        //{"float", "", "", "", "", "", "", "", "", ""},
        //{"string", "", "", "", "", "", "", "", "", ""},
        /*10. <st-list> -> */ {"<stat>", "<st-list>", "", "", "", "", "", "", "", ""},
        /*11. <st-list> -> */ {"end", "EOL", "", "", "", "", "", "", "", ""},
        /*12. <st-list> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*13. <st-list> -> */ {"EOL", "", "", "", "", "", "", "", "", ""},
        /*14. <stat> -> */ {"id", "<eval>", "", "", "", "", "", "", "", ""},
        /*15. <eval> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*16. <eval> -> */ {"=", "<assign>", "", "", "", "", "", "", "", ""},
        //{"id", "=", "<assign>", "", "", "", "", "", "", ""},
        /*17. <assign> -> */ {"<expr>", "", "", "", "", "", "", "", "", ""},
        /*18. <assign> -> */ {"function-id", "(", "<call-par>", ")", "", "", "", "", "", ""},
        //{"id", "<call-par>", "", "", "", "", "", "", "", ""},
        //{"", "", "", "", "", "", "", "", "", ""},
        /*19. <stat> -> */ {"if", "<expr>", "then", "EOL", "<st-list>", "else", "EOL", "<st-list>", "end", "EOL"},
        /*20. <stat> -> */ {"while", "<expr>", "do", "EOL", "<st-list>", "end", "EOL", "", "", ""}
};

/**
 * Function changes top non-terminal in stack to its right side according to rule number.
 *
 * @param stack pointer to tStackPredictive structure is stack from which symbol is changed
 * @param ruleNumber int number of rule by which symbol is changed
 */
void tStackPredictiveChangeTop(tStackPredictive* stack, int ruleNumber) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        tStackPredictivePop(stack);
        //char* newTop = malloc(sizeof(strlen(rightSides[ruleNumber-1])));
        //newTop = rightSides[ruleNumber-1];
        //char* emptyString = malloc(2);
        if (strcmp(rightSides[ruleNumber-1][0], "") != 0) {
            //tStackPredictivePop(stack);
            //} else {
            for (int i = 8; i >= 0; i--) {
                //appendChar(emptyString, newTop[i]);
                if (strcmp(rightSides[ruleNumber - 1][i], "") != 0) {
                    tStackPredictivePush(stack, rightSides[ruleNumber - 1][i]);
                }
                /*if (newTop[i] != '\'') {
                    if (newTop[i+1] != '\0' && newTop[i+1] != '\'') {
                        tStackPredictivePush(stack, emptyString);
                    }
                } else {
                    tStackPredictivePush(stack, emptyString);
                }*/
            }
        }
    }
}

/**
 * Function calculates row offset.
 *
 * @param symbol pointer to char is sought row
 * @return number row by which to look into the table
 */
int rowOffset(char* symbol) {
    char* row[] = {"<start>", "<function>", "<function-head>", "<function-tail>", "<par>", "<next-par>", "<st-list>", "<stat>", "<eval>", "<assign>"};
    for (int i = 0; i < 10; ++i) {
        if (strcmp(symbol, row[i]) == 0) {
            return i;
        }
    }
    return 12;
}

/**
 * Function calculates column offset.
 *
 * @param symbol pointer to char is sought col
 * @return column number by which to look into the table
 */
int colOffset(char* symbol) {
    char* col[] = {"def", "EOL", "end", ",", "=", "id", "expr", "(", ")", "if", "then", "else", "while", "do", "function-id", "$"};
    for (int i = 0; i < 16; i++) {
        if (strcmp(symbol, col[i]) == 0) {
            return i;
        }
    }
    return 18;
}

int LLTable[10][16] = {
        /*<start*/          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<function>*/      {2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<function-head>*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0},
        /*<function-tail>*/ {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<par>*/           {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<next-par>*/      {0, 9, 0, 8, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 9},
        //{0, 0, 0, 0, 0, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<st-list>*/       {0, 13, 11, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12},
        /*<stat>*/          {0, 0, 0, 0, 0, 14, 0, 0, 0, 19, 0, 0, 20, 0, 0, 0},
        /*<eval>*/          {0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15},
        /*<assign>*/        {0, 0, 0, 0, 0, 17, 17, 17, 0, 0, 0, 0, 0, 0, 18, 0},
        //{0, 23, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

/**
 * Function decides whether given symbol is a terminal or not.
 *
 * @param symbol pointer to char is symbol compared to terminals
 * @return non-zero value if symbol is terminal otherwise zero is returned
 */
int isTerminal(char* symbol) {
    char* terminals[] = {"def", "EOL", "(", ")", "end", "+", "-", "*", "/", ".", ",", "integer", "float", "string", "=", "if", "then", "else", "while", "do", "id", "function-id"};
    for (int i = 0; i < 22; ++i) {
        if (strcmp(terminals[i], symbol) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 *
 * @param inputToken
 * @param AST
 */
void simulatePredictive(Token token, tASTPointer* AST, tStackPredictive* predictiveStack) {
    // allocated needed stacks

    tStackASTPtr* stackAST;
    stackAST = malloc(sizeof(struct tStackAST));

    int end = 0;

    if (predictiveStack == NULL || stackAST == NULL) {                        // expendedStack error
        errorHandling(99);
    } else {
        //tStackPredictiveInit(predictiveStack);
        tStackASTInit(stackAST);

        //tStackPredictivePush(predictiveStack, "def");
        //tStackPredictivePush(predictiveStack, "function");

        //int tokenOffset = 0;                    // offset by which to look into the input token
        char* a;
        //char* b;
        char* emptyString = "";
        //fprintf(stdout, "a is:%s\n", a);
        int rule = 0;

        do {
            //b = "";
            //a = malloc(sizeof(strlen(tStackPredictiveGetTop(predictiveStack))));
            a = malloc(10);
            a = tStackPredictiveGetTop(predictiveStack);
            //char tmp2 = inputToken[tokenOffset];

            //b = appendChar(b, tmp2);


            /*if (rule == 0) {
                errorHandling(3);                       // no such rule
                break;
            }*/
            //char *handle;

            if (strcmp(a, "$") == 0) {
                if (strcmp(token.content, "$") == 0) {
                    end = 1;                            // success
                } else {
                    end = -1;                           // failure
                }
            } else if (isTerminal(a) != 0) {
                // todo: if top-most symbol in stack is id, if statement does not necessary mean true
                if (strcmp(a, token.content) == 0) {
                    tStackPredictivePop(predictiveStack);
                    end = 2;
                } else {
                    end = -1;
                }
            } else if (strcmp(a, "<expr>") != 0){
                int row = rowOffset(a);
                int col = colOffset(token.content);
                if (row > 11 || col > 17) {
                    errorHandling(99);                      // symbol doesn't occur in precedence table
                }
                rule = LLTable[row][col];
                if (rule == 0) {
                    errorHandling(3);                       // no such rule
                    end = -1;
                } else {
                    tStackPredictiveChangeTop(predictiveStack, rule);
                    fprintf(stdout, "Applying rule number: %d\n", rule);
                }
            } else {
                end = 3;                                    // need to calculate expression
            }

        } while (rule != 0 && end == 0);
    }

    // todo: do something for failure?
    /*if (end  == -1) {

    }*/

    // assign AST
    /*if (stackAST != NULL && ERROR_TYPE == 0) {
        *AST = *stackAST->body[stackAST->top];
    }*/

    // free allocated stacks
    //tStackPredictiveDispose(predictiveStack);
    //tStackASTDispose(stackAST);
}
