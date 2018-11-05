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
#include "sematnic.h"
#include "prectable.h"

// stack for predictive SA
typedef struct {
    int top;
    char* content[];
} tStackPredictive;

// functions for working with stack
void tStackPredictiveInit(tStackPredictive* stack);
void tStackPredictiveDispose(tStackPredictive* stack);
void tStackPredictivePush(tStackPredictive* stack, char* symbol);
char* tStackPredictiveGetTop(tStackPredictive* stack);

// main function
void simulatePredictive(char* inputToken, tASTPointer* AST);

// helper functions
int rowOffset(char* symbol);
int colOffset(char* symbol);

#endif //IFJ_PREDICT_H
