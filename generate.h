//
// Created by root on 11/23/18.
//

#include "queue.h"
#include "semantic.h"
#include "syntax.h"

#ifndef IFJ18_GENERATE_H
#define IFJ18_GENERATE_H

static int assign = 1; // pocitadlo assign zacina na 1

char* convert_string(char* str);
char* get_frame(tFunctionTracker* functionTracker);
//void operation_assign(tASTPointer* Root, tFunctionTracker* functionTracker, FILE* list);
void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, FILE* list);
void generateExpression(tASTPointer* AST,tFunctionTracker* functionTracker, FILE* list);
void generateCode(char* stackTop, int rules[]);

void type_control(tASTPointer* Root, char* operation, tQueue* q, char* frame, FILE* list);

char* name_parse(char* str);
char* number_parse(char* str);
char* ord_parse(char* str);
char* susbtr_parse(char* str);

#endif //IFJ18_GENERATE_H
