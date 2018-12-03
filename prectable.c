/**
 * Implemented by Marek Varga, xvarga14
 * Precedence table
 */
#include "prectable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Function checks whether given string passed malloc.
 *
 * @param checkedString pointer to char is checked string
 */
void checkMalloc(const char* checkedString) {
    if (checkedString == NULL) {
        errorHandling(99);
    }
}

/**
 * Function initializes stack used while working with operator precedence table.
 *
 * @param stack pointer to tExpendedStack structure is initialized stack
 */
void init(tExpendedStack* stack) {
    stack->top = 1;
    stack->content = malloc(sizeof(char)*2);
    stack->content = strcpy(stack->content, "$");
    stack->content[1] = '\0';
}

/**
 * Function disposes stack.
 *
 * @param stack pointer to tExpendedStack structure is disposed stack
 */
void dispose(tExpendedStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        if (stack->top >= 0) {
            free(stack->content);
            stack->content = NULL;
        }
        free(stack);
    }
}

/**
 * Function "pushes" new element into the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack that will contain new element
 * @param c pointer to char is pushed element
 */
void push(tExpendedStack* stack, char* c) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        size_t len = strlen(stack->content);
        char *tmp = malloc(sizeof(char)*(strlen(stack->content) + strlen(c) + 1));
        //char *tmp = malloc((strlen(stack->content) + strlen(c))*sizeof(char));
        checkMalloc(tmp);
        strcpy(tmp, stack->content);
        tmp[strlen(stack->content)] = '\0';
        free(stack->content);
        stack->content = malloc(sizeof(char)*(strlen(tmp) + strlen(c) + 1));
        //stack->content = malloc((strlen(tmp) + strlen(c))*sizeof(char));
        checkMalloc(stack->content);
        // todo: pushing < into the stack that is not endRuleSign
        if (strcmp(c, "<=") == 0) {     // in case we are pushing two chars
            strcat(tmp, ".");
        } else if (strcmp(c, ">=") == 0) {
            strcat(tmp, ",");
        } else if (strcmp(c, "==") == 0) {
            strcat(tmp, "?");
        } else if (strcmp(c, "!=") == 0) {
            strcat(tmp, "!");
        } else if (strcmp(c, ">") == 0) {
            strcat(tmp, "g");
        } else if (strcmp(c, "<") == 0) {
            strcat(tmp, "l");
        } else {
            strcat(tmp, c);
        }
        tmp[len+1] = '\0';
        strcpy(stack->content, tmp);
        stack->content[strlen(tmp)] = '\0';
        stack->top++;
        free(tmp);
        tmp=NULL;
    }
}

/**
 * Function "pops" top element from the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack that will be removed of element
 */
void pop(tExpendedStack* stack) {
    if (stack->top == 0) {
        errorHandling(99);
    } else {
        stack->top--;
        char* tmp = malloc(sizeof(char)*(strlen(stack->content)));
        checkMalloc(tmp);
        tmp = memcpy(tmp, stack->content, strlen(stack->content)-1);
        tmp[strlen(stack->content)-1] = '\0';
        free(stack->content);
        stack->content = malloc(sizeof(char)*(strlen(tmp)+1));
        checkMalloc(stack->content);
        stack->content = memcpy(stack->content, tmp, strlen(tmp));
        stack->content[strlen(tmp)] = '\0';
        free(tmp);
        tmp=NULL;
    }
}

/**
 * Operator precedence table
 */
