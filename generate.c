//
// Created by root on 11/23/18.
//

#include "generate.h"
#include "semantic.h"
#include "queue.h"
#include <stdlib.h>
#include "errors.h"

static int counter = 1; // zaciname na %1. Je reset counteru potreba, kdyz to pobezi cely program?
//bool concat = false; // jestli ma dojit k CONCAT misto ADD

void printf_string(char* str, FILE* list) { // vytiskni ascii variantu retezce
    for(long unsigned i = 0; i < strlen(str); i++) {
        int s = str[i];
        if (s <= 32)
            fprintf(list,"\\%03d", s);
        else if (s == 35)
            fprintf(list,"\\%03d", s);
        else if (s == 92)
            fprintf(list,"\\%03d", s);
        else
            fprintf(list,"%c", s);
    }
}

void operation_assign(tASTPointer* Root, tFunctionTracker* functionTracker, FILE* list) { // operace prirazeni do promenne, pro strom o velikosti 1 pouze

    char *frame = get_frame(functionTracker);

    if(!strcmp(Root->content->type,"variable")) { // je to promenna, eg. a = b
        fprintf(list,"DEFVAR %s@%%assign\n", frame);
        fprintf(list,"MOVE %s@%%assign %s@%s\n", frame, frame, Root->content->name);
    }
    else { // prirazeni konstanty
        fprintf(list,"DEFVAR %s@%%assign\n", frame);
        fprintf(list,"MOVE %s@%%assign %s@%s\n", frame, Root->content->type, Root->content->name);
    }

}

char* get_frame(tFunctionTracker* functionTracker) { // najdi aktualni ramec
    if(!strcmp(tFunctionTrackerGetTop(functionTracker),"Main"))
        return "GF";
    else {
        return "TF";
    }
}

