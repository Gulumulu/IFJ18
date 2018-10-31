//
// Created by parek on 10/31/18.
//
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
        fprintf(stderr, "Stack error. Not enough space to perform malloc!");
        return;
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
 * Function "pushes" new element into the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack that will contain new element
 * @param c pointer to char is pushed element
 */
void push(tExpendedStack* stack, char* c) {
    char* tmp = malloc(strlen(stack->content)+strlen(c));
    checkMalloc(tmp);
    strcpy(tmp, stack->content);
    stack->content = malloc(strlen(tmp)+strlen(c));
    checkMalloc(stack->content);
    if (strcmp(c, "<=") == 0) {     // in case we are pushing two chars
        strcat(tmp,".");
    } else if (strcmp(c, ">=") == 0){
        strcat(tmp,",");
    } else if (strcmp(c, "==") == 0){
        strcat(tmp,"?");
    } else if (strcmp(c, "!=") == 0){
        strcat(tmp,"!");
    } else if (strcmp(c, ">") == 0){
        strcat(tmp,"g");
    } else if (strcmp(c, "<") == 0){
        strcat(tmp,"l");
    } else {
        strcat(tmp, c);
    }
    strcpy(stack->content, tmp);
    stack->top++;
}

/**
 * Function "pops" top element from the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack that will be removed of element
 */
void pop(tExpendedStack* stack) {
    if (stack->top == 0) {
        fprintf(stderr, "Stack error. Trying to pop empty stack!");
        return;
    } else {
        stack->top--;
        char* tmp = malloc(strlen(stack->content)-1);
        checkMalloc(tmp);
        memcpy(tmp, stack->content, (size_t) stack->top);
        stack->content = malloc(strlen(tmp));
        checkMalloc(stack->content);
        memcpy(stack->content, tmp, strlen(tmp));
        //printf("%s",tmp);
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
    } else if (strcmp(terminal, "<") == 0) {
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

/**
 * Function returns first terminal from top of the stack.
 *
 * @param stack pointer to tExpendedStack structure is stack from which terminal is sought
 * @return char of first terminal
 */
char getTop(tExpendedStack* stack) {
    if (stack != NULL) {
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
    } else {
        fprintf(stderr, "Stack error. Unable to get top terminal from stack.");
        return 99;
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
    strcpy(returnedString, string);
    returnedString[len] = addedChar;
    returnedString[len+1] = '\0';

    return returnedString;
}

/**
 * Function simulates operator precedence look up for given token.
 *
 * @param inputToken pointer to char is input token string
 */
void simulatePrecedence(char* inputToken) {
    tExpendedStack* stack;
    stack = malloc(sizeof(tExpendedStack));

    if (stack == NULL) {                        // stack error
        fprintf(stderr, "Stack error. Not enough space to initialize stack!");
        //return 99;
    } else {
        init(stack);

        int tokenOffset = 0;                    // offset by which to look into the input token
        char* a;
        char* b;
        char* emptyString = "";
        do {
            a = "";
            b = "";
            char tmp1 = getTop(stack);
            char tmp2 = inputToken[tokenOffset++];

            /*size_t len = strlen(a);
            char* a2 = malloc(len + 2);
            strcpy(a2,a);
            a2[len] = tmp1;
            a2[len+1] = '\0';

            len = strlen(b);
            char* b2 = malloc(len + 2);
            strcpy(b2,b);
            b2[len] = tmp2;
            b2[len+1] = '\0';*/
            /*char* a2 = malloc(strlen(a) + 2);
            char* b2 = malloc(strlen(b) + 2);*/
            a = appendChar(a, tmp1);
            b = appendChar(b, tmp2);

            int row = getTableOffset(a);
            int col = getTableOffset(b);
            char prec = precTable[row][col];
            char* handle;

            switch (prec) {
                case '=' :
                    push(stack, appendChar(emptyString, tmp2));
                    break;
                case '<' :
                    push(stack, "|");
                    push(stack, appendChar(emptyString, tmp2));
                    break;
                case '>' :
                    handle = strrchr(stack->content, '|');
                    if (handle != NULL) {           // also check if rule exists
                        // todo: change '<y' a.k.a '|handle' to 'E'
                        fprintf(stdout, "%s\n", handle);    // write rule to stdout
                    } else {
                        fprintf(stderr, "Cannot find the appropriate rule.");
                        // return 99;
                    }
                    break;
                default:
                    fprintf(stderr, "Empty space in precedence table.");
                    //return 99;
                    break;
            }
        } while (strcmp(a, "$") != 0 || strcmp(b, "$") != 0);
    }
}