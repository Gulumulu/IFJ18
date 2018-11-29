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

void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, char* list_str);
void generateExpression(tASTPointer* AST,tFunctionTracker* functionTracker, char* list_str);
void generateCode(char* stackTop, int rules[]);

void type_control(tASTPointer* Root, char* operation, tQueue* q, char* frame, char* list_str);

void generate_to_list(int ad, int* ptr_list_length, int* ptr_dyn_length, char* list_str);
void generate_to_list2(int ad,char* str);

char* name_parse(char* str);
char* number_parse(char* str);
char* ord_parse(char* str);
char* susbtr_parse(char* str);

#endif //IFJ18_GENERATE_H