void type_control(tASTPointer* Root,char* operation, tQueue* q, char* frame, FILE* list) { // typova kontrola obsahu jednoho root uzlu (L + R)

        fprintf(list,"counter: %d\n",counter);

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

        fprintf(list,"left: %d\n",left);
        fprintf(list,"left_operator: %d\n",left_operator);
        fprintf(list,"right: %d\n",right);
        fprintf(list,"right_operator: %d\n",right_operator);

        if(!left_operator) { // leva strana neni operator
            left_supply = Root->LeftPointer->content->name;

            fprintf(list,"DEFVAR %s@$type_%s$%d\n", frame, Root->LeftPointer->content->name, counter);
            fprintf(list,"DEFVAR %s@$temp_%s$%d\n", frame, Root->LeftPointer->content->name, counter);
            if (left) {
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter, frame, Root->LeftPointer->content->name);
                fprintf(list,"MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter, frame, Root->LeftPointer->content->name);
            } else {
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter, Root->LeftPointer->content->type,
                       Root->LeftPointer->content->name);
                fprintf(list,"MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter, Root->LeftPointer->content->type,
                       Root->LeftPointer->content->name);
            }
        }
        else { // leva strana je operator
            int front;
            queueFront(q,&front);

            char buffer[10];
            sprintf(buffer,"%%%d",front);
            left_supply = buffer;

            fprintf(list,"front: %d\n",front);

            fprintf(list,"DEFVAR %s@$type_%%%d\n", frame, front);
            fprintf(list,"DEFVAR %s@$temp_%%%d\n", frame, front);
            fprintf(list,"TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front);
            fprintf(list,"MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front);
        }

        if(!right_operator) { // prava strana neni operator
            right_supply = Root->RightPointer->content->name;

            fprintf(list,"DEFVAR %s@$type_%s$%d\n", frame, Root->RightPointer->content->name, counter);
            fprintf(list,"DEFVAR %s@$temp_%s$%d\n", frame, Root->RightPointer->content->name, counter);
            if (right) {
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter, frame, Root->RightPointer->content->name);
                fprintf(list,"MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter, frame, Root->RightPointer->content->name);
            } else {
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter, Root->RightPointer->content->type,
                       Root->RightPointer->content->name);
                fprintf(list,"MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter, Root->RightPointer->content->type,
                       Root->RightPointer->content->name);
            }
        }
        else { // prava strana je operator
            int front;

            if(left_operator) // pokud byla leva operator, sahni do fronty o jeden dal
                queuePreFront(q,&front);
            else // vlevo nebyl operator, sahni normalne
                queueFront(q,&front);

            char buffer[10];
            sprintf(buffer,"%%%d",front);
            right_supply = buffer;
            
            fprintf(list,"DEFVAR %s@$type_%%%d\n", frame, front);
            fprintf(list,"DEFVAR %s@$temp_%%%d\n", frame, front);
            fprintf(list,"TYPE %s@$type_%%%d %s@%%%d\n", frame, front,frame, front);
            fprintf(list,"MOVE %s@$temp_%%%d %s@%%%d\n", frame, front,frame, front);
        }

        // ZPRACOVANI INSTRUKCE PODLE OPERATORU

        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(left && right) { // obe strany jsou promenna

                fprintf(list,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter, frame, left_supply, counter); // skoc pokud je levy jiny nez int
                fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter, frame, right_supply, counter); // levy je int, otestuj pravy na int
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, right_supply, counter); // pokud pravy neni ani float, chyba
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, left_supply, counter, frame, left_supply); // pravy je float, levy preved na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter); // skoc na konec

                fprintf(list,"LABEL $label_left_not_int$%d\n",counter); // levy nebyl int
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter); // zkus jestli neni float, jestli ne tak chyba
                fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // je to float, otestuj jestli neni druha taky float
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter); // otestuj jestli neni druha int
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply); // druha je int, preved na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter);

                fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                fprintf(list,"ERROR int@4\n");

                fprintf(list,"LABEL $label_same_types$%d\n",counter);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                                   Root->LeftPointer->content->type, Root->LeftPointer->content->name);
                        } else { // int je vpravo, preved ho na float
                            fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                                   Root->RightPointer->content->type, Root->RightPointer->content->name);
                        }
                    }
                }

                else { // je tam alespon jeden operator

                    fprintf(list,"JUMPIFEQ $label_same_types$%d %s@temp_%s$%d @temp_%s\n",counter,frame, left_supply, counter, right_supply);

                    fprintf(list,"JUMPIFNEQ $convert_right$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter);
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply);
                    fprintf(list,"JUMP $label_same_types$%d\n",counter);

                    fprintf(list,"LABEL $convert_right$%d\n",counter);
                    fprintf(list,"INT2FLOAT %s@temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply);

                    fprintf(list,"LABEL $label_same_types$%d\n",counter);

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

                fprintf(list,"DEFVAR %s@$type_%s$%d\n",frame, var,counter);
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var);

                if(left_operator) // levy je operator
                    fprintf(list,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, left_supply, counter); // porovnej typ s typem leveho operatoru
                else if(right_operator) // pravy je operator
                    fprintf(list,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, right_supply, counter); // porovnej typ s typem praveho operatoru
                else // ani jeden nebyl operator
                    fprintf(list,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d string@%s\n",counter,frame, var, counter, cons->content->type); // porovnej typ s konstantou
                fprintf(list,"JUMPIFEQ $label_type_int$%d %s@$type_%s$%d string@int\n",counter, frame, var, counter); // porovnej typ s intem
                fprintf(list,"JUMPIFEQ $label_type_float$%d %s@$type_%s$%d string@float\n",counter, frame, var, counter); // porovnej typ s floatem

                fprintf(list,"EXIT int@4\n"); // chyba 4

                fprintf(list,"LABEL $label_type_int$%d\n",counter); // je to int, preved ho na float
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var,counter,frame, var);
                fprintf(list,"JUMP $label_same_type$%d\n",counter); // prevedena hodnota promenne je ted v temp_

                fprintf(list,"LABEL $label_type_int$%d\n",counter); // je to int, preved ho na float
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var, counter, frame, var);
                fprintf(list,"JUMP $label_same_type$%d\n",counter); // prevedena hodnota promenne je ted v temp_%s

                fprintf(list,"LABEL $label_type_float$%d\n",counter); // je to float, preved const na float
                if(left_operator) { // levy je operator
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply);
                }
                else if(right_operator) { // pravy je operator
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply);
                }
                else // zadny nebyl operator
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, cons->content->name, counter, cons->content->type, cons->content->name); // vysledek prevodu do $temp_const

                fprintf(list,"LABEL $label_same_type$%d\n",counter); // proved operaci, jsou stejnyho typu

            }
        }
        else if(!strcmp(operation,"+")) { // pro ADD oba musi byt float/int || string

            if(left && right) { // obe strany jsou promenna

                fprintf(list,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter); // skoc pokud neni levej string
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter); // proved concat jestli je pravej taky string
                fprintf(list,"JUMP $label_same_types$%d\n",counter);

                fprintf(list,"LABEL $label_left_not_string$%d\n",counter); // je jasne ze to neni retezec
                fprintf(list,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter); // skoc pokud je levy jiny nez int
                fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter); // levy je int, otestuj pravy na int
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // pokud pravy neni ani float, chyba
                fprintf(list,"INT2FLOAT %s@$temp_%s %s@%s\n",frame, left_supply, frame, left_supply); // pravy je float, levy preved na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter); // skoc na konec

                fprintf(list,"LABEL $label_left_not_int$%d\n",counter); // levy nebyl int
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter); // zkus jestli neni float, jestli ne tak chyba
                fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // je to float, otestuj jestli neni druha taky float
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter); // otestuj jestli neni druha int
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply); // druha je int, preved na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter);

                fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                fprintf(list,"ERROR int@4\n");

                fprintf(list,"LABEL $label_same_types$%d\n",counter);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int || string

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if((!strcmp(Root->LeftPointer->content->type,"string")) || (!strcmp(Root->RightPointer->content->type,"string"))) { // jeden z nich string a nejsou stejne typy, chyba
                            fprintf(list,"ERROR int@4\n");
                        }
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, Root->LeftPointer->content->name, counter,
                                   Root->LeftPointer->content->type, Root->LeftPointer->content->name);
                        } else { // int je vpravo, preved ho na float
                            fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                                   Root->RightPointer->content->type, Root->RightPointer->content->name);
                        }
                    }
                    //else if(!strcmp(Root->LeftPointer->content->type,"string")) // jeden z nich je string
                    //    concat = true; // nastav CONCAT misto ADD
                }
                else { // je tam alespon jeden operator

                    fprintf(list,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter); // skoc pokud neni levej string
                    fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter); // proved concat jestli je pravej taky string
                    fprintf(list,"JUMP $label_same_types$%d\n",counter);

                    fprintf(list,"LABEL $label_left_not_string$%d\n",counter); // je jasne ze to neni retezec
                    fprintf(list,"JUMPIFEQ $label_same_types$%d %s@temp_%s$%d %s@temp_%s$%d\n",counter,frame, left_supply, counter, frame, right_supply, counter);

                    fprintf(list,"JUMPIFNEQ $convert_right$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter);
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, left_supply, counter, frame, left_supply);
                    fprintf(list,"JUMP $label_same_types$%d\n",counter);

                    fprintf(list,"LABEL $convert_right$%d\n",counter);
                    fprintf(list,"INT2FLOAT %s@temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply);
                    fprintf(list,"JUMP $label_same_types$%d\n",counter);

                    fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                    fprintf(list,"ERROR int@4\n");

                    fprintf(list,"LABEL $label_same_types$%d\n",counter);

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

                fprintf(list,"DEFVAR %s@$type_%s$%d\n", frame, var, counter);
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var);

                // TOHLE JE ASI BLBE PREPIS NA PODMINKY
                fprintf(list,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter, frame, left_supply, counter); // skoc pokud neni levej string
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter, frame, right_supply, counter); // proved concat jestli je pravej taky string
                fprintf(list,"JUMP $label_same_types$%d\n",counter);
                // BLBE KONEC

                fprintf(list,"LABEL $label_left_not_string$%d\n",counter); // je jasne ze to neni retezec
                if(left_operator)
                    fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, left_supply, counter); // porovnej typ s typem leveho operatoru
                else if(right_operator)
                    fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, right_supply, counter); // porovnej typ s typem praveho operatoru
                else
                    fprintf(list,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@%s\n",counter,frame, var, counter, cons->content->type); // porovnej typ s konstantou
                fprintf(list,"JUMPIFEQ $label_type_int$%d %s@$type_%s$%d string@int\n",counter, frame, var, counter); // porovnej typ s intem
                fprintf(list,"JUMPIFEQ $label_type_float$%d %s@$type_%s$%d string@float\n",counter, frame, var, counter); // porovnej typ s floatem

                fprintf(list,"LABEL $label_error$%d\n",counter);
                fprintf(list,"EXIT int@4\n"); // chyba 4

                fprintf(list,"LABEL $label_type_int$%d\n",counter); // je to int, preved ho na float
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var, counter, frame, var);
                fprintf(list,"JUMP $label_same_types$%d\n",counter); // prevedena hodnota promenne je ted v temp_

                fprintf(list,"LABEL $label_type_int$%d\n",counter); // je to int, preved ho na float
                fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var,counter, frame, var);
                fprintf(list,"JUMP $label_same_types$%d\n",counter); // prevedena hodnota promenne je ted v temp_%s

                fprintf(list,"LABEL $label_type_float$%d\n",counter); // je to float, preved const na float
                if(left_operator) {
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply);
                }
                else if(right_operator) {
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply);
                }
                else
                    fprintf(list,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, cons->content->name, counter, cons->content->type, cons->content->name); // vysledek prevodu do $temp_const

                fprintf(list,"LABEL $label_same_types$%d\n",counter); // proved operaci, jsou stejnyho typu

            }
        }
        else { // pro DIV musi byt oba float a pravej nesmi byt nula

            if(left && right) { // obe strany jsou promenna

                fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter); // porovnani s int 0
                fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0.0\n",counter,frame, right_supply, counter); // porovnani s float 0 - BLBE

                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter); // skoc pokud je levy jiny nez float
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // levy je float, otestuj pravy na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter);

                fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                fprintf(list,"ERROR int@4\n");

                fprintf(list,"LABEL $label_error_div$%d\n",counter); // chyba deleni
                fprintf(list,"ERROR int@9\n");

                fprintf(list,"LABEL $label_same_types$%d\n",counter);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float, jsou tedy ok

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if(!strcmp(Root->RightPointer->content->name,"0") || !strcmp(Root->RightPointer->content->name,"0.0")) { // BLBE
                        fprintf(list,"ERROR int@9\n");
                        errorHandling(9);
                        return;
                    }
                }
                else { // je tam alespon jeden operator

                    fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter); // porovnani s int 0
                    fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0.0\n",counter,frame, right_supply, counter); // porovnani s float 0 - BLBE

                    fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter); // skoc pokud je levy jiny nez float
                    fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // levy je float, otestuj pravy na float
                    fprintf(list,"JUMP $label_same_types$%d\n",counter);

                    fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                    fprintf(list,"ERROR int@4\n");

                    fprintf(list,"LABEL $label_error_div$%d\n",counter); // chyba deleni
                    fprintf(list,"ERROR int@9\n");

                    fprintf(list,"LABEL $label_same_types$%d\n",counter);
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

                fprintf(list,"DEFVAR %s@$type_%s$%d\n", frame, var, counter);
                fprintf(list,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var);

                fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter); // porovnani s int 0
                fprintf(list,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0\n",counter,frame, right_supply, counter); // porovnani s float 0 - BLBE

                // ASI BLBE
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter); // skoc pokud je levy jiny nez float
                fprintf(list,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter); // levy je float, otestuj pravy na float
                fprintf(list,"JUMP $label_same_types$%d\n",counter);
                // BLBE KONEC

                fprintf(list,"LABEL $label_error$%d\n",counter); // chyba typu
                fprintf(list,"ERROR int@4\n");

                fprintf(list,"LABEL $label_error_div$%d\n",counter); // chyba deleni
                fprintf(list,"ERROR int@9\n");

                fprintf(list,"LABEL $label_same_types$%d\n",counter);

            }
        }

}

