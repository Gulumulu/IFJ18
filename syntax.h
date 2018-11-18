/**
 * Header file for syntax driven compilation
 *
 * Implemented by Gabriel Quirschfeld   xquirs00
 *                Marek Varga           xvarga14
 */
#ifndef IFJ_SYNTAX_H
#define IFJ_SYNTAX_H

#include "errors.h"
#include "prectable.h"
#include "symtable.h"
#include "predict.h"
#include "semantic.h"
#include <string.h>

// structure for stack of functions
typedef struct tFT{
    int top;
    char* function[];
} tFunctionTracker;

// function that does all the work
void doMagic();

// functions for working with stack
void tFunctionTrackerInit(tFunctionTracker* stack);
void tFunctionTrackerDispose(tFunctionTracker* stack);
void tFunctionTrackerPush(tFunctionTracker* stack, char* pushedFunction);
void tFunctionTrackerPop(tFunctionTracker* stack);
char* tFunctionTrackerGetTop(tFunctionTracker* stack);
#endif //IFJ_SYNTAX_H
