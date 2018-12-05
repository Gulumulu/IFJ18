/**
 * Project for IFJ course - compiler
 * Source file for predictive syntax analysis.
 *
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
    stack->content[0] = malloc(sizeof(char) * 2);
    stack->content[0] = strcpy(stack->content[0], "$");
    stack->content[0][1] = '\0';
    stack->content[1] = malloc(sizeof(char) * 8);
    stack->content[1] = strcpy(stack->content[1], "<start>");
    stack->content[1][7] = '\0';
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
        while (stack->top > 0) {
            free(stack->content[stack->top-1]);
            stack->content[stack->top-1] = NULL;
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
        stack->content[stack->top] = malloc(sizeof(char)*(strlen(symbol)+1));
        //checkMalloc(stack->content[stack->top]);
        stack->content[stack->top] = strcpy(stack->content[stack->top], symbol);
        stack->content[stack->top][strlen(symbol)] = '\0';
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
        free(stack->content[stack->top-1]);
        stack->content[stack->top-1] = NULL;
        stack->top--;
    }
}

char* rightSides[30][10] = {
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
        /*11. <st-list> -> */ {"EOL", "<st-list>", "", "", "", "", "", "", "", ""},
        /*12. <st-list> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*--. <st-list> -> */ //{"EOL", "", "", "", "", "", "", "", "", ""},
        /*13. <stat> -> */ {"id", "<eval>", "", "", "", "", "", "", "", ""},
        /*14. <eval> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*15. <eval> -> */ {"=", "<assign>", "", "", "", "", "", "", "", ""},
        //{"id", "=", "<assign>", "", "", "", "", "", "", ""},
        /*16. <assign> -> */ {"<expr>", "", "", "", "", "", "", "", "", ""},
        /*17. <assign> -> */ {"function-id", "<f-params>", "", "", "", "", "", "", "", ""},
        //{"id", "<call-par>", "", "", "", "", "", "", "", ""},
        //{"", "", "", "", "", "", "", "", "", ""},
        /*18. <stat> -> */ {"if", "<expr>", "then", "EOL", "<st-list>", "else", "EOL", "<st-list>", "end", ""},
        /*19. <stat> -> */ {"while", "<expr>", "do", "EOL", "<st-list>", "end", "", "", "", ""},
        /*20. <stat> -> */ {"print", "<print-expr>", "", "", "", "", "", "", "", ""},
        /*21. <print-expr> -> */ {"(", "<print-expr>", ")", "", "", "", "", "", "", ""},
        /*22. <print-expr> -> */ {"id", "<next-print-expr>", "", "", "", "", "", "", "", ""},
        /*23. <print-expr> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*24. <next-print-expr> -> */ {",", "<print-expr>", "", "", "", "", "", "", "", ""},
        /*25. <stat> -> */ {"function-id", "<f-params>", "", "", "", "", "", "", "", "" },
        /*26. <f-params> -> */ {"(", "<f-params>", ")", "", "", "", "", "", "", ""},
        /*27. <f-params> -> */ {"id", "<next-f-params>", "", "", "", "", "", "", "", ""},
        /*28. <f-params> -> */ {"", "", "", "", "", "", "", "", "", ""},
        /*29. <next-f-params> -> */ {",", "<f-params>", "", "", "", "", "", "", "", ""},
        /*30. <next-f-params> -> */ {"", "", "", "", "", "", "", "", "", ""}
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
    char* row[] = {"<start>", "<function>", "<function-head>", "<function-tail>", "<par>", "<next-par>", "<st-list>", "<stat>", "<eval>", "<assign>", "<print-expr>", "<next-print-expr>", "<f-params>", "<next-f-params>"};
    for (int i = 0; i < 14; ++i) {
        if (strcmp(symbol, row[i]) == 0) {
            return i;
        }
    }
    return 15;
}

/**
 * Function calculates column offset.
 *
 * @param symbol pointer to char is sought col
 * @return column number by which to look into the table
 */
int colOffset(TokenType symbol) {
    TokenType col[] = {kw_def, ss_eol, kw_end, s_comma, s_eq, s_id, s_string, s_lbrac, s_rbrac, kw_if, kw_then, kw_else, kw_while, kw_do, s_func_id, ss_eof, kw_print};
    for (int i = 0; i < 17; i++) {
        if (symbol == col[i]) {
            return i;
        }
    }
    if (symbol == s_int || symbol == s_float || symbol == s_exp_int || symbol == s_exp_int_s || symbol == s_exp_f || symbol == s_exp_f_s || symbol == kw_nil) {
        return 6;
    }
    if (symbol == kw_length || symbol == kw_substr || symbol == kw_ord || symbol == kw_chr || symbol == kw_inputf || symbol == kw_inputi || symbol == kw_inputs) {
        return 14;
    }
    return 18;
}