char precTable[14][14] = {
               //*   -   *   /   <   <=  >   >=  ==  !=  (   )   i   $
    /* + */    {'>','>','<','<','>','>','>','>','>','>','<','>','<','>'},
    /* - */    {'>','>','<','<','>','>','>','>','>','>','<','>','<','>'},
    /* * */    {'>','>','>','>','>','>','>','>','>','>','<','>','<','>'},
    /* / */    {'>','>','>','>','>','>','>','>','>','>','<','>','<','>'},
    /* < */    {'<','<','<','<','=','=','=','=','>','>','<','>','<','>'},
    /* <= */   {'<','<','<','<','=','=','=','=','>','>','<','>','<','>'},
    /* > */    {'<','<','<','<','=','=','=','=','>','>','<','>','<','>'},
    /* >= */   {'<','<','<','<','=','=','=','=','>','>','<','>','<','>'},
    /* == */   {'<','<','<','<','<','<','<','<','=','=','<','>','<','>'},
    /* != */   {'<','<','<','<','<','<','<','<','=','=','<','>','<','>'},
    /* ( */    {'<','<','<','<','<','<','<','<','<','<','<','=','<','E'},
    /* ) */    {'>','>','>','>','>','>','>','>','>','>','E','>','E','>'},
    /* i */    {'>','>','>','>','>','>','>','>','>','>','E','>','E','>'},
    /* $ */    {'<','<','<','<','<','<','<','<','<','<','<','E','<','E'}
};

/**
 * Function returns row/column of interest in precedence table based on terminal.
 *
 * @param terminal pointer to char is terminal symbol
 * @return int row/col for precedence table
 */
int getTableOffset(char* terminal) {
    if (strcmp(terminal, "+") == 0) {
        return 0;
    } else if (strcmp(terminal, "-") == 0) {
        return 1;
    } else if (strcmp(terminal, "*") == 0) {
        return 2;
    } else if (strcmp(terminal, "/") == 0) {
        return 3;
    } else if (strcmp(terminal, "<") == 0 || strcmp(terminal, "l") == 0) {
        return 4;
    } else if (strcmp(terminal, "<=") == 0 || strcmp(terminal, ".") == 0) {
        return 5;
    } else if (strcmp(terminal, ">") == 0 || strcmp(terminal, "g") == 0) {
        return 6;
    } else if (strcmp(terminal, ">=") == 0 || strcmp(terminal, ",") == 0) {
        return 7;
    } else if (strcmp(terminal, "==") == 0 || strcmp(terminal, "?") == 0) {
        return 8;
    } else if (strcmp(terminal, "!=") == 0 || strcmp(terminal, "!") == 0) {
        return 9;
    } else if (strcmp(terminal, "(") == 0) {
        return 10;
    } else if (strcmp(terminal, ")") == 0) {
        return 11;
    } else if (strcmp(terminal, "i") == 0) {
        return 12;
    } else if (strcmp(terminal, "$") == 0) {
        return 13;
    } else {
        return 14;
    }
}

char changeTokenTypeToChar(TokenType tokenType) {
    switch (tokenType) {
        case s_add:
            return '+';
        case s_sub:
            return '-';
        case s_mul:
            return '*';
        case s_div:
            return '/';
        case s_less:
            return '<';
        case s_less_eq:
            return '.';
        case s_great:
            return 'g';
        case s_great_eq:
            return ',';
        case s_eqto:
            return '?';
        case s_noteq:
            return '!';
        case s_lbrac:
            return '(';
        case s_rbrac:
            return ')';
        case s_id:
        case s_exp_f_s:
        case s_exp_f:
        case s_float:
        case s_int:
        case s_exp_int_s:
        case s_exp_int:
        case s_string:
        case s_func_expr:
            return 'i';
        case ss_eol:
        case ss_eof:
        case kw_then:
        case kw_do:
            return '$';
        case kw_length:
        case kw_inputs:
        case kw_inputi:
        case kw_inputf:
        case kw_print:
        case kw_chr:
        case kw_ord:
        case kw_substr:
        case s_func_id:
            return 'f';
        default:
            return '&';
    }
}

/**
 * Function returns first terminal from top of the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack from which terminal is sought
 * @return char of first terminal
 */
char getTop(tExpendedStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return '$';
    } else {
        if (stack->top == 1) {
            return stack->content[0];
        } else {
            int tmp = stack->top-1;
            const char* terminals = "+-*/l.g,?!()i$";
            while (strchr(terminals,stack->content[tmp]) == NULL) {
                tmp--;
            }
            return stack->content[tmp];
        }
    }
}

