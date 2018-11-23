//
// Created by root on 11/23/18.
//

#include "generate.h"
#include "semantic.h"
#include "queue.h"
#include <stdlib.h>

static int counter = 1; // zaciname na %1. Je reset counteru potreba, kdyz to pobezi cely program?

void operation_assign(tASTPointer* Root) { // operace prirazeni do promenne, pro strom o velikosti 1 pouze
    printf("DEFVAR %%assign\n");
    printf("MOVE %%assign %s@%s\n",Root->content->type,Root->content->name);
}

void postorder(tASTPointer* Root, tQueue* q) { // rekurzivni postorder pro postupne generovani vyrazu v generate_expression(AST)

    if (Root == NULL)
        return;
    postorder(Root->LeftPointer,q);
    postorder(Root->RightPointer,q);

    // PROCESSING SINGLE NODE

    char* op;
    if(!strcmp(Root->ID,"+"))
        op = "ADD";
    else if(!strcmp(Root->ID,"-"))
        op = "SUB";
    else if(!strcmp(Root->ID,"*"))
        op = "MUL";
    else if(!strcmp(Root->ID,"/"))
        op = "DIV";
    else
        goto not_operator;

    printf("DEFVAR %%%i\n", counter); // operace, chystam tedy novou promennou
    int leftvar; // leva strana
    int rightvar; // prava strana

    if(Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name != NULL) { // ani jeden z L R neni operace
        printf("%s @%%%i %s@%s %s@%s\n", op, counter, Root->LeftPointer->content->type, Root->LeftPointer->content->name, Root->RightPointer->content->type, Root->RightPointer->content->name);
    }
    else if((Root->LeftPointer->content->name == NULL && Root->RightPointer->content->name != NULL) || (Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name == NULL)) { // jeden z L R je operace
        if(Root->LeftPointer->content->name == NULL) { // L je operator
            queueGet(q, &leftvar);
            printf("%s @%%%i @%%%i %s@%s\n", op, counter, leftvar, Root->RightPointer->content->type, Root->RightPointer->content->name);
        }
        else { // R je operator
            queueGet(q, &rightvar);
            printf("%s @%%%i %s@%s @%%%i\n", op, counter, Root->LeftPointer->content->type, Root->LeftPointer->content->name,rightvar);
        }
    }
    else { // L R jsou operatory
        queueGet(q, &leftvar);
        queueGet(q, &rightvar);
        printf("%s @%%%i @%%%i @%%%i\n", op, counter, leftvar,rightvar);
    }

    queueUp(q,counter); // nahravas do fronty pokazde kdyz delas vyraz, kde je root operator

    counter++; // pricti 1 k promenne

    not_operator: ; // KILL ME PLEASE :-D OPTIMALIZACE

}

void generateExpression(tASTPointer* AST) {
    if(AST->LeftPointer == NULL && AST->RightPointer == NULL) {// jedna se pouze o assign jednoducheho typu x = 1
        operation_assign(AST);
    }
    tQueue* q = malloc(sizeof(tQueue)); // nova fronta pro generate_expression
    queueInit(q); // inicializuj frontu
    postorder(AST,q); // rekurzivni postorder stromem
    free(q); // uvolni frontu
}

