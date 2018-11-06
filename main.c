#include <stdio.h>
#include <memory.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "sematnic.h"
#include "prectable.h"
#include "predict.h"
#include "syntax.h"

/*
 * Main file for our compilator.
 * Implemented by Gabriel Quirschfeld, xquirs00
 *                Marek Imrich, ximric01
 *                Marek Varga, xvarga14
 *                Michal Plsek, xplsek03
 */

/**
 * Main function.
 * @return
 */
int main() {
    printf("Hello, World!\n");

    doMagic();

    return ERROR_TYPE;
}