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

void type_control(tASTPointer* Root,char* operation) {

        bool left = false; // false = neni promenna
        bool right = false; // false = neni promenna

        if(!strcmp(Root->LeftPointer->content->type,"variable")) // leva strana je VAR
            left = true;

        printf("DEFVAR @$type_%s\n", Root->LeftPointer->content->name);
        printf("DEFVAR @$temp_%s\n", Root->LeftPointer->content->name);
        printf("TYPE @$type_%s @%s,\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);
        printf("MOVE @$temp_%s @%s,\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);

        if(!strcmp(Root->RightPointer->content->type,"variable")) // prava strana je VAR
            right = true;

        printf("DEFVAR @$type_%s\n", Root->RightPointer->content->name);
        printf("DEFVAR @$temp_%s\n", Root->RightPointer->content->name);
        printf("TYPE @$type_%s @%s,\n", Root->RightPointer->content->name, Root->RightPointer->content->name);
        printf("MOVE @$temp_%s @%s,\n", Root->RightPointer->content->name, Root->RightPointer->content->name);

        // NEXT - rozdel na templ a tempr, kdyby byly stejny hodnoty
        // bacha na stejne nazvy v ramci ramce, radsi bych na kazdy prubeh vytvoril solo ramec

        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(left && right) { // obe strany jsou promenna
                printf("JUMPIFNEQ $label_left_not_int $type_%s string@int\n",Root->LeftPointer->content->name); // skoc pokud je levy jiny nez int
                printf("JUMPIFEQ $label_same_types $type_%s string@int\n",Root->RightPointer->content->name); // levy je int, otestuj pravy na int
                printf("JUMPIFNEQ $label_error @$type_%s string@float\n",Root->RightPointer->content->name); // pokud pravy neni ani float, chyba
                printf("INT2FLOAT @$temp_%s @%s\n",Root->LeftPointer->content->name, Root->LeftPointer->content->name); // pravy je float, levy preved na float
                printf("JUMP $label_same_types\n"); // skoc na konec

                printf("LABEL $label_left_not_int\n"); // levy nebyl int
                printf("JUMPIFNEQ $label_error @$type_%s string@float\n",Root->LeftPointer->content->name); // zkus jestli neni float, jestli ne tak chyba
                printf("JUMPIFEQ $label_same_types @$type_%s string@float\n",Root->RightPointer->content->name); // je to float, otestuj jestli neni druha taky float
                printf("JUMPIFNEQ $label_error @$type_%s string@int\n",Root->RightPointer->content->name); // otestuj jestli neni druha int
                printf("INT2FLOAT @$temp_%s @%s\n",Root->RightPointer->content->name, Root->RightPointer->content->name); // druha je int, preved na float
                printf("JUMP $label_same_types\n");

                printf("LABEL $label_error\n"); // chyba typu
                printf("ERROR int@4\n");
                printf("LABEL $label_same_types\n"); // operaci je mozne provest s $temp_%s na obou stranach
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int
                if(strcmp(Root->LeftPointer->content->type,Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                    if(!strcmp(Root->LeftPointer->content->type,"int")) { // pokud je vlevo int, preved ho na float
                        printf("INT2FLOAT @$temp_%s %s@%s\n",Root->LeftPointer->content->type,Root->LeftPointer->content->name);
                    }
                    else { // int je vpravo, preved ho na float
                        printf("INT2FLOAT @$temp_%s %s@%s\n",Root->RightPointer->content->type,Root->RightPointer->content->name);
                    }
                }
            } // operaci je mozne provest s $temp_%s na obou stranach

            else { // jedna ze stran je konstanta

                char* var; // replacement name leve nebo prave var, podle toho kde byla
                tASTPointer* cons; // replacement name za konstantu

                if(left) { // var je leva
                    var = Root->LeftPointer->content->name;
                    cons = Root->RightPointer;
                }
                else { // var je prava
                    var = Root->RightPointer->content->name;
                    cons = Root->LeftPointer;
                }

                printf("DEFVAR @$type_%s\n", var);
                printf("TYPE @$type_%s %s,\n", var, var);

                printf("JUMPIFEQ $label_same_type @$type_%s string@%s\n",var,cons->content->type); // porovnej typ s konstantou
                printf("JUMPIFEQ $label_type_int @$type_%s string@int\n", var); // porovnej typ s intem
                printf("JUMPIFEQ $label_type_float @$type_%s string@float\n", var); // porovnej typ s floatem

                printf("EXIT int@4\n"); // chyba 4

                printf("LABEL $label_type_int\n"); // je to int, preved ho na float
                printf("INT2FLOAT @$temp_%s %s\n",var,var);
                printf("JUMP $label_same_type\n"); // prevedena hodnota promenne je ted v temp_

                printf("LABEL $label_type_int\n"); // je to int, preved ho na float
                printf("INT2FLOAT @$temp_%s %s\n",var,var);
                printf("JUMP $label_same_type\n"); // prevedena hodnota promenne je ted v temp_%s

                printf("LABEL $label_type_float\n"); // je to float, preved const na float
                printf("INT2FLOAT @$temp_%s %s@%s\n",cons->content->name, cons->content->type, cons->content->name); // vysledek prevodu do $temp_const

                printf("LABEL $label_same_type\n"); // proved operaci, jsou stejnyho typu

            }
        } // operaci je mozne provest s $temp_%s na obou stranach


        /*
         * co jisti gabriel: vstup const const do operace budou oba spravne typy, v operaci const var ze bude const spravneho typu
         */

}

void postorder(tASTPointer* Root, tQueue* q) { // rekurzivni postorder pro postupne generovani vyrazu v generate_expression(AST)

    if (Root == NULL)
        return ;
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
        return ;

    printf("DEFVAR @%%%i\n", counter); // operace, chystam tedy novou promennou
    int leftvar; // leva strana
    int rightvar; // prava strana

    type_control(Root, Root->ID); // typova kontrola probehne v kazdem pripade.

    // TISK OPERACE START
    if(Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name != NULL) { // ani jeden z L R neni operator, tisk operace
        printf("%s @%%%i $type_%s $type_%s\n", op, counter, Root->LeftPointer->content->name, Root->RightPointer->content->name);
    }
    else if((Root->LeftPointer->content->name == NULL && Root->RightPointer->content->name != NULL) || (Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name == NULL)) { // jeden z L R je operace
        // tisk operace kdyz je pouze jedna strana (L || R) operaator
        if(Root->LeftPointer->content->name == NULL) { // L je operator
            queueGet(q, &leftvar);
            printf("%s @%%%i @%%%i $type_%s\n", op, counter, leftvar, Root->RightPointer->content->name);
        }
        else { // R je operator
            queueGet(q, &rightvar);
            printf("%s @%%%i $type_%s @%%%i\n", op, counter, Root->LeftPointer->content->name,rightvar);
        }
    }
    else { // tisk operace kdyz je operator L i R
        queueGet(q, &leftvar);
        queueGet(q, &rightvar);
        printf("%s @%%%i @%%%i @%%%i\n", op, counter, leftvar,rightvar);
    }
    // TISK OPERACE END

    queueUp(q,counter); // nahravas do fronty pokazde kdyz delas vyraz, kde je root operator

    counter++; // pricti 1 k promenne

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

