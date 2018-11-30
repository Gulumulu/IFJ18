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
char* myTmpVariable = "$myTmpVariable";
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
            tmpVariables = malloc(sizeof(tLabelStack));
            tLabelStackInit(tmpVariables);
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
        } else if (strcmp(AST->ID, "<") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("LT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFNEQ %s%d TF@%s%d bool@true \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("AND TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFEQ %s%d TF@%s%d $symb2 \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, ">") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("GT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFNEQ %s%d TF@%s%d bool@true\n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, ">=") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("OR TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFEQ %s%d TF@%s%d $symb1 \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber);
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
 * Function generates end to if statement.
 */
void generateIfEnd() {
    printf("LABEL %s%d \n", myIfEndLabel, tLabelStackGetTop(endLabelStack));
    tLabelStackPop(endLabelStack);
}

/**
 * Function generates head for while loop.
 *
 * @param AST structure tASTPointer is pointer to AST
 */
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
            printf("JUMPIFNEQ %s%d $symb1 $symb2 \n", myWhileEndLabel, whileEndLabelNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            printf("LABEL %s%d \n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("LT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFNEQ %s%d TF@%s%d bool@true \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            printf("LABEL %s%d\n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("AND TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFEQ %s%d TF@%s%d $symb2 \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, ">") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            printf("LABEL %s%d \n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("GT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFNEQ %s%d TF@%s%d bool@true \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            printf("LABEL %s%d\n", myWhileLabel, whileLabelNumber);
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            printf("DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber);
            printf("OR TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber);
            printf("JUMPIFEQ %s%d TF@%s%d $symmb1 \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber);
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        }
    }
}

/**
 * Function generates an ending for while loop.
 */
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
void generatePrint(Token* token, char* currentFunction) {
    if (token == NULL) {
        errorHandling(99);
    } else {
        if (token->type == s_string) {
            printf("WRITE string@%s\n", token->content);
        } else if (token->type == s_int) {
            printf("WRITE int@%s\n", token->content);
        } else if (token->type == s_float) {
            printf("WRITE float@%s\n", token->content);
        } else if (token->type == s_id) {
            if (strcmp(currentFunction, "Main") == 0) {
                printf("WRITE GF@%s\n", token->content);
            } else {
                printf("WRITE LF@%s\n", token->content);
            }
        }
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