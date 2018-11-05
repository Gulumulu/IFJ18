//
// Created by parek on 10/31/18.
//
/**
 * Implemented by Marek Varga, xvarga14
 * Header for precedence table
 */
#ifndef IFJ_PRECTABLE_H
#define IFJ_PRECTABLE_H

#include "sematnic.h"
#include "errors.h"

int rule;

// structure for extended stack
typedef struct {
    int top;
    char* content;
} tExpendedStack;

// functions for working with stack
void init(tExpendedStack* stack);
void dispose(tExpendedStack* stack);
void push(tExpendedStack* stack, char* c);
void pop(tExpendedStack* stack);
void pushEndRuleSign(tExpendedStack* stack, char firstChar);
void changeHandle(tExpendedStack* stack, char* handle);
void applyRule(tExpendedStack* stack, char* handle,char* rule);

// helper functions
void checkMalloc(const char* checkedString);
int getTableOffset(char* terminal);
void simulatePrecedence(char* inputToken, tASTPointer* AST);
char getTop(tExpendedStack* stack);
char* appendChar(char *string, char addedChar);
#endif //IFJ_PRECTABLE_H
