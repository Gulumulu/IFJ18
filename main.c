#include <stdio.h>
#include "symtable.h"
#include "prectable.h"
#include "errors.h"
#include "scanner.h"

int main() {
    printf("Hello, World!\n");

    // if lexical analysis passed ERROR_TYPE should still be 0
    if (ERROR_TYPE == 0) {
        simulatePrecedence("i+i*i$");
    }

    return ERROR_TYPE;
}