/**
 * Function appends char to string and returns newly formed string.
 *
 * @param string pointer to char that will be concatenated
 * @param addedChar char that will be added
 * @return newly formed string
 */
void appendChar(char* string, char addedChar) {
    size_t len = strlen(string);
    char* returnedString = malloc(len + sizeof(char) * 2);
    checkMalloc(returnedString);
    strcpy(returnedString, string);
    returnedString[len] = addedChar;
    returnedString[len+1] = '\0';
    size_t len2 = strlen(returnedString);
    string = malloc(len2+1);
    checkMalloc(string);
    string = strcpy(string, returnedString);
    string[strlen(returnedString)] = '\0';
    free(returnedString);
    returnedString = NULL;
}

char* catStrings(char* string1, char* string2) {
    char* returnedString = malloc((strlen(string1)+strlen(string2)+1)*sizeof(char));
    checkMalloc(returnedString);
    strcpy(returnedString, string1);
    returnedString[strlen(string1)] = '\0';
    strcat(returnedString, string2);
    returnedString[strlen(string1)+strlen(string2)+1] = '\0';
    return returnedString;
}

/**
 * Function changes top-most terminal in stack to top-most+'<'. E.g. '$' -> '$<'
 *
 * @param stack pointer to tExpendedStack structure is stack to which rule sign is pushed
 * @param firstChar character of top-most terminal in stack
 */
void pushEndRuleSign(tExpendedStack* stack, char firstChar) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        char* rest = NULL;
        size_t len;
        if (strrchr(stack->content, firstChar) == NULL) {
            rest = malloc(sizeof(char) * 1);
            rest[0] = '\0';
        } else {
            rest = malloc(sizeof(char)*(strlen(strrchr(stack->content, firstChar))+1));
            if (rest == NULL) {
                errorHandling(99);
            }
            len = strlen(strrchr(stack->content, firstChar));
            rest = strcpy(rest,strrchr(stack->content, firstChar));
            rest[len] = '\0';
        }
        //rest[strlen(strrchr(stack->content, firstChar))] = '\0';
        char *beginning = malloc((strlen(stack->content) + 2)*sizeof(char));
        memcpy(beginning, stack->content, (strlen(stack->content) - strlen(rest)) + 1);
        beginning[(strlen(stack->content) - strlen(rest)) + 1] = '<';
        beginning[(strlen(stack->content) - strlen(rest)) + 2] = '\0';
        //appendChar(beginning, '<');
        for (int i = 0; i < (int) strlen(rest) - 1; i++) {
            rest[i] = rest[i + 1];
        }
        rest[strlen(rest)-1] = '\0';
        if (strlen(rest) != 2) {
            rest[strlen(rest)] = '\0';
            size_t len2 = strlen(beginning);
            strcat(beginning, rest);
            beginning[len2+strlen(rest)] = '\0';
        } else {
            size_t len2 = strlen(beginning);
            //strcat(beginning, "\0");
            beginning[len2+strlen(rest)] = '\0';
        }
        free(stack->content);
        stack->content = malloc((strlen(beginning)+1)*sizeof(char));
        stack->content = memcpy(stack->content, beginning, strlen(beginning));
        stack->content[strlen(beginning)] = '\0';
        //stack->content = beginning;
        free(beginning);
        beginning = NULL;
        free(rest);
        rest = NULL;
        stack->top++;
    }
}

/**
 * Function changes terminals to non terminals according to rules.
 *
 * @param stack pointer to tExpendedStack structure is stack in which terminals are changed
 * @param handle pointer to char is right side of rule
 * @param rule pointer to char is left side of rule
 */
