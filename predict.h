//
// Created by parek on 11/5/18.
//
/**
 * Header file for predictive syntax analysis.
 * Implemented by Marek Varga, xvarga14
 */
#ifndef IFJ_PREDICT_H
#define IFJ_PREDICT_H

#include "errors.h"
#include "semantic.h"
#include "prectable.h"
#include "scanner.h"
#include <string.h>
// stack for predictive SA
typedef struct {
    int top;
    char* content[];
} tStackPredictive;

int rulesApplied[50];

// functions for working with stack
void tStackPredictiveInit(tStackPredictive* stack);
void tStackPredictiveDispose(tStackPredictive* stack);
void tStackPredictivePush(tStackPredictive* stack, char* symbol);
void tStackPredictivePop(tStackPredictive* stack);
char* tStackPredictiveGetTop(tStackPredictive* stack);
void tStackPredictiveChangeTop(tStackPredictive* stack, int ruleNumber);

// main function
void simulatePredictive(Token token, tStackPredictive* predictiveStack);

// helper functions
int rowOffset(char* symbol);
int colOffset(TokenType symbol);
int isTerminal(char* symbol);
void fillRulesApplied(int rule);
#endif //IFJ_PREDICT_H