void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, FILE* list) { // rekurzivni postorder pro postupne generovani vyrazu v generate_expression(AST)

    if (Root == NULL)
        return ;
    postorder(Root->LeftPointer,q, functionTracker, list);
    postorder(Root->RightPointer,q, functionTracker, list);

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


    char* frame = get_frame(functionTracker); // vyhledej ve ktere jsme funkci

    type_control(Root, Root->ID,q,frame, list); // typova kontrola probehne v kazdem pripade

    /*if(concat) { // pokud to vyhodilo ze ma byt operace CONCAT misto ADD, nastav ji a resetuj bool concat
        op = "CONCAT";
        concat = false;
    }*/

    fprintf(list,"DEFVAR %s@%%%i\n",frame, counter); // operace, chystam tedy novou promennou
    int leftvar; // leva strana
    int rightvar; // prava strana

    // TISK OPERACE START
    if(Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name != NULL) { // ani jeden z L R neni operator, tisk operace
        fprintf(list,"%s %s@%%%i %s@$temp_%s$%d %s@$temp_%s$%d\n", op, frame, counter, frame, Root->LeftPointer->content->name, counter, frame, Root->RightPointer->content->name, counter);
    }
    else if((Root->LeftPointer->content->name == NULL && Root->RightPointer->content->name != NULL) || (Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name == NULL)) { // jeden z L R je operace
        // tisk operace kdyz je pouze jedna strana (L || R) operaator
        if(Root->LeftPointer->content->name == NULL) { // L je operator
            queueGet(q, &leftvar);
            fprintf(list,"%s %s@%%%i %s@$temp_%%%i %s@$temp_%s$%d\n", op, frame, counter, frame, leftvar, frame, Root->RightPointer->content->name, counter);
        }
        else { // R je operator
            queueGet(q, &rightvar);
            fprintf(list,"%s %s@%%%i %s@$temp_%s$%d %s@$temp_%%%i\n", op, frame, counter, frame, Root->LeftPointer->content->name, counter, frame, rightvar);
        }
    }
    else { // tisk operace kdyz je operator L i R
        queueGet(q, &leftvar);
        queueGet(q, &rightvar);
        fprintf(list,"%s %s@%%%i %s@%%%i %s@%%%i\n", op, frame, counter, frame, leftvar, frame, rightvar);
    }
    // TISK OPERACE END

    queueUp(q,counter); // nahravas do fronty pokazde kdyz delas vyraz, kde je root operator

    counter++; // pricti 1 k promenne

}

void generateExpression(tASTPointer* AST, tFunctionTracker* functionTracker, FILE* list) {

    if(AST->LeftPointer == NULL && AST->RightPointer == NULL) {// jedna se pouze o assign jednoducheho typu x = 1
        operation_assign(AST,functionTracker, list);
    }
    else { // jedna se o delsi vyraz

        char* frame = get_frame(functionTracker);

        tQueue* q = malloc(sizeof(tQueue)); // nova fronta pro generate_expression
        queueInit(q); // inicializuj frontu
        postorder(AST,q,functionTracker, list); // rekurzivni postorder stromem

        fprintf(list,"DEFVAR %s@%%assign\n",frame); // cilova hodnota vyrazu, NEXT mozna pojmenovat s counter kvuli originalite
        fprintf(list,"MOVE %s@%%assign %s@%%%i\n",frame, frame, counter-1); // do %assign dej posledni hodnotu counteru - po pricteni

        free(q); // uvolni frontu

    }

}