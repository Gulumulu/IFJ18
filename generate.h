/**
 * Header file for code generation
 *
 * Implemented by Gabriel Quirschfeld   xquirs00
 *                Marek Varga           xvarga14
 *                Michal Plsek          xplsek03
 */

#include "queue.h"
#include "semantic.h"
#include "syntax.h"
#include "scanner.h"
#include <stdio.h>
#include "symtable.h"

#ifndef IFJ18_GENERATE_H
#define IFJ18_GENERATE_H

static int assign = 1; // pocitadlo assign zacina na 1

char* convert_string(char* str);
char* get_frame(tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken);

void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken, char* list_str);
void generateExpression(tASTPointer* AST, tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken, char* list_str, bool comp);
void generateCode(char* stackTop, int rules[],char* list_str, tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken);

void type_control(tASTPointer* Root, char* operation, tQueue* q, char* frame, char* list_str);

void generate_to_list2(int ad);

void call_function(int id, char* frame, tASTPointer* Root, char* list_str);

char* name_parse(char* str);
char* number_parse(char* str);
char* ord_parse(char* str);
char* susbtr_parse(char* str);

// stack for if labels
typedef struct tLabelStack {
    int top;
    int numbers[100];
} tLabelStack;

tLabelStack* labelStack;         // stack for if labels
tLabelStack* endLabelStack;      // stack for if end labels
tLabelStack* tmpVariables;      // stack for tmp variables

double str2fl(char* str);
char* str2str(char * str);

void args_to_frame(char* name, char* frame);
char* parse_func_args(char* str);
char* parse_func_name(char* str);

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
void generateIfHead(tASTPointer *AST, tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken);
void generateIfMid();
void generateIfEnd();
void generatePrint(Token* token, char* currentFunction);

void generateWhileHead(tASTPointer *AST,tFunctionTracker* functionTracker, struct BSTNode **array, BSTNodePtr* global_symtable, Token tmpToken);
void generateWhileEnd();

// functions for working with stack
void tLabelStackInit(tLabelStack* stack);
void tLabelStackPush(tLabelStack* stack, int labelNumber);
void tLabelStackPop(tLabelStack* stack);
int tLabelStackGetTop(tLabelStack* stack);

#endif //IFJ18_GENERATE_H
