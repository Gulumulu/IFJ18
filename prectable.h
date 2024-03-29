/**
 * Project for IFJ course - compiler
 * Header for precedence table
 *
 * Implemented by Marek Varga, xvarga14
 */
#ifndef IFJ_PRECTABLE_H
#define IFJ_PRECTABLE_H

#include "semantic.h"
#include "errors.h"
#include "scanner.h"
#include "predict.h"

int rule;
int isFunction;
char* functionName;
tStackPredictive* stackPredictive;

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
int changeHandle(tExpendedStack* stack, char* handle);
void applyRule(tExpendedStack* stack, char* handle,char* rule);

// helper functions
void checkMalloc(const char* checkedString);
int getTableOffset(char* terminal);
void simulatePrecedence(Token token, tExpendedStack* expendedStack, tStackASTPtr* stackAST, BSTNodePtr* node, BSTNodePtr* globalSymtable);
char getTop(tExpendedStack* stack);
void appendChar(char *string, char addedChar);
TokenType decideID(Token nextToken);
#endif //IFJ_PRECTABLE_H
