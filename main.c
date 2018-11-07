#include <stdio.h>
#include <memory.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "sematnic.h"
#include "prectable.h"
#include "predict.h"
#include "syntax.h"

int main() {
    printf("Hello, World!\n");
    FILE *file = fopen("..\\test.txt", "r");

    int count = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    rewind(file);
    for (int i = 0; i <= count; i++) {
        token_generate(file);
    }

    fclose(file);
//    doMagic();

    return ERROR_TYPE;
}