void applyRule(tExpendedStack* stack, char* handle,char* rule) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        char *tmpStackContent = NULL;
        tmpStackContent = malloc(sizeof(char)*(strlen(stack->content) - strlen(handle) + strlen(rule)+1));
        //tmpStackContent = malloc(10);
        //tmpStackContent = memmove(tmpStackContent, stack->content, strlen(stack->content) - strlen(handle));
        tmpStackContent = memcpy(tmpStackContent, stack->content, strlen(stack->content) - strlen(handle));
        tmpStackContent[strlen(stack->content) - strlen(handle)] = '\0';
        size_t len = strlen(tmpStackContent);
        tmpStackContent = strcat(tmpStackContent, rule);
        tmpStackContent[len + strlen(rule)] = '\0';
        //tmpStackContent = appendChar(tmpStackContent, 'E');
        stack->top -= (strlen(handle) - 1);
        free(stack->content);
        stack->content = malloc(sizeof(char)*(strlen(tmpStackContent)+1));
        stack->content = strcpy(stack->content, tmpStackContent);
        stack->content[strlen(tmpStackContent)] = '\0';
        //stack->content = tmpStackContent;
        free(tmpStackContent);
    }
}

/**
 * Function checks whether rule exists according to right side a.k.a handle.
 *
 * @param stack pointer to tExpendedStack structure is stack in which terminals are changed
 * @param handle pointer to char is right side of rule
 */
