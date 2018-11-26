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
    stack->content = "$";
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
            //free(stack->content);
            stack->content = NULL;
        }
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
        char *tmp = malloc(strlen(stack->content) + strlen(c));
        checkMalloc(tmp);
        strcpy(tmp, stack->content);
        stack->content = malloc(strlen(tmp) + strlen(c));
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
        strcpy(stack->content, tmp);
        stack->top++;
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
        char* tmp = malloc(strlen(stack->content));
        checkMalloc(tmp);
        tmp = memcpy(tmp, stack->content, strlen(stack->content)-1);
        tmp[strlen(stack->content)-1] = '\0';
        stack->content = malloc(strlen(tmp)+1);
        checkMalloc(stack->content);
        stack->content = memcpy(stack->content, tmp, strlen(tmp));
        stack->content[strlen(tmp)] = '\0';
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
char* appendChar(char* string, char addedChar) {
    size_t len = strlen(string);
    char* returnedString = malloc(len + 2);
    checkMalloc(returnedString);
    strcpy(returnedString, string);
    returnedString[len] = addedChar;
    returnedString[len+1] = '\0';

    return returnedString;
}

char* catStrings(char* string1, char* string2) {
    char* returnedString = malloc(strlen(string1)+strlen(string2)+1);
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
        char* rest = malloc(strlen(strrchr(stack->content, firstChar))+1);
        rest = strrchr(stack->content, firstChar);
        //rest[strlen(strrchr(stack->content, firstChar))] = '\0';
        char *beginning = malloc(strlen(stack->content) + 2);
        memcpy(beginning, stack->content, (strlen(stack->content) - strlen(rest)) + 1);
        beginning[(strlen(stack->content) - strlen(rest)) + 1] = '\0';
        beginning = appendChar(beginning, '<');
        for (int i = 0; i < (int) strlen(rest) - 1; i++) {
            rest[i] = rest[i + 1];
        }
        if (strlen(rest) != 1) {
            rest[strlen(rest)-1] = '\0';
            strcat(beginning, rest);
        } else {
            strcat(beginning, "\0");
        }
        stack->content = malloc(strlen(beginning)+1);
        stack->content = memcpy(stack->content, beginning, strlen(beginning));
        stack->content[strlen(beginning)] = '\0';
        //stack->content = beginning;
        free(beginning);
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
        tmpStackContent = malloc(strlen(stack->content) - strlen(handle) + strlen(rule)+1);
        //tmpStackContent = malloc(10);
        //tmpStackContent = memmove(tmpStackContent, stack->content, strlen(stack->content) - strlen(handle));
        tmpStackContent = memcpy(tmpStackContent, stack->content, strlen(stack->content) - strlen(handle));
        tmpStackContent[strlen(stack->content) - strlen(handle)] = '\0';
        tmpStackContent = strcat(tmpStackContent, rule);
        //tmpStackContent = appendChar(tmpStackContent, 'E');
        stack->top -= (strlen(handle) - 1);
        free(stack->content);
        stack->content = malloc(strlen(tmpStackContent)+1);
        stack->content = strcpy(stack->content, tmpStackContent);
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
        char* a;
        char* c;
        char* emptyString = "";
        int end = 0;
        if (functionName == NULL || strcmp(functionName, "") == 0) {
            functionName = "";
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
            a = "";
            c = "";
            char tmp1 = getTop(expendedStack);
            char tmp3 = changeTokenTypeToChar(token.type);

            a = appendChar(a, tmp1);
            c = appendChar(c, tmp3);

            if (strcmp(c, "f") != 0 && isFunction == 0) {
                // normal expression => not dealing with assigning a function
                functionName = NULL;
                int row = getTableOffset(a);
                int col = getTableOffset(c);

                if (row == 13 && col == 13) {
                    pop(expendedStack);                     // precedence SA is done, pop stack, there should only be 'E' left
                    tStackPredictiveDispose(stackPredictive);
                    break;
                } else if (row > 13 || col > 13) {
                    errorHandling(44);                      // symbol doesn't occur in precedence table
                    end = 1;
                } else {
                    char prec = precTable[row][col];        // get precedence operator from precedence table
                    char *handle = NULL;

                    switch (prec) {
                        // decide what to do based on precedence operator obtained from precedence table
                        case '=' :
                            // copy token into the stack
                            push(expendedStack, appendChar(emptyString, tmp3));
                            end = 1;                            // need to get next token
                            break;
                        case '<' :
                            // put end rule sign into the stack and push token into the stack
                            pushEndRuleSign(expendedStack, tmp1);
                            push(expendedStack, appendChar(emptyString, tmp3));
                            if (tmp3 == 'i') {
                                tStackASTPush(stackAST, makeLeaf(findVariable(*node, &token)));
                            }
                            end = 1;                            // need to get next token
                            break;
                        case '>' :
                            // change expresion in the stack
                            handle = malloc(strlen(strrchr(expendedStack->content, '<')));
                            handle = strcpy(handle,strrchr(expendedStack->content, '<'));
                            if (handle != NULL && changeHandle(expendedStack, handle) != 0) {
                                switch (rule) {
                                    case 1:
                                    case 2:
                                    case 3:
                                        // merging two operands - creating new tree
                                        tStackASTPush(stackAST,
                                                      makeTree(a, tStackASTPop(stackAST), tStackASTPop(stackAST)));
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
                }
            } else if (strcmp(c, "f") == 0) {
                // assigning a function in expression => need to load other tokens as well
                char* tmpFuncName = malloc(strlen(functionName));
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                functionName = malloc(strlen(functionName)+strlen(token.content)+1);
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
            } else if (token.type == s_lbrac || token.type == s_comma || token.type == s_id || token.type == s_int || token.type == s_float || token.type == s_exp_int || token.type == s_exp_int_s || token.type == s_exp_f || token.type == s_exp_f_s || token.type == s_string) {
                // assigning a function in expression => need to load other tokens as well
                char* tmpFuncName = malloc(strlen(functionName));
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                functionName = malloc(strlen(functionName)+strlen(token.content)+1);
                //functionName = catStrings(functionName, tmpFuncName);
                //functionName = catStrings(functionName, token.content);
                functionName = memcpy(functionName, tmpFuncName, strlen(tmpFuncName));
                functionName[strlen(tmpFuncName)] = '\0';
                functionName = strcat(functionName, token.content);
                functionName[strlen(tmpFuncName)+strlen(token.content)] = '\0';
                simulatePredictive(token, stackPredictive, globalSymtable, node);
                end = 1;
                free(tmpFuncName);
            } else if (token.type == s_rbrac || token.type == ss_eol || token.type == ss_eof) {
                // assigning a function in expression => need to load other tokens as well
                char* tmpFuncName = malloc(strlen(functionName));
                strcpy(tmpFuncName, functionName);
                tmpFuncName[strlen(functionName)] = '\0';
                functionName = malloc(strlen(functionName)+strlen(token.content)+1);
                //functionName = catStrings(functionName, tmpFuncName);
                //functionName = catStrings(functionName, token.content);
                functionName = memcpy(functionName, tmpFuncName, strlen(tmpFuncName));
                functionName[strlen(tmpFuncName)] = '\0';
                functionName = strcat(functionName, token.content);
                functionName[strlen(tmpFuncName)+strlen(token.content)] = '\0';
                simulatePredictive(token, stackPredictive, globalSymtable, node);
                isFunction = 0;
                tStackPredictiveDispose(stackPredictive);
                if (token.type == ss_eol || token.type == ss_eof) {
                    Token tmpToken;
                    tmpToken.type = s_func_expr;
                    tmpToken.content = malloc(strlen(functionName)+1);
                    tmpToken.content = strcpy(tmpToken.content, functionName);
                    tmpToken.content[strlen(functionName)] = '\0';
                    simulatePrecedence(tmpToken, expendedStack, stackAST, node, globalSymtable);
                    end = 0;
                } else {
                    token.type = s_func_expr;
                    token.content = malloc(strlen(functionName)+1);
                    token.content = strcpy(token.content, functionName);
                }
                free(functionName);
                free(tmpFuncName);
            } else {
                // something went wrong
                errorHandling(2);
                end = 1;
            }
        } while (end != 1);
    }
}
