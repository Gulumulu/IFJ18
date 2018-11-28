#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "semantic.h"
#include "prectable.h"
#include "predict.h"
#include "syntax.h"
#include "if-generate.h"

int main() {

    /*FILE *file = fopen("test.txt", "r");

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

    fclose(file);*/
    doMagic();

    return showErrorMessage(ERROR_TYPE);
}
