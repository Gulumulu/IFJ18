/**
 * Project for IFJ course - compiler
 * Source file for main.
 *
 * Implemented by: Marek Varga      xvarga14
 */
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "semantic.h"
#include "prectable.h"
#include "predict.h"
#include "syntax.h"
//#include "if-generate.h"

int main() {

    doMagic();

    return showErrorMessage(ERROR_TYPE);
}