int LLTable[14][17] = {
        /*<start*/          {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1},
        /*<function>*/      {2, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 3, 0, 3, 3, 3},
        /*<function-head>*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0},
        /*<function-tail>*/ {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<par>*/           {0, 0, 0, 0, 0, 6, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<next-par>*/      {0, 9, 0, 8, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 9, },
        //{0, 0, 0, 0, 0, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0},
        /*<st-list>*/       {1, 11, 12, 0, 0, 10, 0, 0, 0, 10, 0, 12, 10, 0, 10, 12, 10},
        /*<stat>*/          {0, 0, 0, 0, 0, 13, 0, 0, 0, 18, 0, 0, 19, 0, 25, 0, 20},
        /*<eval>*/          {0, 14, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0},
        /*<assign>*/        {0, 0, 0, 0, 0, 16, 16, 16, 0, 0, 0, 0, 0, 0, 16, 0, 16},
        /*<print-expr*/     {0, 23, 0, 24, 0, 22, 22, 21, 23, 0, 0, 0, 0, 0, 0, 23, 0},
        /*<next-print-expr>*/ {0, 23, 0, 24, 0, 22, 22, 0, 23, 0, 0, 0, 0, 0, 0, 23, 0},
        /*<f-params>*/      {0, 28, 0, 29, 0, 27, 27, 26, 28, 0, 0, 0, 0, 0, 0, 28, 0},
        /*<next-f-params>*/ {0, 28, 0, 29, 0, 27, 27, 0, 30, 0, 0, 0, 0, 0, 0, 28, 0}
        //{0, 23, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

/**
 * Function decides whether given symbol is a terminal or not.
 *
 * @param symbol pointer to char is symbol compared to terminals
 * @return non-zero value if symbol is terminal otherwise zero is returned
 */
int isTerminal(char* symbol) {
    char* terminals[] = {"def", "EOL", "(", ")", "end", "+", "-", "*", "/", ".", ",", "integer", "float", "string", "=", "if", "then", "else", "while", "do", "id", "function-id", "print"};
    for (int i = 0; i < 23; ++i) {
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
 * Function clears and array of applied rules
 */
void clearRulesApplied() {
    for (int i = 0; i < 50; i++) {
        rulesApplied[i] = 0;
    }
}

/**
 * Function counts how many rules while loading arguments were used during function use.
 *
 * @return number of arguments that were input
 */
int checkRulesApplied() {
    int numberOfArgs = 0;
    for (int i = 50; i > 0 && rulesApplied[i] != 17; i--) {
        if (rulesApplied[i] == 27) {
            numberOfArgs++;
        }
    }
    return numberOfArgs;
}

/**
 * Function checks whether user-defined function was called with the right number of args
 *
 * @param functionContent BSTNodeContentPtr* is pointer to node content in gloabl symtable
 * @param numberOfArgs int number is number of arguments
 *
 * @return non-zero value is returned if arguments are matchings otherwise zero-value is returned
 */
int checkUserFunction(BSTNodeContentPtr* functionContent, int numberOfArgs) {
    if (functionContent == NULL) {
        return 0;
    } else if (functionContent->func_params == numberOfArgs){
        return 1;
    }

    return 0;
}

/**
 * Function checks whether function arguments are of correct type.
 *
 * @param function tokenType is input function
 * @return non-zero value is returned if arguments are correct otherwise zero-value is returned
 */
int checkArgsType(TokenType function) {
    switch (function) {
        case kw_length:
            if (strcmp(tStackPredictiveGetTop(argsTracker), "string") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                return 1;
            }
            return 0;
        case kw_substr:
            if (strcmp(tStackPredictiveGetTop(argsTracker), "integer") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                tStackPredictivePop(argsTracker);
                if (strcmp(tStackPredictiveGetTop(argsTracker), "integer") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                    tStackPredictivePop(argsTracker);
                    if (strcmp(tStackPredictiveGetTop(argsTracker), "string") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                        return 1;
                    }
                }
            }
            return 0;
        case kw_ord:
            if (strcmp(tStackPredictiveGetTop(argsTracker), "integer") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                tStackPredictivePop(argsTracker);
                if (strcmp(tStackPredictiveGetTop(argsTracker), "string") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                    return 1;
                }
            }
            return 0;
        case kw_chr:
            if (strcmp(tStackPredictiveGetTop(argsTracker), "integer") == 0 || strcmp(tStackPredictiveGetTop(argsTracker), "undecided") == 0) {
                return 1;
            }
            return 0;
        case kw_inputf:
            break;
        case kw_inputi:
            break;
        case kw_inputs:
            break;
        case kw_print:

            break;
        default:

            break;
    }
    return 1;
}

/**
 * Function checks whether of arguments are correct for build-in functions.
 *
 * @param inputFunction tokenType is input function
 * @param globalSymtable BSTNotePtr* is pointer to root of gloabl symtable
 * @return non-zero value is returned if number of arguments is correct
 */
int checkNumberOfArgs(TokenType inputFunction, BSTNodePtr* globalSymtable) {
    //BSTNodeContentPtr* functionContent = malloc(sizeof(struct BSTNodeContent));
    switch (inputFunction) {
        case kw_length:
            if (checkRulesApplied() == 1 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_substr:
            if (checkRulesApplied() == 3 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_ord:
            if (checkRulesApplied() == 2 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_chr:
            if (checkRulesApplied() == 1 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_inputf:
            if (checkRulesApplied() == 0 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_inputi:
            if (checkRulesApplied() == 0 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case kw_inputs:
            if (checkRulesApplied() == 0 ) {
                if (checkArgsType(inputFunction) != 1) {
                    errorHandling(4);
                    return 0;
                }
                return 1;
            }
            break;
        case s_func_id:
            if (checkUserFunction(BSTSearch(globalSymtable, hash_id(inputFunctionName)), checkRulesApplied()) == 1) {
                return 1;
            }
            break;
        case kw_print:
            if (checkArgsType(inputFunction) != 1) {
                errorHandling(4);
                return 0;
            }
            return 1;
        default:
            return 0;
    }
    return 0;
}

/**
 * Function checks whether rule #3 was applied. If it was we are in main function
 *
 * @return non-zero value is returned if rule #3 was applied otherwise zero value is returned
 */
int checkMainFunction() {
    for (int i = 0; i < 50; i++) {
        if (rulesApplied[i] == 3 || rulesApplied[i] == 5) {
            return 1;
        }
    }
    return 0;
}
/**
 * Function checks whether rule #16 was applied. If it was an expression follows.
 *
 * @return non-zero value is returned if rule #16 was applied otherwise zero value is returned
 */
int checkAssignRule() {
    for (int i = 0; i < 50; i++) {
        if (rulesApplied[i] == 16) {
            return 1;
        }
    }
    return 0;
}

/**
 * Function pushes argument type into the stack.
 *
 * @param token Token is input token
 * @param node BSTNodePtr* is pointer to current variable in symmtable
 */
void pushArg(Token* token, BSTNodePtr* node) {
    if (token == NULL || node == NULL) {
        errorHandling(99);
    } else {
        //BSTNodeContentPtr* tmpID = BSTSearch(node, hash_id(token->content));
        //tmpID = BSTSearch(node, hash_id(token->content));
        switch (token->type) {
            case s_string:
                tStackPredictivePush(argsTracker, "string");
                break;
            case s_int:
                tStackPredictivePush(argsTracker, "integer");
                break;
            case s_float:
                tStackPredictivePush(argsTracker, "float");
                break;
            case s_id:
                if (BSTSearch(node, hash_id(token->content)) != NULL) {
                    if (BSTSearch(node, hash_id(token->content))->var != NULL) {
                        // push type of variable
                        tStackPredictivePush(argsTracker, BSTSearch(node, hash_id(token->content))->var);
                    } else {
                        // push undecided if type is not known yet
                        tStackPredictivePush(argsTracker, "undecided");
                    }
                }
            default:
                break;
        }
        //free(tmpID);
        //tmpID=NULL;
    }
}

/**
 * Function simulates predictive syntax analysis for given token.
 *
 * @param token token is given token from lexical analysis
 * @param predictiveStack pointer to tStackPredictive structure is predictive stack
 * @param globalSymtable BSTNodePtr* is pointer to root of global symtable
 * @param node BSTNodePtr* is pointer to current variable in symtable
 */
void simulatePredictive(Token token, tStackPredictive* predictiveStack, BSTNodePtr* globalSymtable, BSTNodePtr* node) {
    int end = 0;

    if (predictiveStack == NULL) {                        // expendedStack error
        errorHandling(99);
    } else {
        char* predictiveStackTop = NULL;                           // top rule in stack

        do {
            size_t predictiveStackTopLen = strlen(tStackPredictiveGetTop(predictiveStack));
            predictiveStackTop = malloc(sizeof(char)*(predictiveStackTopLen+1));
            predictiveStackTop = strcpy(predictiveStackTop, tStackPredictiveGetTop(predictiveStack));
            predictiveStackTop[(int)predictiveStackTopLen] = '\0';

            if (strcmp(predictiveStackTop, "$") == 0) {
                // end of predictiveStack was reached
                if (token.type == ss_eof) {
                    end = 1;                                // success
                    //fprintf(stdout, "SUCCESS, YOU ARE AWESOME!\n");
                } else if (token.type == kw_def) {
                    tStackPredictivePush(predictiveStack, "<start>");  // another function follows
                    rule = 1;
                } else if (token.type == s_id) {
                    tStackPredictivePush(predictiveStack, "<st-list>"); // statement not included in any function
                    rule = 10;
                } else {
                    end = -1;                               // failure
                }
            } else if (isTerminal(predictiveStackTop) != 0) {
                // terminal is on top of the predictiveStack
                // todo: if top-most symbol in stack is id, if statement does not necessary mean true
                if (strcmp(predictiveStackTop, token.content) == 0 || token.type == s_id  || token.type == s_func_id || (strcmp(predictiveStackTop, "EOL") == 0 && token.type == ss_eol) || token.type == kw_length || token.type == kw_substr || token.type == kw_ord || token.type == kw_chr || token.type == kw_inputf || token.type == kw_inputi || token.type == kw_inputs || token.type == kw_print) {
                    tStackPredictivePop(predictiveStack);
                    end = 2;
                    if (checkingArgs == 1 && token.type != s_lbrac && token.type != s_rbrac && token.type != s_comma) {
                        // this token is function argument => push type to stack
                        pushArg(&token, node);
                    }
                } else if (strcmp(predictiveStackTop, "EOL") == 0 && token.type == ss_eof) {
                    tStackPredictivePop(predictiveStack);
                    rule = 2;
                } else if (strcmp(predictiveStackTop, "id") == 0 && checkingArgs == 1) {
                    tStackPredictivePop(predictiveStack);
                    end = 2;
                    if (checkingArgs == 1 && token.type != s_lbrac && token.type != s_rbrac && token.type != s_comma) {
                        // this token is function argument => push type to stack
                        pushArg(&token, node);
                    }
                } else {
                    end = -1;
                }
                if (checkingArgs == 1 && (token.type == s_rbrac || token.type == ss_eol || token.type == ss_eof) ) {
                    // need to check number of arguments of function
                    checkingArgs = 0;
                    if (checkNumberOfArgs(inputFunction, globalSymtable) == 0) {
                        errorHandling(45);
                        end = -1;
                    }
                    free(inputFunctionName);
                    inputFunctionName = NULL;
                    tStackPredictiveDispose(argsTracker);
                    free(argsTracker);
                    inputFunction = s_comma;
                }
                if (inputFunction == kw_print) {
                    printing = 1;
                }
            } else if (strcmp(predictiveStackTop, "<expr>") != 0){
                // non-terminal in on top of the predictiveStack && no need to calculate expression
                int row = rowOffset(predictiveStackTop);
                int col = colOffset(token.type);
                if (row > 14 || col > 17) {
                    errorHandling(40);                      // symbol doesn't occur in LL rule table
                    end = -1;
                } else {
                    rule = LLTable[row][col];
                    if (rule == 0) {
                        errorHandling(41);                      // no such rule
                        end = -1;
                    } else {
                        // change non-terminal according to rule from LL grammar
                        tStackPredictiveChangeTop(predictiveStack, rule);
                        //fprintf(stdout, "Applying rule number: %d\n", rule);
                        fillRulesApplied(rule);
                    }
                    if (rule == 17 || rule == 25 || rule == 22) {
                        // need to check arguments of function
                        checkingArgs = 1;
                        if (rule == 22) {
                            inputFunction = kw_print;
                        } else {
                            inputFunction = token.type;
                        }
                        //inputFunctionName = "";
                        inputFunctionName = malloc(strlen(token.content)+1);
                        inputFunctionName = memcpy(inputFunctionName, token.content, strlen(token.content));
                        inputFunctionName[strlen(token.content)] = '\0';
                        argsTracker = malloc(sizeof(tStackPredictive)*5);
                        if (argsTracker == NULL) {
                            errorHandling(99);
                            break;
                        }
                        tStackPredictiveInit(argsTracker);
                        tStackPredictivePop(argsTracker);
                    }
                }
            } else if (rule == 22 || rule == 21) {
                // need to print
                printing = 1;
                end = 3;
            } else {
                // need to calculate expression
                precedence = 1;
                end = 3;
            }

            free(predictiveStackTop);
            predictiveStackTop = NULL;
        } while (rule != 0 && end == 0);
        if (predictiveStackTop != NULL) {
            free(predictiveStackTop);
            predictiveStackTop = NULL;
        }
        /*if (strcmp(predictiveStack->content[predictiveStack->top-1],"<expr>") == 0 && printing != 1) {
            // expression will be calculated next
            precedence = 1;
        }*/
    }

    if (end  == -1) {
        // finding rule resulted in failure
        errorHandling(41);
    }
}
