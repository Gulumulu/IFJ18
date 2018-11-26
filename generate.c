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
    printf("MOVE %%assign @%s\n",Root->ID);
}

void type_control(tASTPointer* Root,char* operation, tQueue* q) {

        bool left = false; // false = neni promenna
        bool right = false; // false = neni promenna
        bool left_operator = false; // true = vlevo je operator
        bool right_operator = false; // true = vpravo je operator

        char* left_supply; // zastupny symbol za levy uzel Root->LeftPointer->content->name
        char* right_supply; // zastupny symbol za pravy uzel Root->RightPointer->content->name

        // PREDBEZNE TESTY NA OBSAH UZLU

        if(Root->LeftPointer->content->name == NULL) // vlevo je operator
            left_operator = true;
        else if(!strcmp(Root->LeftPointer->content->type,"variable")) // leva strana je VAR
            left = true;
        if(Root->RightPointer->content->name == NULL) // vpravo je operator
            right_operator = true;
        else if(!strcmp(Root->RightPointer->content->type,"variable")) // prava strana je VAR
            right = true;

        if(!left_operator) { // leva strana neni operator
            left_supply = Root->LeftPointer->content->name;

            printf("DEFVAR @$type_%s\n", Root->LeftPointer->content->name);
            printf("DEFVAR @$temp_%s\n", Root->LeftPointer->content->name);
            if (left) {
                printf("TYPE @$type_%s @%s\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);
                printf("MOVE @$temp_%s @%s\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);
            } else {
                printf("TYPE @$type_%s %s@%s\n", Root->LeftPointer->content->name, Root->LeftPointer->content->type,
                       Root->LeftPointer->content->name);
                printf("MOVE @$temp_%s %s@%s\n", Root->LeftPointer->content->name, Root->LeftPointer->content->type,
                       Root->LeftPointer->content->name);
            }
        }
        else { // leva strana je operator
            int front;
            queueFront(q,&front);

            char buffer[10];
            sprintf(buffer,"%%%d",front);
            left_supply = buffer;

            printf("DEFVAR @$type_%%%d\n", front);
            printf("DEFVAR @$temp_%%%d\n", front);
            printf("TYPE @$type_%%%d @%%%d\n", front,front);
            printf("MOVE @$temp_%%%d @%%%d\n", front,front);
        }

        if(!right_operator) { // prava strana neni operator
            right_supply = Root->RightPointer->content->name;

            printf("DEFVAR @$type_%s\n", Root->RightPointer->content->name);
            printf("DEFVAR @$temp_%s\n", Root->RightPointer->content->name);
            if (right) {
                printf("TYPE @$type_%s @%s\n", Root->RightPointer->content->name, Root->RightPointer->content->name);
                printf("MOVE @$temp_%s @%s\n", Root->RightPointer->content->name, Root->RightPointer->content->name);
            } else {
                printf("TYPE @$type_%s %s@%s\n", Root->RightPointer->content->name, Root->RightPointer->content->type,
                       Root->RightPointer->content->name);
                printf("MOVE @$temp_%s %s@%s\n", Root->RightPointer->content->name, Root->RightPointer->content->type,
                       Root->RightPointer->content->name);
            }
        }
        else { // prava strana je operator
            int prefront;
            queuePreFront(q,&prefront);

            char buffer[10];
            sprintf(buffer,"%%%d",prefront);
            right_supply = buffer;

            printf("DEFVAR @$type_%%%d\n", prefront);
            printf("DEFVAR @$temp_%%%d\n", prefront);
            printf("TYPE @$type_%%%d @%%%d\n", prefront,prefront);
            printf("MOVE @$temp_%%%d @%%%d\n", prefront,prefront);
        }

        // ZPRACOVANI INSTRUKCE PODLE OPERATORU

        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(left && right) { // obe strany jsou promenna

                printf("JUMPIFNEQ $label_left_not_int $type_%s string@int\n",left_supply); // skoc pokud je levy jiny nez int
                printf("JUMPIFEQ $label_same_types $type_%s string@int\n",right_supply); // levy je int, otestuj pravy na int
                printf("JUMPIFNEQ $label_error @$type_%s string@float\n",right_supply); // pokud pravy neni ani float, chyba
                printf("INT2FLOAT @$temp_%s @%s\n",left_supply, left_supply); // pravy je float, levy preved na float
                printf("JUMP $label_same_types\n"); // skoc na konec

                printf("LABEL $label_left_not_int\n"); // levy nebyl int
                printf("JUMPIFNEQ $label_error @$type_%s string@float\n",left_supply); // zkus jestli neni float, jestli ne tak chyba
                printf("JUMPIFEQ $label_same_types @$type_%s string@float\n",right_supply); // je to float, otestuj jestli neni druha taky float
                printf("JUMPIFNEQ $label_error @$type_%s string@int\n",right_supply); // otestuj jestli neni druha int
                printf("INT2FLOAT @$temp_%s @%s\n",right_supply, right_supply); // druha je int, preved na float
                printf("JUMP $label_same_types\n");

                printf("LABEL $label_error\n"); // chyba typu
                printf("ERROR int@4\n");
                printf("LABEL $label_same_types\n");
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            printf("INT2FLOAT @$temp_%s %s@%s\n", Root->LeftPointer->content->name,
                                   Root->LeftPointer->content->type, Root->LeftPointer->content->name);
                        } else { // int je vpravo, preved ho na float
                            printf("INT2FLOAT @$temp_%s %s@%s\n", Root->RightPointer->content->name,
                                   Root->RightPointer->content->type, Root->RightPointer->content->name);
                        }
                    }
                }

                else { // je tam alespon jeden operator

                    printf("JUMPIFEQ $label_same_types @temp_%s @temp_%s\n",left_supply, right_supply);

                    printf("JUMPIFNEQ $convert_right @$type_%s string@int\n",left_supply);
                    printf("INT2FLOAT @$temp_%s @%s\n", left_supply, left_supply);
                    printf("JUMP $label_same_types\n");

                    printf("LABEL $convert_right\n");
                    printf("INT2FLOAT @temp_%s @%s\n",right_supply, right_supply);

                    printf("LABEL $label_same_types\n");

                }

            }

            else { // jedna ze stran je konstanta

                char* var; // replacement name leve nebo prave var, podle toho kde byla
                tASTPointer* cons; // replacement name za konstantu

                if(left) { // var je leva
                    var = Root->LeftPointer->content->name;
                    if(!right_operator)
                        cons = Root->RightPointer;

                }
                else { // var je prava
                    var = Root->RightPointer->content->name;
                    if(!left_operator)
                        cons = Root->LeftPointer;
                }

                printf("DEFVAR @$type_%s\n", var);
                printf("TYPE @$type_%s %s,\n", var, var);

                if(left_operator)
                    printf("JUMPIFEQ $label_same_type @$type_%s $type_%s\n",var,left_supply); // porovnej typ s typem leveho operatoru
                else if(right_operator)
                    printf("JUMPIFEQ $label_same_type @$type_%s $type_%s\n",var,right_supply); // porovnej typ s typem praveho operatoru
                else
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
                if(left_operator) {
                    printf("INT2FLOAT @$temp_%s @%s\n", left_supply, left_supply);
                }
                else if(right_operator) {
                    printf("INT2FLOAT @$temp_%s @%s\n", right_supply, right_supply);
                }
                else
                    printf("INT2FLOAT @$temp_%s %s@%s\n",cons->content->name, cons->content->type, cons->content->name); // vysledek prevodu do $temp_const

                printf("LABEL $label_same_type\n"); // proved operaci, jsou stejnyho typu

            }
        }

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

    type_control(Root, Root->ID,q); // typova kontrola probehne v kazdem pripade

    printf("DEFVAR @%%%i\n", counter); // operace, chystam tedy novou promennou
    int leftvar; // leva strana
    int rightvar; // prava strana

    // TISK OPERACE START
    if(Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name != NULL) { // ani jeden z L R neni operator, tisk operace
        printf("%s @%%%i $temp_%s $temp_%s\n", op, counter, Root->LeftPointer->content->name, Root->RightPointer->content->name);
    }
    else if((Root->LeftPointer->content->name == NULL && Root->RightPointer->content->name != NULL) || (Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name == NULL)) { // jeden z L R je operace
        // tisk operace kdyz je pouze jedna strana (L || R) operaator
        if(Root->LeftPointer->content->name == NULL) { // L je operator
            queueGet(q, &leftvar);
            printf("%s @%%%i @%%%i $temp_%s\n", op, counter, leftvar, Root->RightPointer->content->name);
        }
        else { // R je operator
            queueGet(q, &rightvar);
            printf("%s @%%%i $temp_%s @%%%i\n", op, counter, Root->LeftPointer->content->name,rightvar);
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

    printf("DEFVAR %%assign\n"); // cilova hodnota vyrazu, NEXT mozna pojmenovat s counter kvuli originalite
    printf("MOVE %%assign @%%%i\n",counter-1); // do %assign dej posledni hodnotu counteru - po pricteni


    free(q); // uvolni frontu
}