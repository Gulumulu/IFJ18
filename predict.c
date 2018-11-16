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
        stack->content[stack->top] = malloc(sizeof(strlen(symbol)));
        checkMalloc(stack->content[stack->top]);
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

char* rightSides[19][10] = {
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
        /*10. <st-list> -> */ {"<stat>", "EOL", "<st-list>", "", "", "", "", "", "", ""},
        /*11. <st-list> -> */ {"EOL", "", "", "", "", "", "", "", "", ""},
        /*12. <st-list> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*--. <st-list> -> */ //{"EOL", "", "", "", "", "", "", "", "", ""},
        /*13. <stat> -> */ {"id", "<eval>", "", "", "", "", "", "", "", ""},
        /*14. <eval> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*15. <eval> -> */ {"=", "<assign>", "", "", "", "", "", "", "", ""},
        //{"id", "=", "<assign>", "", "", "", "", "", "", ""},
        /*16. <assign> -> */ {"<expr>", "", "", "", "", "", "", "", "", ""},
        /*17. <assign> -> */ {"function-id", "(", "<call-par>", ")", "", "", "", "", "", ""},
        //{"id", "<call-par>", "", "", "", "", "", "", "", ""},
        //{"", "", "", "", "", "", "", "", "", ""},
        /*18. <stat> -> */ {"if", "<expr>", "then", "EOL", "<st-list>", "else", "EOL", "<st-list>", "end", "EOL"},
        /*19. <stat> -> */ {"while", "<expr>", "do", "EOL", "<st-list>", "end", "EOL", "", "", ""}
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
int colOffset(TokenType symbol) {
    TokenType col[] = {kw_def, ss_eol, kw_end, s_comma, s_eq, s_id, s_exp_int_s, s_lbrac, s_rbrac, kw_if, kw_then, kw_else, kw_while, kw_do, s_func_id, ss_eof};
    for (int i = 0; i < 16; i++) {
        if (symbol == col[i]) {
            return i;
        }
    }
    if (symbol == s_int || symbol == s_float || symbol == s_exp_int || symbol == s_exp_int_s || symbol == s_exp_f || symbol == s_exp_f_s) {
        return 6;
    }
    return 18;
}

int LLTable[10][16] = {
        /*<start*/          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<function>*/      {2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
        /*<function-head>*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0},
        /*<function-tail>*/ {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<par>*/           {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<next-par>*/      {0, 9, 0, 8, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 9},
        //{0, 0, 0, 0, 0, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<st-list>*/       {0, 11, 12, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12},
        /*<stat>*/          {0, 0, 0, 0, 0, 13, 0, 0, 0, 18, 0, 0, 19, 0, 0, 0},
        /*<eval>*/          {0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14},
        /*<assign>*/        {0, 0, 0, 0, 0, 16, 16, 16, 0, 0, 0, 0, 0, 0, 17, 0},
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
 * Function stores applied rule in an array of applied rule rulesApplied.
 *
 * @param rule int number of rule currently being applied
 */
void fillRulesApplied(int rule) {
    for (int i = 0; i < 50; i++) {
        if (rulesApplied[i] == 0) {
            rulesApplied[i] = rule;
            break;
        }
    }
}

/**
 * Function simulates predictive syntax analysis for given token.
 *
 * @param token token is given token from lexical analysis
 * @param predictiveStack
 */
void simulatePredictive(Token token, tStackPredictive* predictiveStack) {
    // allocated needed stacks

    //tStackASTPtr* stackAST;
    //stackAST = malloc(sizeof(struct tStackAST));

    int end = 0;

    if (predictiveStack == NULL) {                        // expendedStack error
        errorHandling(99);
    } else {
        //tStackPredictiveInit(predictiveStack);
        //tStackASTInit(stackAST);

        //tStackPredictivePush(predictiveStack, "def");
        //tStackPredictivePush(predictiveStack, "function");

        //int tokenOffset = 0;                    // offset by which to look into the input token
        char* predictiveStackTop;
        //char* b;
        //char* emptyString = "";
        //fprintf(stdout, "predictiveStackTop is:%s\n", predictiveStackTop);
        int rule = 0;

        do {
            //b = "";
            //predictiveStackTop = malloc(sizeof(strlen(tStackPredictiveGetTop(predictiveStack))));
            predictiveStackTop = malloc(10);
            predictiveStackTop = tStackPredictiveGetTop(predictiveStack);
            //char tmp2 = inputToken[tokenOffset];

            //b = appendChar(b, tmp2);


            /*if (rule == 0) {
                errorHandling(3);                       // no such rule
                break;
            }*/
            //char *handle;

            if (strcmp(predictiveStackTop, "$") == 0) {
                // end of predictiveStack was reached
                if (token.type == ss_eof) {
                    end = 1;                                // success
                    fprintf(stdout, "SUCCESS, YOU ARE AWESOME!");
                } else if (token.type == kw_def) {
                    tStackPredictivePush(predictiveStack, "<start>");  // another function follows
                    rule = 1;
                } else {
                    end = -1;                               // failure
                }
            } else if (isTerminal(predictiveStackTop) != 0) {
                // terminal is on top of the predictiveStack
                // todo: if top-most symbol in stack is id, if statement does not necessary mean true
                if (strcmp(predictiveStackTop, token.content) == 0 || token.type == s_id  || token.type == s_func_id || (strcmp(predictiveStackTop, "EOL") == 0 && token.type == ss_eol)) {
                    tStackPredictivePop(predictiveStack);
                    end = 2;
                } else if (strcmp(predictiveStackTop, "EOL") == 0 && token.type == ss_eof) {
                    tStackPredictivePop(predictiveStack);
                    rule = 2;
                }else {
                    end = -1;
                }
            } else if (strcmp(predictiveStackTop, "<expr>") != 0){
                // non-terminal in on top of the predictiveStack && no need to calculate expression
                int row = rowOffset(predictiveStackTop);
                int col = colOffset(token.type);
                if (row > 11 || col > 17) {
                    errorHandling(40);                      // symbol doesn't occur in LL rule table
                } else {
                    rule = LLTable[row][col];
                    if (rule == 0) {
                        errorHandling(41);                      // no such rule
                        end = -1;
                    } else {
                        // change non-terminal according to rule from LL grammar
                        tStackPredictiveChangeTop(predictiveStack, rule);
                        fprintf(stdout, "Applying rule number: %d\n", rule);
                        fillRulesApplied(rule);
                    }
                }
            } else {
                // need to calculate expression
                precedence = 1;
                end = 3;
            }

        } while (rule != 0 && end == 0);

        if (strcmp(predictiveStack->content[predictiveStack->top-1],"<expr>") == 0) {
            // expression will be calculated next
            precedence = 1;
        }
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
