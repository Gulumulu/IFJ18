//
// Created by root on 11/23/18.
//

#include "queue.h"
#include "semantic.h"
#include "syntax.h"

#ifndef IFJ18_GENERATE_H
#define IFJ18_GENERATE_H

char* convert_string(char* str, FILE* list);
char* get_frame(tFunctionTracker* functionTracker);
void operation_assign(tASTPointer* Root, tFunctionTracker* functionTracker, FILE* list);
void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, FILE* list);
void generateExpression(tASTPointer* AST,tFunctionTracker* functionTracker, FILE* list);

void type_control(tASTPointer* Root, char* operation, tQueue* q, char* frame, FILE* list);

#endif //IFJ18_GENERATE_H
