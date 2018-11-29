//
// Created by root on 11/23/18.
//

#include "generate.h"
#include "semantic.h"
#include "queue.h"
#include <stdlib.h>
#include <ctype.h>
#include "errors.h"


int parse_text = false; // jestli je parsovany vyraz "xx"
static int counter = 1; // globalni pocitadlo v uzlech. zaciname na %1
bool concat = false; // jestli ma dojit k CONCAT misto ADD

char* name_parse(char* str) { // vypreparuje ven z retezce string || promennou pro funkci strlen(string)

    int l = strlen(str);
    char* buff = malloc((l + 1)*sizeof(char));
    char help[l+1];
    int internal = 0;

    for(int j = 0; j < l+1; j++) {
        buff[j] = '\0';
        help[j] = '\0';
    }
    bool startReading = false;

    for(int i = 0; i < l; i++) {
        if(str[i] == '(') {
            startReading = true;
            continue;
        }

        if(startReading) {

            if(str[i] == ')') {
                strncpy(buff,help,10);
                return buff;
            }
            if(str[i] == '"') {
                parse_text = true;
                continue;
            }

            help[internal] = str[i];
            internal++;
        }
    }
    return "";
}

char* number_parse(char* str) { // vypreparuje pro funkci chr(i) string a vrati ho

    int l = strlen(str);
    char* buff = malloc((l + 1)*sizeof(char));
    char help[l+1];
    int internal = 0;

    for(int j = 0; j < l+1; j++) {
        buff[j] = '\0';
        help[j] = '\0';
    }
    bool startReading = false;

    for(int i = 0; i < l; i++) {
        if(str[i] == '(') {
            startReading = true;
            continue;
        }

        if(startReading) {

            if(str[i] == ')') {
                strncpy(buff,help,10);

                return buff;
            }
                help[internal] = str[i];
                internal++;
                continue;
        }
    }
    return "";
}

char* ord_parse(char* str) { // vypreparuje pro funkci strukturu s argumenty a vrati je

    int l = strlen(str);

    char* buff = malloc((l + 1)*sizeof(char));
    char help[l+1];
    int internal = 0;

    for(int j = 0; j < l+1; j++) {
        buff[j] = '\0';
        help[j] = '\0';
    }
    bool startReading = false;

    for(int i = 0; i < l; i++) {
        if(str[i] == '(') {
            startReading = true;
            continue;
        }

        if(startReading) {

            if(str[i] == ')') {
                strncpy(buff,help,10);
                return buff;
            }
            help[internal] = str[i];
            internal++;
            continue;
        }
    }

    return "";
}

char* convert_string(char* str) { // vytiskni ascii variantu retezce

    char* asciistr = malloc((strlen(str)* 4 + 1) * sizeof(char)); // novy retezec
    char help[strlen(str) * 4 + 1];

    for(unsigned long i = 0; i < (strlen(str)* 4 + 1); i++) {
        asciistr[i] = '\0';
        help[i] = '\0';
    }

    long unsigned a = 0;
    for(long unsigned i = 0; i < strlen(str); i++) {

        int s = str[i];

        if(s <= 32) {
            help[a] = '\\';
            a++;
            help[a] ='0';
            a++;
            if(a < 10) {
                help[a] = '0';
                a++;
                help[a] = (char)s;
                a++;
            }
            else {
                help[a] = (char)s;
                a++;
            }
        }
        else if(s == 35 || s == 92) {
            help[a] = '\\';
            a++;
            help[a] = '0';
            a++;
            help[a] = (char)s;
            a++;
        }
        else {
            help[a] = (char)s;
            a++;
        }
    }
    strncpy(asciistr,help,10);
    return asciistr;
}

char* get_frame(tFunctionTracker* functionTracker) { // najdi aktualni ramec
    if(!strcmp(tFunctionTrackerGetTop(functionTracker),"Main"))
        return "GF";
    else {
        return "TF";
    }
}