int changeHandle(tExpendedStack* stack, char* handle) {
    if (stack == NULL) {
        errorHandling(99);
        return 0;
    } else {
        char* bigE = "E";
        if (strcmp(handle, "<i") == 0) {
            applyRule(stack, handle, bigE);
            rule = 4;
        } else if (strcmp(handle, "<E+E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 1;
        } else if (strcmp(handle, "<E-E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 1;
        } else if (strcmp(handle, "<E*E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<(E)") == 0) {
            applyRule(stack, handle, bigE);
            rule = 3;
        } else if (strcmp(handle, "<E/E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<ElE") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<EgE") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<E.E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<E,E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<E?E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else if (strcmp(handle, "<E!E") == 0) {
            applyRule(stack, handle, bigE);
            rule = 2;
        } else {
            errorHandling(42);                  // no rule for this kind of handle
            return 0;
        }
        return 1;
    }
}

/**
 * Function decides whether current token is id or function id based on next token.
 *
 * @param nextToken Token type is next token after current token
 * @return TokenType s_id if current token is variable otherwise s_func_id is returned if current token is function
 */
TokenType decideID(Token nextToken) {
    switch (nextToken.type) {
        case s_lbrac:
        case s_id:
            return s_func_id;
        default:
            return s_id;
    }
}

/**
 * Function simulates operator precedence look up for given token.
 *
 * @param Token token is given token from lexical analysis
 * @param expendedStack
 * @param stackAST
 * @param node
 */
void simulatePrecedence(Token token, tExpendedStack* expendedStack, tStackASTPtr* stackAST, BSTNodePtr* node, BSTNodePtr* globalSymtable) {

    if (expendedStack == NULL || stackAST == NULL) {                        // expendedStack error
        errorHandling(99);
    } else {
        char* a = malloc(sizeof(char) * 2);
        char* c = malloc((sizeof(char)) * 2);
        char* emptyString = malloc(sizeof(char) * 2);
        int end = 0;
        if (stackPredictive == NULL) {
            functionName = malloc(sizeof(char)*1);
            functionName = strcpy(functionName, "");
            functionName[0] = '\0';
            stackPredictive = malloc(sizeof(tStackPredictive)*15);
            tStackPredictiveInit(stackPredictive);
            tStackPredictivePop(stackPredictive);
            tStackPredictivePush(stackPredictive, "EOL");
            tStackPredictivePush(stackPredictive, "<stat>");
            //tStackPredictivePush(stackPredictive, "function-id");
        }

        if (token.type == ss_eol || token.type == kw_then || token.type == ss_eof || token.type == kw_do) {
            precedence = 0;                     // precedence SA has finished, need to do predictive SA with the same token
        }
        do {
            //a = "";
            //c = "";
            char tmp1 = getTop(expendedStack);
            char tmp3 = changeTokenTypeToChar(token.type);

            //a = malloc(sizeof(char) * 2);
            //appendChar(a, tmp1);
            a[0] = tmp1;
            a[1] = '\0';
            //c = malloc((sizeof(char)) * 2);
            //appendChar(c, tmp3);
            c[0] = tmp3;
            c[1] = '\0';
            //emptyString = malloc(sizeof(char) * 2);

            if (strcmp(c, "f") != 0 && isFunction == 0) {
                // normal expression => not dealing with assigning a function
                functionName = NULL;
                int row = getTableOffset(a);
                int col = getTableOffset(c);

                if (row == 13 && col == 13) {
                    pop(expendedStack);                     // precedence SA is done, pop stack, there should only be 'E' left
                    tStackPredictiveDispose(stackPredictive);
                    free(stackPredictive);
                    stackPredictive = NULL;
                    break;
                } else if (row > 13 || col > 13) {
                    errorHandling(44);                      // symbol doesn't occur in precedence table
                    end = 1;
                } else {
                    char prec = precTable[row][col];        // get precedence operator from precedence table
                    char *handle = NULL;
                    size_t len;

                    switch (prec) {
                        // decide what to do based on precedence operator obtained from precedence table
                        case '=' :
                            // copy token into the stack
                            //emptyString = malloc()
                            //appendChar(emptyString, tmp3);
                            emptyString[0] = tmp3;
                            emptyString[1] = '\0';
                            push(expendedStack, emptyString);
                            end = 1;                            // need to get next token
                            break;
                        case '<' :
                            // put end rule sign into the stack and push token into the stack
                            pushEndRuleSign(expendedStack, tmp1);
                            //appendChar(emptyString, tmp3);
                            emptyString[0] = tmp3;
                            emptyString[1] = '\0';
                            push(expendedStack, emptyString);
                            if (tmp3 == 'i') {
                                tStackASTPush(stackAST, *node, &token, "", NULL, NULL);
                            }
                            end = 1;                            // need to get next token
                            break;
                        case '>' :
                            // change expresion in the stack
                            if (strrchr(expendedStack->content, '<') == NULL) {
                                handle = malloc(sizeof(char)*2);
                                len = 0;
                            } else {
                                handle = malloc(sizeof(char)*(strlen(strrchr(expendedStack->content, '<'))+1));
                                len = strlen(strrchr(expendedStack->content, '<'));
                            }
                            handle = strcpy(handle,strrchr(expendedStack->content, '<'));
                            handle[len] = '\0';
                            if (strcmp(handle, "") != 0 && changeHandle(expendedStack, handle) != 0) {
                                switch (rule) {
                                    case 1:
                                    case 2:
                                    case 3:
                                        // merging two operands - creating new tree
                                        tStackASTPush(stackAST, *node,&token, a, tStackASTPop(stackAST), tStackASTPop(stackAST));
                                        /*free(a);
                                        free(c);
                                        free(emptyString);*/
                                        break;
                                    case 4:
                                        // doing nothing, just changing handle
                                        break;
                                    default:
                                        break;
                                }
                                // also check if rule exists
                                //fprintf(stdout, "%s\n", handle);    // write rule to stdout
                            } else {
                                errorHandling(42);                 // cannot find the right rule
                                end = 1;
                            }
                            free(handle);
                            break;
                        default:
                            // error occurred
                            errorHandling(43);                     // empty space in precedence table
                            end = 1;
                            break;
                    }
                    /*free(handle);
                    handle=NULL;*/
                }
            } else if (strcmp(c, "f") == 0) {
                // assigning a function in expression => need to load other tokens as well
                char *tmpFuncName = NULL;
                if (functionName != NULL) {
                    tmpFuncName = malloc(sizeof(char) * (strlen(functionName)+1));
                } else {
                    tmpFuncName = malloc(sizeof(char)*1);
                    functionName = malloc(sizeof(char)*1);
                    functionName = strcpy(functionName, "");
                    functionName[0] = '\0';
                }
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                free(functionName);
                functionName = malloc(sizeof(char)*(strlen(tmpFuncName)+strlen(token.content)+1));
                //functionName = catStrings(functionName, tmpFuncName);
                //functionName = catStrings(functionName, token.content);
                functionName = memcpy(functionName, tmpFuncName, strlen(tmpFuncName));
                functionName[strlen(tmpFuncName)] = '\0';
                functionName = strcat(functionName, token.content);
                functionName[strlen(tmpFuncName)+strlen(token.content)] = '\0';
                isFunction = 1;
                simulatePredictive(token, stackPredictive, globalSymtable, node);
                end = 1;
                free(tmpFuncName);
                tmpFuncName = NULL;
            } else if (token.type == s_lbrac || token.type == s_comma || token.type == s_id || token.type == s_int || token.type == s_float || token.type == s_exp_int || token.type == s_exp_int_s || token.type == s_exp_f || token.type == s_exp_f_s || token.type == s_string) {
                // assigning a function in expression => need to load other tokens as well
                char* tmpFuncName = malloc(sizeof(char)*(strlen(functionName))+1);
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                free(functionName);
                functionName = malloc(sizeof(char)*(strlen(tmpFuncName)+strlen(token.content)+1));
                //functionName = catStrings(functionName, tmpFuncName);
                //functionName = catStrings(functionName, token.content);
                functionName = memcpy(functionName, tmpFuncName, strlen(tmpFuncName));
                functionName[strlen(tmpFuncName)] = '\0';
                functionName = strcat(functionName, token.content);
                functionName[strlen(tmpFuncName)+strlen(token.content)] = '\0';
                simulatePredictive(token, stackPredictive, globalSymtable, node);
                end = 1;
                free(tmpFuncName);
                tmpFuncName = NULL;
            } else if (token.type == s_rbrac || token.type == ss_eol || token.type == ss_eof) {
                // assigning a function in expression => need to load other tokens as well
                char* tmpFuncName = malloc(sizeof(char)*(strlen(functionName)+1));
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                free(functionName);
                functionName = malloc(sizeof(char)*(strlen(tmpFuncName)+strlen(token.content)+1));
                //functionName = catStrings(functionName, tmpFuncName);
                //functionName = catStrings(functionName, token.content);
                functionName = memcpy(functionName, tmpFuncName, strlen(tmpFuncName));
                functionName[strlen(tmpFuncName)] = '\0';
                functionName = strcat(functionName, token.content);
                functionName[strlen(tmpFuncName)+strlen(token.content)] = '\0';
                simulatePredictive(token, stackPredictive, globalSymtable, node);
                isFunction = 0;
                tStackPredictiveDispose(stackPredictive);
                stackPredictive = NULL;
                if (token.type == ss_eol || token.type == ss_eof) {
                    Token tmpToken;
                    tmpToken.type = s_func_expr;
                    tmpToken.content = malloc(sizeof(char)*(strlen(functionName)+1));
                    tmpToken.content = strcpy(tmpToken.content, functionName);
                    tmpToken.content[strlen(functionName)] = '\0';
                    simulatePrecedence(tmpToken, expendedStack, stackAST, node, globalSymtable);
                    end = 0;
                    //tStackPredictiveDispose(stackPredictive);
                } else {
                    token.type = s_func_expr;
                    token.content = malloc(sizeof(char)*(strlen(functionName)+1));
                    token.content = strcpy(token.content, functionName);
                }
                free(functionName);
                functionName = NULL;
                free(tmpFuncName);
                tmpFuncName = NULL;
            } else {
                // something went wrong
                errorHandling(2);
                end = 1;
            }
            /*free(a);
            a=NULL;
            free(c);
            c=NULL;
            free(emptyString);
            emptyString=NULL;*/
        } while (end != 1);
        free(a);
        free(c);
        free(emptyString);
    }
}
