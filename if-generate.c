//
// Created by parek on 11/28/18.
//

#include "if-generate.h"
#include "predict.h"

char* myLabel = "$myIfLabel";
char* myEndLabel = "$myIfEndLabel";

void tLabelStackInit(tLabelStack* stack) {
    stack->top = 0;
    for (int i = 0; i < 6; i++) {
        stack->numbers[i] = 0;
    }
}

void tLabelStackPush(tLabelStack* stack, int labelNumber) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->numbers[stack->top] = labelNumber;
        stack->top++;
    }
}

void tLabelStackPop(tLabelStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        if (stack->top > 0) {
            stack->top--;
            stack->numbers[stack->top] = 0;
        }
    }
}

int tLabelStackGetTop(tLabelStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return 0;
    } else {
        return stack->numbers[stack->top-1];
    }
}

void generateIfHead(tASTPointer *AST) {
    if (AST == NULL) {
        errorHandling(99);
    } else {
        if (firstTime == 0) {
            firstTime++;
            labelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(labelStack);
            endLabelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(endLabelStack);
        }
        if (strcmp(AST->ID, "!=") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            printf("JUMPIFEQ %s%d $symb1 $symb2 \n", myLabel, ifLabelNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, "==") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            printf("JUMPIFNEQ %s%d $symb1 $symb2 \n", myLabel, ifLabelNumber);
            tLabelStackPush(endLabelStack, ifEndLabelNumber);
        }
    }
}

void generateIfMid() {
    ifEndLabelNumber++;
    printf("JUMP %s%d \n", myEndLabel, ifEndLabelNumber);
    tLabelStackPush(endLabelStack, ifEndLabelNumber);
    printf("LABEL %s%d \n", myLabel, tLabelStackGetTop(labelStack));
    tLabelStackPop(labelStack);
}

void generateIfEnd() {
    printf("LABEL %s%d \n", myEndLabel, tLabelStackGetTop(endLabelStack));
    tLabelStackPop(endLabelStack);
}

void generateCodeParek(Token* token) {
    if (token == NULL) {
        errorHandling(99);
    } else {
        if (ifStatement != 0) {
            switch (token->type) {
                case kw_if:
                case kw_then:
                    // if-statement header has already been generated
                    break;
                case kw_else:
                    generateIfMid();
                    break;
                case kw_end:
                    generateIfEnd();
                    break;
                default:
                    break;
            }
        }
        if (printing == 1) {
            printf("PRINT %s\n", token->content);
        }
    }
}