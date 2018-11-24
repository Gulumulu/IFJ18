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

/*char* which_frame(tASTPointer* Root) { // zjednodusena funkce, ktery ramec zvolit
    if(!strcmp(Root->content->type,"variable")) { // je to promenna
        if(!strcmp(tFunctionTrackerGetTop(functionTracker2),"main"))
            return "GF";
    }
}*/

void type_control(tASTPointer* Root,char* operation) {

        bool left = false; // false = neni promenna
        bool right = false; // false = neni promenna

        if(!strcmp(Root->LeftPointer->content->type,"variable")) { // leva strana je VAR
            left = true;
            printf("DEFVAR $type_%s\n", Root->LeftPointer->content->name);
            printf("DEFVAR $temp_%s\n", Root->LeftPointer->content->name);
            printf("TYPE $type_%s @%s,\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);
            printf("MOVE $temp_%s @%s,\n", Root->LeftPointer->content->name, Root->LeftPointer->content->name);
        }

        if(!strcmp(Root->RightPointer->content->type,"variable")) { // prava strana je VAR
            right = true;
            printf("DEFVAR $type_%s\n", Root->RightPointer->content->name);
            printf("DEFVAR $temp_%s\n", Root->RightPointer->content->name);
            printf("TYPE $type_%s @%s,\n", Root->RightPointer->content->name, Root->RightPointer->content->name);
            printf("MOVE $temp_%s @%s,\n", Root->RightPointer->content->name, Root->RightPointer->content->name);
        }

        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(left && right) { // obe strany jsou promenna

                // SITUACE VAR VAR: zjisti jestli je hodnota int. Pokud ne jump na label2. Pokud je zjisti jestli je druha int.
                // Pokud jo proved operaci. Pokud neni druha int zjisti jestli je druha hodnota float. Pokud je preved prvni na float a proved operaci.
                // Label2: Pokud prvni nebyla int otestuj jestli je prvni float. Pokud je prvni float otestuj jestli je druha float. Pokud ano proved operaci.
                // Pokud neni druha float otestuj jestli je druha int. Pokud je druha int preved ji na float a proved operaci.
                // Ted uz vyhod EXIT 4.

            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int
                if(strcmp(Root->LeftPointer->content->type,Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                    if(!strcmp(Root->LeftPointer->content->type,"int")) { // pokud je vlevo int, preved ho na float
                        printf("DEFVAR $temp_const\n");
                        printf("INT2FLOAT $temp_const %s@%s\n",Root->LeftPointer->content->type,Root->LeftPointer->content->name);
                    }
                    else { // int je vpravo, preved ho na float
                        printf("DEFVAR $temp_const\n");
                        printf("INT2FLOAT $temp_const %s@%s\n",Root->RightPointer->content->type,Root->RightPointer->content->name);
                    }
                }
            }

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

                printf("DEFVAR $type_%s\n", var);
                printf("TYPE $type_%s %s,\n", var, var);

                printf("JUMPIFEQ $label_same_type $type_%s string@%s\n",var,cons->content->type); // porovnej typ s konstantou
                printf("JUMPIFEQ $label_type_int $type_%s string@int\n", var); // porovnej typ s intem
                printf("JUMPIFEQ $label_type_float $type_%s string@float\n", var); // porovnej typ s floatem

                printf("EXIT int@4\n"); // chyba 4

                printf("LABEL $label_type_int\n"); // je to int, preved ho na float
                printf("INT2FLOAT $temp_%s %s\n",var,var);
                printf("JUMP $label_same_type\n"); // prevedena hodnota promenne je ted v temp_

                printf("LABEL $label_type_int\n"); // je to int, preved ho na float
                printf("INT2FLOAT $temp_%s %s\n",var,var);
                printf("JUMP $label_same_type\n"); // prevedena hodnota promenne je ted v temp_%s

                printf("LABEL $label_type_float\n"); // je to float, preved const na float
                printf("DEFVAR $temp_const\n"); // pro ulozeni nove hodnoty const
                printf("INT2FLOAT $temp_const %s@%s\n",cons->content->type,cons->content->name); // vysledek prevodu do $temp_const

                printf("LABEL $label_same_type\n"); // proved operaci, jsou stejnyho typu

            }
        }


        /*
         * co jisti gabriel: vstup const const do operace budouu oba spravne typy, v operaci const var ze bude const spravneho typu */

        // VYRESIT JAK SLOUCIT NAZVY PROMENNYCH Z TYPE CONTROL A POCETNI OPERACE!
        // mozna udrzet nazvy pomoci counteru u var a do stacku davat hodnoty konstant?

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

    // tady se musi spustit typova kontrola, pokud se jedna o alespon jednu promennou a ne o hodnotu

    // typova kontrola vezme uzel a zkontroluje, jestli se do nej neprirazuje z druhe strany jiny typ

    // v ADD SUB MUL musi byt oba int/float, v DIV oba float. v ADD mohou byt oba stringy

    // situace: var op const, const op const, var op var

    type_control(Root); // typova kontrola probehne v kazdem pripade.

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