void type_control(tASTPointer* Root,char* operation, tQueue* q, char* frame, FILE* list) { // typova kontrola obsahu jednoho root uzlu (L + R)

        bool left = false; // false = neni promenna
        bool right = false; // false = neni promenna
        bool left_operator = false; // true = vlevo je operator
        bool right_operator = false; // true = vpravo je operator

        bool single = false; // single node

        char* left_supply; // zastupny symbol za levy uzel Root->LeftPointer->content->name
        char* right_supply; // zastupny symbol za pravy uzel Root->RightPointer->content->name

        // PREDBEZNE TESTY NA OBSAH UZLU

        if(Root->LeftPointer == NULL && Root->RightPointer == NULL) {// single node, pouze operace assign, bez L a R
            single = true;
        }

        if(single) { // operace, ktere probihaji nad stormem se single node

            if(!strcmp(Root->content->type, "length")) { // funkce length(string)

                char*parsed = name_parse(Root->content->name);

                fprintf(list,"DEFVAR %s%%%d\n",frame,counter);

                if(parse_text) { // parsovany vyraz byl "xx"
                    parse_text = false; // nastav zpet
                    fprintf(list,"STRLEN %s@%%%d string@%s\n",frame,counter,parsed);
                }
                else
                    fprintf(list,"STRLEN %s@%%%d %s@%s\n",frame, counter,frame, Root->content->name);

            }
            else if(!strcmp(Root->content->type, "chr")) { // funkce chr(i)

                char* parsed = number_parse(Root->content->name); // ulozena podstatna cast retezce z Root->content->name

                char *ptr; // string s testovou casti
                unsigned long ret; // ciselna cast
                ret = strtol(parsed,&ptr,10);

                if(ret < 256 && strlen(ptr) == 0) { // zadano cislo v platnem rozsahu
                    fprintf(list,"DEFVAR %s@%%%d\n",frame, counter);
                    fprintf(list,"INT2CHAR %s@%%%d int@%lu\n",frame,counter,ret);
                }
                else if(strlen(ptr) > 0) { // zadana promenna
                    fprintf(list,"DEFVAR %s@%%%d\n",frame, counter);
                    fprintf(list,"MOVE %s@%%%d %s@%s\n",frame,counter,frame,parsed);
                    fprintf(list,"DEFVAR %s@bool%d\n",frame,counter); // novy bool pro porovnani
                    fprintf(list,"LT %s@bool%d int@-1 %s@%%%d\n",frame,counter,frame,counter); // jestli vetsi nez -1 tak true do bool
                    fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter); // pokud bool false skoc do error
                    fprintf(list,"GT %s@bool%d int@256 %s@%%%d\n",frame,counter,frame,counter); // bool true pokud je mensi jak 256
                    fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter); // pokud bool false smoc na error
                    fprintf(list,"DEFVAR %s@newchar%d\n",frame,counter); // novy znak
                    fprintf(list,"INT2CHAR %s@newchar%d %s@%%%d\n",frame,counter,frame,counter); // preved na novy znak
                    fprintf(list,"JUMP $label_converted%d\n",counter);
                    fprintf(list,"LABEL $label_error_%d\n",counter);
                    fprintf(list,"ERROR int@4\n");
                    fprintf(list,"LABEL $label_converted%d\n",counter);
                }
                else { // zadana spatna ciselna hodnota
                    errorHandling(4);
                }

            }
            else if(!strcmp(Root->content->type, "ord")) { // funkce ord(s,i)

                fprintf(list,"DEFVAR %s@%%%d\n",frame, counter);

                char *str = ord_parse(Root->content->name);
                int l = strlen(str);
                char s[l+1];
                char i[l+1];
                for(int a = 0; a < l+1; a++) {
                    s[a] = '\0';
                    i[a] = '\0';
                }
                bool second = false;
                int c = 0;
                for(int a = 0; a < l; a++) {
                    if(str[a] == ',') {
                        second = true;
                        c = 0;
                        continue;
                    }
                    if(second) {
                        i[c] = str[a];
                        c++;
                    }
                    if(!second) {
                        if(str[a] == '"') {
                            parse_text = true;
                            continue;
                        }
                        s[c] = str[a];
                        c++;
                    }
                }
                char* s_help = malloc(sizeof(char) * (l + 1));
                char* i_help = malloc(sizeof(char) * (l + 1));
                strncpy(i_help,i,10);
                strncpy(s_help,s,10);
                char* i_ptr;
                unsigned long i_ret = strtol(i_help,&i_ptr,10);

                if(parse_text) { // zpracovavali jsme primo retezec

                    if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                        if(i_ret > (strlen(s_help)-1)) { // i je mimo rozsah
                            fprintf(list,"MOVE %s@%%%d string@nil\n",frame,counter);
                        }
                        else { // index je v rozsahu
                            fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                            fprintf(list,"GETCHAR %s@%%%d string@%s int@%s\n",frame,counter,s_help,i_help);
                        }
                    }
                    else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                        fprintf(list,"DEFVAR %s@$i_type%d\n",frame,counter);
                        fprintf(list,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help);
                        fprintf(list,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter);
                        fprintf(list,"ERROR int@4\n");

                        fprintf(list,"LABEL $label_i_ok_%d\n",counter);
                        fprintf(list,"DEFVAR %s@$len%d\n",frame,counter);
                        fprintf(list,"DEFVAR %s@$i%d\n",frame,counter);
                        fprintf(list,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help);
                        fprintf(list,"STRLEN %s@$len%d string@%s\n",frame,counter,s_help); // delku retezce do promenne len
                        fprintf(list,"DEFVAR %s@bool%d\n",frame,counter); // novy bool pro porovnani
                        fprintf(list,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter); // jestli vetsi nez -1 tak true do bool
                        fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter); // pokud bool false skoc do error
                        fprintf(list,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter); // bool true pokud je mensi jak 256
                        fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter);

                        fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                        fprintf(list,"GETCHAR %s@%%%d string@%s %s@%s\n",frame,counter,s_help, frame, i_help);
                        fprintf(list,"JUMP $label_end_%d\n",counter);

                        fprintf(list,"LABEL $label_error_%d\n",counter);
                        fprintf(list,"MOVE %s@%%%d string@nil\n",frame,counter);
                        fprintf(list,"LABEL $label_end_%d\n",counter);
                    }

                    parse_text = false; // reset parse_text zpatky
                }
                else { // nepracovali jsme s primo textem, ale s promennou

                    fprintf(list,"DEFVAR %s@$s_type%d\n",frame,counter);
                    fprintf(list,"TYPE %s@$s_type%d %s@%s\n",frame,counter,frame,s_help);
                    fprintf(list,"JUMPIFEQ $label_ok_%d %s@$s_type%d string@string\n",counter,frame,counter);
                    fprintf(list,"ERROR int@4\n");

                    fprintf(list,"LABEL $label_ok_%d\n",counter);

                    if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                        if(i_ret > l-1) { // i je mimo rozsah
                            fprintf(list,"MOVE %s@%%%d string@nil\n",frame,counter);
                        }
                        else { // index je v rozsahu
                            fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                            fprintf(list,"GETCHAR %s@%%%d string@%s int@%s\n",frame,counter,s_help,i_help);
                        }
                    }
                    else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                        fprintf(list,"DEFVAR %s@$i_type%d\n",frame,counter);
                        fprintf(list,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help);
                        fprintf(list,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter);
                        fprintf(list,"ERROR int@4\n");

                        fprintf(list,"LABEL $label_i_ok_%d\n",counter);
                        fprintf(list,"DEFVAR %s@$len%d\n",frame,counter);
                        fprintf(list,"DEFVAR %s@$i%d\n",frame,counter);
                        fprintf(list,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help);
                        fprintf(list,"STRLEN %s@$len%d string@%s\n",frame,counter,s_help); // delku retezce do promenne len
                        fprintf(list,"DEFVAR %s@bool%d\n",frame,counter); // novy bool pro porovnani
                        fprintf(list,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter); // jestli vetsi nez -1 tak true do bool
                        fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter); // pokud bool false skoc do error
                        fprintf(list,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter); // bool true pokud je mensi jak 256
                        fprintf(list,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter);

                        fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                        fprintf(list,"GETCHAR %s@%%%d string@%s %s@%s\n",frame,counter,s_help, frame, i_help);
                        fprintf(list,"JUMP $label_end_%d\n",counter);

                        fprintf(list,"LABEL $label_error_%d\n",counter);
                        fprintf(list,"MOVE %s@%%%d string@nil\n",frame,counter);
                        fprintf(list,"LABEL $label_end_%d\n",counter);
                    }
                }

                free(s_help);
                free(i_help);

            }

            else if(!strcmp(Root->content->type,"variable")) { // je to promenna, eg. a = b
                fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                fprintf(list,"MOVE %s@%%%d %s@%s\n",frame,counter,frame,Root->content->name);
            }

            else { // zbyva prirazeni konstanty
                fprintf(list,"DEFVAR %s@%%%d\n",frame,counter);
                fprintf(list,"MOVE %s@%%%d %s@%s\n",frame,counter,Root->content->type,Root->content->name);
            }

            counter++; // operace probehla

            return ; // vyskoc ven
        }

        // ODTUD TO NENI SINGLE NODE

            if (Root->LeftPointer->content->name == NULL) // vlevo je operator
                left_operator = true;
            else if (!strcmp(Root->LeftPointer->content->type, "variable")) // leva strana je VAR
                left = true;
            if (Root->RightPointer->content->name == NULL) // vpravo je operator
                right_operator = true;
            else if (!strcmp(Root->RightPointer->content->type, "variable")) // prava strana je VAR
                right = true;

            if (!left_operator) { // leva strana neni operator
                left_supply = Root->LeftPointer->content->name;

                fprintf(list, "DEFVAR %s@$type_%s$%d\n", frame, Root->LeftPointer->content->name, counter);
                fprintf(list, "DEFVAR %s@$temp_%s$%d\n", frame, Root->LeftPointer->content->name, counter);
                if (left) {
                    fprintf(list, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            frame, Root->LeftPointer->content->name);
                    fprintf(list, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            frame, Root->LeftPointer->content->name);
                } else {
                    fprintf(list, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            Root->LeftPointer->content->type,
                            Root->LeftPointer->content->name);
                    fprintf(list, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            Root->LeftPointer->content->type,
                            Root->LeftPointer->content->name);
                }
            } else { // leva strana je operator
                int front;
                queueFront(q, &front);

                char buffer[10];
                sprintf(buffer, "%%%d", front);
                left_supply = buffer;

                fprintf(list, "DEFVAR %s@$type_%%%d\n", frame, front);
                fprintf(list, "DEFVAR %s@$temp_%%%d\n", frame, front);
                fprintf(list, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front);
                fprintf(list, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front);
            }

            if (!right_operator) { // prava strana neni operator
                right_supply = Root->RightPointer->content->name;

                fprintf(list, "DEFVAR %s@$type_%s$%d\n", frame, Root->RightPointer->content->name, counter);
                fprintf(list, "DEFVAR %s@$temp_%s$%d\n", frame, Root->RightPointer->content->name, counter);
                if (right) {
                    fprintf(list, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            frame, Root->RightPointer->content->name);
                    fprintf(list, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            frame, Root->RightPointer->content->name);
                } else {
                    fprintf(list, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            Root->RightPointer->content->type,
                            Root->RightPointer->content->name);
                    fprintf(list, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            Root->RightPointer->content->type,
                            Root->RightPointer->content->name);
                }
            } else { // prava strana je operator
                int front;

                if (left_operator) // pokud byla leva operator, sahni do fronty o jeden dal
                    queuePreFront(q, &front);
                else // vlevo nebyl operator, sahni normalne
                    queueFront(q, &front);

                char buffer[10];
                sprintf(buffer, "%%%d", front);
                right_supply = buffer;

                fprintf(list, "DEFVAR %s@$type_%%%d\n", frame, front);
                fprintf(list, "DEFVAR %s@$temp_%%%d\n", frame, front);
                fprintf(list, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front);
                fprintf(list, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front);
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

    char* frame = get_frame(functionTracker); // vyhledej ve ktere jsme funkci
    type_control(Root, Root->ID,q,frame, list); // typova kontrola probehne v kazdem pripade
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

        char* frame = get_frame(functionTracker);

        tQueue* q = malloc(sizeof(tQueue)); // nova fronta pro generate_expression
        queueInit(q); // inicializuj frontu
        postorder(AST,q,functionTracker, list); // rekurzivni postorder stromem

        fprintf(list,"DEFVAR %s@%%assign%d\n",frame,assign); // cilova hodnota vyrazu, NEXT mozna pojmenovat s counter kvuli originalite
        fprintf(list,"MOVE %s@%%assign%d %s@%%%i\n",frame, assign, frame, counter-1); // do %assign dej posledni hodnotu counteru - po pricteni
        assign++;

        free(q); // uvolni frontu

}

void generateCode(char* stackTop, int rules[]) {
    printf("STACK: %s\n",stackTop);
    for(int i = 0; i < 50; i++)
        printf("rule: %d\n",rules[i]);
    printf("GEN FINISHED.\n");
}