/**
 *  Source file for generating if and while statements.
 *  Implemented by: Marek Varga             xvarga14
 *                  Gabriel Quirschfeld     xquirs00
 */

#include "if-generate.h"
#include "predict.h"

char* myIfLabel = "$myIfLabel";
char* myIfEndLabel = "$myIfEndLabel";
char* myWhileLabel = "$myWhileLabel";
char* myWhileEndLabel = "$myWhileEndLabel";

/**
 * Function initializes stack to store label numbers.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 */
void tLabelStackInit(tLabelStack* stack) {
    stack->top = 0;
    for (int i = 0; i < 100; i++) {
        stack->numbers[i] = 0;
    }
}

/**
 * Function pushes label number in the stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 * @param labelNumber int number pushed in the stack
 */
void tLabelStackPush(tLabelStack* stack, int labelNumber) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->numbers[stack->top] = labelNumber;
        stack->top++;
    }
}

/**
 * Function pop label number from stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 */
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

/**
 * Function retrieves top label number from stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 * @return int label number from stack top
 */
int tLabelStackGetTop(tLabelStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return 0;
    } else {
        return stack->numbers[stack->top-1];
    }
}

/**
 * Function generates head for if statement.
 *
 * @param AST structure tASTPointer is pointer to AST
 */
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
            printf("JUMPIFEQ %s%d $symb1 $symb2 \n", myIfLabel, ifLabelNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, "==") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            printf("JUMPIFNEQ %s%d $symb1 $symb2 \n", myIfLabel, ifLabelNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        }
    }
}

/**
 * Function generates middle section for if statement.
 */
void generateIfMid() {
    ifEndLabelNumber++;
    printf("JUMP %s%d \n", myIfEndLabel, ifEndLabelNumber);
    tLabelStackPush(endLabelStack, ifEndLabelNumber);
    printf("LABEL %s%d \n", myIfLabel, tLabelStackGetTop(labelStack));
    tLabelStackPop(labelStack);
}

/**
 * Function generates end to if statement
 */
void generateIfEnd() {
    printf("LABEL %s%d \n", myIfEndLabel, tLabelStackGetTop(endLabelStack));
    tLabelStackPop(endLabelStack);
}

void generateWhileHead(tASTPointer *AST) {
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
            whileLabelNumber++;
            printf("LABEL %s%d \n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("JUMPIFEQ %s%d $symb1 $symb2 \n", myWhileEndLabel, whileEndLabelNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "==") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            printf("LABEL %s%d \n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("JUMPIFEQ %s%d $symb1 $symb2 \n", myWhileEndLabel, whileEndLabelNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        }
    }
}

void generateWhileEnd() {
    printf("JUMP %s%d \n", myWhileLabel, tLabelStackGetTop(labelStack));
    tLabelStackPop(labelStack);
    printf("LABEL %s%d \n", myWhileEndLabel, tLabelStackGetTop(endLabelStack));
    tLabelStackPop(endLabelStack);
}

/**
 * Function generates print in IFJcode18
 * @param token input token
 */
void generatePrint(Token* token) {
    if (token == NULL) {
        errorHandling(99);
    } else {
        printf("WRITE %s\n", token->content);
    }
}

/**
 * Function for deciding which generate function will be called.
 *
 * @param token input token
 */
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
        if (whileStatement != 0) {
            if (token->type == kw_end) {
                generateWhileEnd();
            }
        }
    }
}