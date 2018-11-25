//
// Created by root on 11/23/18.
//

#include "queue.h"
#include "semantic.h"

#ifndef IFJ18_GENERATE_H
#define IFJ18_GENERATE_H

void operation_assign(tASTPointer* Root);
void postorder(tASTPointer* Root, tQueue* q);
void generateExpression(tASTPointer* AST);

#endif //IFJ18_GENERATE_H
