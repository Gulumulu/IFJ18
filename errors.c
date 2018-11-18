/**
 * Source file for error types
 * Implemented by Marek Varga, xvarga14
 */
#include <stdio.h>
#include "errors.h"

/**
 * Function sets ERROR_TYPE according to error number.
 *
 * @param errorNumber error number occurred during compilation
 */
void errorHandling(int errorNumber) {
    if (ERROR_TYPE == 0) {
        ERROR_TYPE = errorNumber;
    }
}

/**
 * Function displays appropriate error message according to error number.
 *
 * @param errorNumber error number occurred during compilation
 * @return error code for compilation result
 */
int showErrorMessage(int errorNumber) {
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
        fprintf(stderr, "Internal compiler error. Sorry :(\n");
    } else if (errorNumber == 40) {
        fprintf(stderr, "Error occurred during syntax analysis. Symbol doesn't occur in LL rule table.\n");
        ERROR_TYPE = 2;
    } else if (errorNumber == 41) {
        fprintf(stderr, "Error occurred during syntax analysis. No such rule for given symbol.\n");
        ERROR_TYPE = 2;
    } else if (errorNumber == 42) {
        fprintf(stderr, "Error occurred during syntax analysis. Cannot find the appropriate rule for current handle in operator precedence.\n");
        ERROR_TYPE = 2;
    } else if (errorNumber == 43) {
        fprintf(stderr, "Error occurred during syntax analysis. Empty space in precedence table.\n");
        ERROR_TYPE = 2;
    } else if (errorNumber == 44) {
        fprintf(stderr, "Error occurred during syntax analysis. Symbol doesn't occur in precedence table.\n");
        ERROR_TYPE = 2;
    } else if (errorNumber == 45) {
        fprintf(stderr, "Error occurred during syntax analysis. Wrong number of function arguments.\n");
        ERROR_TYPE = 2;
    }

    return ERROR_TYPE;
}