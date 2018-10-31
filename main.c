#include "symtable.h"
#include "prectable.h"
#include <stdio.h>

int main() {
    printf("Hello, World!\n");

    simulatePrecedence("i+i*i$");
    return 0;
}