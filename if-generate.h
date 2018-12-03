/**
 *  Header file for generating if and while statements.
 *  Implemented by: Marek Varga             xvarga14
 *                  Gabriel Quirschfeld     xquirs00
 */
#ifndef IFJ_IF_GENERATE_H
#define IFJ_IF_GENERATE_H

#include "scanner.h"
#include "semantic.h"
#include <stdio.h>

// stack for if labels
typedef struct tLabelStack {
    int top;
    int numbers[100];
} tLabelStack;

tLabelStack* labelStack;         // stack for if labels
tLabelStack* endLabelStack;      // stack for if end labels
tLabelStack* tmpVariables;      // stack for tmp variables

float str2fl(char* str);
char* str2str(char * str);

// helper vars
int firstTime;

int ifLabelNumber;
int ifEndLabelNumber;
int ifStatement;

int whileLabelNumber;
int whileEndLabelNumber;
int whileStatement;

int tmpVariableNumber;

// functions for generating
void generateCodeParek(Token* token);
void generateIfHead(tASTPointer *AST);
void generateIfMid();
void generateIfEnd();
void generatePrint(Token* token, char* currentFunction);

void generateWhileHead(tASTPointer *AST);
void generateWhileEnd();

// functions for working with stack
void tLabelStackInit(tLabelStack* stack);
void tLabelStackPush(tLabelStack* stack, int labelNumber);
void tLabelStackPop(tLabelStack* stack);
int tLabelStackGetTop(tLabelStack* stack);
#endif //IFJ_IF_GENERATE_H
