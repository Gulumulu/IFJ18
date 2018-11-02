//
// Created by parek on 11/2/18.
//
#include <stdio.h>
#include "errors.h"

/**
 * Source file for error types
 * Implemented by Marek Varga, xvarga14
 */
void errorHandling(int errorNumber) {
    ERROR_TYPE = errorNumber;

    if (errorNumber == 1) {
        fprintf(stderr, "Error occurred during lexical analysis.\n");
    } else if (errorNumber == 2) {
        fprintf(stderr, "Error occurred during syntax analysis.\n");
    } else if (errorNumber == 3) {
        fprintf(stderr, "Error occurred during semantics analysis. Undefined or attempting to redefine function or variable.\n");
    } else if (errorNumber == 4) {
        fprintf(stderr, "Error occurred during semantics analysis. Incompatible types.\n");
    } else if (errorNumber == 5) {
        fprintf(stderr, "Error occurred during semantics analysis. Wrong number of parameters in function call.\n");
    } else if (errorNumber == 6) {
        fprintf(stderr, "Error occurred during semantics analysis.\n");
    } else if (errorNumber == 9) {
        fprintf(stderr, "Division by zero!!!\n");
    } else if (errorNumber == 99) {
        fprintf(stderr, "Internal compiler error. Sorry\n");
    }
}
