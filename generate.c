//
// Created by root on 11/23/18.
//

#include "generate.h"
#include "semantic.h"
#include "queue.h"
#include <stdlib.h>
#include <ctype.h>
#include "errors.h"
#include "list.h"

int dyn_length = 1024; // dyn poc delka listu pro tisk
int list_length = 0; // ukazatel na pozici v listu

bool issingle = false; // urceni jestli je single node (strom vel. 1)

bool parse_text = false; // jestli je parsovany vyraz "xx"
static int counter = 1; // globalni pocitadlo v uzlech. zaciname na %1
//bool concat = false; // jestli ma dojit k CONCAT misto ADD

void generate_to_list2(int ad,char* str) { // generovani do seznamu misto do souboru v2
    if(ad > dyn_length - list_length) { // uz tam neni dost mista, realloc
        dyn_length *= 2;
        str = realloc(str,dyn_length);
    }
    list_length += ad; // pridej nove vygenerovanou delku ze sprintf do ukazatele na pozici v listu
}

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

char* arguments_parse(char* str) { // vypreparuje pro funkci ord a strsub string s argumenty a vrati ho

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

void type_control(tASTPointer* Root,char* operation, tQueue* q, char* frame, char* list_str) { // typova kontrola obsahu jednoho root uzlu (L + R)

        bool left = false; // false = neni promenna
        bool right = false; // false = neni promenna
        bool left_operator = false; // true = vlevo je operator
        bool right_operator = false; // true = vpravo je operator

        bool single = false; // single node

        char* left_supply; // zastupny symbol za levy uzel Root->LeftPointer->content->name
        char* right_supply; // zastupny symbol za pravy uzel Root->RightPointer->content->name

        // PREDBEZNE TESTY NA OBSAH UZLU

        if(issingle) { // operace, ktere probihaji nad stromem se single node

            if(!strcmp(Root->content->type, "length")) { // funkce length(string)

                char*parsed = name_parse(Root->content->name);

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s%%%d\n",frame,counter),list_str);

                if(parse_text) { // parsovany vyraz byl "xx"
                    parse_text = false; // nastav zpet
                    generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@%%%d string@%s\n",frame,counter,parsed),list_str);
                }
                else // parsovany vyraz byla promenna
                    generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@%%%d %s@%s\n",frame, counter,frame, Root->content->name),list_str);


            }
            else if(!strcmp(Root->content->type, "chr")) { // funkce chr(i)

                char* parsed = number_parse(Root->content->name); // ulozena podstatna cast retezce z Root->content->name

                char *ptr; // string s testovou casti
                unsigned long ret; // ciselna cast
                ret = strtol(parsed,&ptr,10);


                if(ret < 256 && strlen(ptr) == 0) { // zadano cislo v platnem rozsahu
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2CHAR %s@%%%d int@%lu\n",frame,counter,ret),list_str);
                }
                else if(strlen(ptr) > 0) { // zadana promenna
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@%s\n",frame,counter,frame,parsed),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter),list_str); // novy bool pro porovnani
                    generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@%%%d\n",frame,counter,frame,counter),list_str); // jestli vetsi nez -1 tak true do bool
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str); // pokud bool false skoc do error
                    generate_to_list2(sprintf(list_str+list_length,"GT %s@bool%d int@256 %s@%%%d\n",frame,counter,frame,counter),list_str); // bool true pokud je mensi jak 256
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str); // pokud bool false smoc na error
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@newchar%d\n",frame,counter),list_str); // novy znak
                    generate_to_list2(sprintf(list_str+list_length,"INT2CHAR %s@newchar%d %s@%%%d\n",frame,counter,frame,counter),list_str); // preved na novy znak
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_converted%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_converted%d\n",counter),list_str);
                }
                else { // zadana spatna ciselna hodnota
                    errorHandling(4);
                }

            }
            else if(!strcmp(Root->content->type, "ord")) { // funkce ord(s,i)

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter),list_str);

                char *str = arguments_parse(Root->content->name);
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
                            generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                        }
                        else { // index je v rozsahu
                            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                            generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%%%d string@%s int@%s\n",frame,counter,s_help,i_help),list_str);
                        }
                    }
                    else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i_type%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_i_ok_%d\n",counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$len%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@$len%d string@%s\n",frame,counter,s_help),list_str); // delku retezce do promenne len
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter),list_str); // novy bool pro porovnani
                        generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter),list_str); // jestli vetsi nez -1 tak true do bool
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str); // pokud bool false skoc do error
                        generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter),list_str); // bool true pokud je mensi jak 256
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%%%d string@%s %s@%s\n",frame,counter,s_help, frame, i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_end_%d\n",counter),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_end_%d\n",counter),list_str);
                    }

                    parse_text = false; // reset parse_text zpatky
                }
                else { // nepracovali jsme s primo textem, ale s promennou

                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$s_type%d\n",frame,counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$s_type%d %s@%s\n",frame,counter,frame,s_help),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_ok_%d %s@$s_type%d string@string\n",counter,frame,counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_ok_%d\n",counter),list_str);

                    if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                        if(i_ret > (unsigned long)l-1) { // i je mimo rozsah
                            generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                        }
                        else { // index je v rozsahu
                            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                            generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%%%d string@%s int@%s\n",frame,counter,s_help,i_help),list_str);
                        }
                    }
                    else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i_type%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_i_ok_%d\n",counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$len%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@$len%d string@%s\n",frame,counter,s_help),list_str); // delku retezce do promenne len
                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter),list_str); // novy bool pro porovnani
                        generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter),list_str); // jestli vetsi nez -1 tak true do bool
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str); // pokud bool false skoc do error
                        generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter),list_str); // bool true pokud je mensi jak 256
                        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%%%d string@%s %s@%s\n",frame,counter,s_help, frame, i_help),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_end_%d\n",counter),list_str);

                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_end_%d\n",counter),list_str);
                    }
                }

                free(s_help);
                free(i_help);

            }
            /*else if(!strcmp(Root->content->type,"substr")) { // funkce susbtr(s,i,n)

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter);

                char *str = arguments_parse(Root->content->name);
                int l = strlen(str);
                char s[l+1];
                char i[l+1];
                char n[l+1];
                for(int a = 0; a < l+1; a++) {
                    s[a] = '\0';
                    i[a] = '\0';
                    n[a] = '\0';
                }
                bool second = false;
                bool third = false;
                int c = 0;
                for(int a = 0; a < l; a++) {
                    if(str[a] == ',' && !second) {
                        second = true;
                        c = 0;
                        continue;
                    }
                    if(str[a] == ',' && second && !third) {
                        third = true;
                        c = 0;
                        continue;
                    }
                    if(second && !third) {
                        i[c] = str[a];
                        c++;
                    }
                    if(third) {
                        n[c] = str[a];
                        c++;
                    }
                    if(!second && !third) {
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
                char* n_help = malloc(sizeof(char) * (l + 1));
                strncpy(i_help,i,10);
                strncpy(s_help,s,10);
                strncpy(n_help,s,10);
                char* i_ptr;
                unsigned long i_ret = strtol(i_help,&i_ptr,10);
                char* n_ptr;
                unsigned long n_ret = strtol(n_help,&n_ptr,10);

                ; // DOPLNIT NA KONCI

                parse_text = false;
                free(s_help);
                free(i_help);
                free(n_help);

            }*/
            else if(!strcmp(Root->content->type,"inputi")) { // vestavena funkce inputi
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"READ %s@%%%d int\n",frame,counter),list_str); // nacti do promenne
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@intemp%d\n",frame,counter),list_str); // uloz do tmp
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@intemp%d %s%%%d\n",frame,counter,frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@intype%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@intype%d %s@%%%d\n",frame,counter,frame,counter),list_str); // uloz typ
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_convert%d %s@intype%d string@float\n",counter,frame,counter),list_str); // jestli je float zkonvertuj
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_wrong_type%d %s@intype%d string@int\n",counter,frame,counter),list_str); // jestli neni ani int tak 0
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter),list_str); // je to int

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_convert%d\n",counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"FLOAT2INT %s@%%%d %s@intemp%d\n",frame,counter,frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_wrong_type%d\n",counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d int@0\n",frame,counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter),list_str);
            }
            else if(!strcmp(Root->content->type,"inputf")) { // vestavena funkce inputf
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"READ %s@%%%d float\n",frame,counter),list_str); // nacti do promenne
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@floatemp%d\n",frame,counter),list_str); // uloz do tmp
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@floatemp%d %s%%%d\n",frame,counter,frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@floatype%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@floatype%d %s@%%%d\n",frame,counter,frame,counter),list_str); // uloz typ
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_convert%d %s@floatype%d string@int\n",counter,frame,counter),list_str); // jestli je int zkonvertuj
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_wrong_type%d %s@floatype%d string@float\n",counter,frame,counter),list_str); // jestli neni ani float tak 0.0
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter),list_str); // je to float

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_convert%d\n",counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@%%%d %s@floatemp%d\n",frame,counter,frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_wrong_type%d\n",counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d float@0.0\n",frame,counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter),list_str);
            }
            else if(!strcmp(Root->content->type,"inputs")) { // vestavena funkce inputs
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"READ %s@%%%d string\n",frame,counter),list_str); // nacti do promenne
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@strtype%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@strtype%d %s@%%%d\n",frame,counter,frame,counter),list_str); // uloz typ
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_inok%d %s@strtype%d string@string\n",counter,frame,counter),list_str); // jestli neni ani string tak nil
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter),list_str);
            }
            else if(!strcmp(Root->content->type,"print")) { // vestavena funkce print

                char *str = arguments_parse(Root->content->name); // dlooooouhy retezec se vsemi argumenty
                int l = strlen(str); // delka celkoveho retezce
                char s[l+1]; // pomocne pole pro jednotlivy operand
                char* operand = malloc(sizeof(char) * (l + 1)); // finalni operand
                long int_val;
                float float_val;
                char* float_rest;
                char* int_rest;
                for(int a = 0; a < l+1; a++) // vynulovani pole
                    s[a] = '\0';
                int internal = 0;
                bool second_time = false; // zatim jeste nebyla nalezena parova '"'

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);

                for(int a = 0; a < l+1; a++) { // projizdim znak po znaku retezec s argumenty

                    if(second_time && (str[a] == ',' || str[a] == '\0')) { // oddelovac nebo konec, kdyz uz byla nalezena druha '"'
                        strncpy(operand,s,10); // vytvoren retezec operand
                        int_val = strtol(operand, &int_rest,10);
                        float_val = strtof(operand,&float_rest);

                        // tady pracuj s jednim operandem

                        if(parse_text) { // operand je textovy retezec
                            generate_to_list2(sprintf(list_str+list_length,"WRITE string@%s\n",operand),list_str);
                            parse_text = false;
                            second_time = false;
                        }
                        else if(int_val != 0 && !strlen(int_rest)) { // operand je int
                            generate_to_list2(sprintf(list_str+list_length,"WRITE int@%ld\n",int_val),list_str);
                        }

                        else if(float_val != 0 && !strlen(float_rest)) { // operand je float
                            generate_to_list2(sprintf(list_str+list_length,"WRITE float@%f\n",float_val),list_str);
                        }

                        else { // operand je promenna
                            generate_to_list2(sprintf(list_str+list_length,"WRITE %s@%s\n",frame,operand),list_str); // dodelat az to pujde testovat
                        } // DODELAT

                        // konec prace s operandem

                        internal = 0;
                        for(int a = 0; a < l+1; a++) // vynulovani pole
                            s[a] = '\0';
                        continue;
                    }

                    if(parse_text && str[a] == '"') { // byla nalezena parova uvozovka, moznost nacitat, dalsi retezec
                        second_time = true;
                        continue;
                    }

                    if(str[a] == '"') {
                        parse_text = true;
                        continue;
                    }
                    s[internal] = str[a];
                    internal++;
                }

                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d nil@nil\n",frame,counter),list_str); // navrat print je vzdy nil@nil

                free(operand);
                parse_text = false; // kdyby nahodou

            }

            else if(!strcmp(Root->content->type,"variable")) { // je to promenna, eg. a = b
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@%s\n",frame,counter,frame,Root->content->name),list_str);
            }

            else { // zbyva prirazeni konstanty
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@%s\n",frame,counter,Root->content->type,Root->content->name),list_str);
            }

            counter++; // operace probehla

            return ; // vyskoc ven
        }

        // ODTUD TO NENI SINGLE NODE

            // priprava na

            char* funkce[] = {"ord","chr","print","length","inputi","inputf","inputs"};
            bool left_func = false;
            bool right_func = false;

            for(int i = 0; i < 7; i++) {
                if(!strcmp(Root->LeftPointer->content->type,funkce[i]))
                    left_func = true;
                if(!strcmp(Root->RightPointer->content->type,funkce[i]))
                    right_func = true;
            }

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

                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, Root->LeftPointer->content->name, counter),list_str);
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, Root->LeftPointer->content->name, counter),list_str);
                if (left) {
                    generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            frame, Root->LeftPointer->content->name),list_str);
                    generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            frame, Root->LeftPointer->content->name),list_str);
                } else {
                    generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            Root->LeftPointer->content->type,
                            Root->LeftPointer->content->name),list_str);
                    generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                            Root->LeftPointer->content->type,
                            Root->LeftPointer->content->name),list_str);
                }
            } else { // leva strana je operator
                int front;
                queueFront(q, &front);

                char buffer[10];
                sprintf(buffer, "%%%d", front);
                left_supply = buffer;

                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%%%d\n", frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%%%d\n", frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front),list_str);
            }



            if (!right_operator) { // prava strana neni operator
                right_supply = Root->RightPointer->content->name;

                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, Root->RightPointer->content->name, counter),list_str);
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, Root->RightPointer->content->name, counter),list_str);
                if (right) {
                    generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            frame, Root->RightPointer->content->name),list_str);
                    generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            frame, Root->RightPointer->content->name),list_str);
                } else {
                    generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            Root->RightPointer->content->type,
                            Root->RightPointer->content->name),list_str);
                    generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                            Root->RightPointer->content->type,
                            Root->RightPointer->content->name),list_str);
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

                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%%%d\n", frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%%%d\n", frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front),list_str);
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front),list_str);
            }


        // ZPRACOVANI INSTRUKCE PODLE OPERATORU
        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(left && right) { // obe strany jsou promenna

                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter, frame, left_supply, counter),list_str); // skoc pokud je levy jiny nez int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter, frame, right_supply, counter),list_str); // levy je int, otestuj pravy na int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, right_supply, counter),list_str); // pokud pravy neni ani float, chyba
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, left_supply, counter, frame, left_supply),list_str); // pravy je float, levy preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str); // skoc na konec

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter),list_str); // levy nebyl int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter),list_str); // zkus jestli neni float, jestli ne tak chyba
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // je to float, otestuj jestli neni druha taky float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter),list_str); // otestuj jestli neni druha int
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply),list_str); // druha je int, preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->LeftPointer->content->name, counter,
                                   Root->LeftPointer->content->type, Root->LeftPointer->content->name),list_str);
                        } else { // int je vpravo, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                                   Root->RightPointer->content->type, Root->RightPointer->content->name),list_str);
                        }
                    }
                }

                else { // je tam alespon jeden operator

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@temp_%s$%d @temp_%s\n",counter,frame, left_supply, counter, right_supply),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $convert_right$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $convert_right$%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);

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

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$type_%s$%d\n",frame, var,counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var),list_str);

                if(left_operator) // levy je operator
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, left_supply, counter),list_str); // porovnej typ s typem leveho operatoru
                else if(right_operator) // pravy je operator
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, right_supply, counter),list_str); // porovnej typ s typem praveho operatoru
                else // ani jeden nebyl operator
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d string@%s\n",counter,frame, var, counter, cons->content->type),list_str); // porovnej typ s konstantou
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_int$%d %s@$type_%s$%d string@int\n",counter, frame, var, counter),list_str); // porovnej typ s intem
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_float$%d %s@$type_%s$%d string@float\n",counter, frame, var, counter),list_str); // porovnej typ s floatem

                generate_to_list2(sprintf(list_str+list_length,"EXIT int@4\n"),list_str); // chyba 4

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter),list_str); // je to int, preved ho na float
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var,counter,frame, var),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_type$%d\n",counter),list_str); // prevedena hodnota promenne je ted v temp_

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter),list_str); // je to int, preved ho na float
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var, counter, frame, var),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_type$%d\n",counter),list_str); // prevedena hodnota promenne je ted v temp_%s

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_float$%d\n",counter),list_str); // je to float, preved const na float
                if(left_operator) { // levy je operator
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply),list_str);
                }
                else if(right_operator) { // pravy je operator
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply),list_str);
                }
                else // zadny nebyl operator
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, cons->content->name, counter, cons->content->type, cons->content->name),list_str); // vysledek prevodu do $temp_const

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_type$%d\n",counter),list_str); // proved operaci, jsou stejnyho typu

            }
        }
        else if(!strcmp(operation,"+")) { // pro ADD oba musi byt float/int || string

            if(left && right) { // obe strany jsou promenna

                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter),list_str); // skoc pokud neni levej string
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter),list_str); // proved concat jestli je pravej taky string
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter),list_str); // je jasne ze to neni retezec
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter),list_str); // skoc pokud je levy jiny nez int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter),list_str); // levy je int, otestuj pravy na int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // pokud pravy neni ani float, chyba
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@%s\n",frame, left_supply, frame, left_supply),list_str); // pravy je float, levy preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str); // skoc na konec

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter),list_str); // levy nebyl int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter),list_str); // zkus jestli neni float, jestli ne tak chyba
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // je to float, otestuj jestli neni druha taky float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter),list_str); // otestuj jestli neni druha int
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply),list_str); // druha je int, preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float/int || string

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if((!strcmp(Root->LeftPointer->content->type,"string")) || (!strcmp(Root->RightPointer->content->type,"string"))) { // jeden z nich string a nejsou stejne typy, chyba
                            generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);
                        }
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, Root->LeftPointer->content->name, counter,
                                   Root->LeftPointer->content->type, Root->LeftPointer->content->name),list_str);
                        } else { // int je vpravo, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, Root->RightPointer->content->name, counter,
                                   Root->RightPointer->content->type, Root->RightPointer->content->name),list_str);
                        }
                    }
                    //else if(!strcmp(Root->LeftPointer->content->type,"string")) // jeden z nich je string
                    //    concat = true; // nastav CONCAT misto ADD
                }
                else { // je tam alespon jeden operator

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter),list_str); // skoc pokud neni levej string
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter),list_str); // proved concat jestli je pravej taky string
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter),list_str); // je jasne ze to neni retezec
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@temp_%s$%d %s@temp_%s$%d\n",counter,frame, left_supply, counter, frame, right_supply, counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $convert_right$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, left_supply, counter, frame, left_supply),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $convert_right$%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@temp_%s$%d %s@%s\n",frame, right_supply, counter, frame, right_supply),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);

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

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$type_%s$%d\n", frame, var, counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var),list_str);

                // TOHLE JE ASI BLBE PREPIS NA PODMINKY
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter, frame, left_supply, counter),list_str); // skoc pokud neni levej string
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter, frame, right_supply, counter),list_str); // proved concat jestli je pravej taky string
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);
                // BLBE KONEC

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter),list_str); // je jasne ze to neni retezec
                if(left_operator)
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, left_supply, counter),list_str); // porovnej typ s typem leveho operatoru
                else if(right_operator)
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d %s@$type_%s$%d\n",counter,frame, var, counter, frame, right_supply, counter),list_str); // porovnej typ s typem praveho operatoru
                else
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@%s\n",counter,frame, var, counter, cons->content->type),list_str); // porovnej typ s konstantou
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_int$%d %s@$type_%s$%d string@int\n",counter, frame, var, counter),list_str); // porovnej typ s intem
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_float$%d %s@$type_%s$%d string@float\n",counter, frame, var, counter),list_str); // porovnej typ s floatem

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"EXIT int@4\n"),list_str); // chyba 4

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter),list_str); // je to int, preved ho na float
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var, counter, frame, var),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str); // prevedena hodnota promenne je ted v temp_

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter),list_str); // je to int, preved ho na float
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, var,counter, frame, var),list_str);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str); // prevedena hodnota promenne je ted v temp_%s

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_float$%d\n",counter),list_str); // je to float, preved const na float
                if(left_operator) {
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply),list_str);
                }
                else if(right_operator) {
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply),list_str);
                }
                else
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@%s\n",frame, cons->content->name, counter, cons->content->type, cons->content->name),list_str); // vysledek prevodu do $temp_const

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str); // proved operaci, jsou stejnyho typu

            }
        }
        else { // pro DIV musi byt oba float a pravej nesmi byt nula

            if(left && right) { // obe strany jsou promenna

                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter),list_str); // porovnani s int 0
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0.0\n",counter,frame, right_supply, counter),list_str); // porovnani s float 0 - BLBE

                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter),list_str); // skoc pokud je levy jiny nez float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // levy je float, otestuj pravy na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter),list_str); // chyba deleni
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);
            }

            else if(!left && !right) { // obe jsou konstanty, gabriel: filtrem prosly jako float, jsou tedy ok

                if(!left_operator && !right_operator) { // pro pripady kdy tam neni operator
                    if(!strcmp(Root->RightPointer->content->name,"0") || !strcmp(Root->RightPointer->content->name,"0.0")) { // BLBE
                        generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"),list_str);
                        errorHandling(9);
                        return;
                    }
                }
                else { // je tam alespon jeden operator

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter),list_str); // porovnani s int 0
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0.0\n",counter,frame, right_supply, counter),list_str); // porovnani s float 0 - BLBE

                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter),list_str); // skoc pokud je levy jiny nez float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // levy je float, otestuj pravy na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter),list_str); // chyba deleni
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"),list_str);

                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);
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

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$type_%s$%d\n", frame, var, counter),list_str);
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$type_%s$%d %s@%s\n", frame, var, counter, frame, var),list_str);

                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d int@0\n",counter,frame, right_supply, counter),list_str); // porovnani s int 0
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$type_%s$%d float@0\n",counter,frame, right_supply, counter),list_str); // porovnani s float 0 - BLBE

                // ASI BLBE
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter),list_str); // skoc pokud je levy jiny nez float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter),list_str); // levy je float, otestuj pravy na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter),list_str);
                // BLBE KONEC

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter),list_str); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter),list_str); // chyba deleni
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"),list_str);

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter),list_str);

            }
        }

}

void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, char* list_str) { // rekurzivni postorder pro postupne generovani vyrazu v generate_expression(AST)

    if (Root == NULL)
        return ;
    postorder(Root->LeftPointer,q, functionTracker, list_str);
    postorder(Root->RightPointer,q, functionTracker, list_str);

    // PROCESSING SINGLE NODE

    char* frame = get_frame(functionTracker); // vyhledej ve ktere jsme funkci

    if(issingle || (Root->LeftPointer != NULL && Root->RightPointer != NULL)) // vylouceni listu z type_control, krome single node
        type_control(Root, Root->ID,q,frame, list_str); // typova kontrola

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

    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%i\n",frame, counter),list_str); // operace, chystam tedy novou promennou
    int leftvar; // leva strana
    int rightvar; // prava strana

    // TISK OPERACE START
    if(Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name != NULL) { // ani jeden z L R neni operator, tisk operace
        generate_to_list2(sprintf(list_str+list_length,"%s %s@%%%i %s@$temp_%s$%d %s@$temp_%s$%d\n", op, frame, counter, frame, Root->LeftPointer->content->name, counter, frame, Root->RightPointer->content->name, counter),list_str);
    }
    else if((Root->LeftPointer->content->name == NULL && Root->RightPointer->content->name != NULL) || (Root->LeftPointer->content->name != NULL && Root->RightPointer->content->name == NULL)) { // jeden z L R je operace
        // tisk operace kdyz je pouze jedna strana (L || R) operaator
        if(Root->LeftPointer->content->name == NULL) { // L je operator
            queueGet(q, &leftvar);
            generate_to_list2(sprintf(list_str+list_length,"%s %s@%%%i %s@$temp_%%%i %s@$temp_%s$%d\n", op, frame, counter, frame, leftvar, frame, Root->RightPointer->content->name, counter),list_str);
        }
        else { // R je operator
            queueGet(q, &rightvar);
            generate_to_list2(sprintf(list_str+list_length,"%s %s@%%%i %s@$temp_%s$%d %s@$temp_%%%i\n", op, frame, counter, frame, Root->LeftPointer->content->name, counter, frame, rightvar),list_str);
        }
    }
    else { // tisk operace kdyz je operator L i R
        queueGet(q, &leftvar);
        queueGet(q, &rightvar);
        generate_to_list2(sprintf(list_str+list_length,"%s %s@%%%i %s@%%%i %s@%%%i\n", op, frame, counter, frame, leftvar, frame, rightvar),list_str);
    }
    // TISK OPERACE END

    queueUp(q,counter); // nahravas do fronty pokazde kdyz delas vyraz, kde je root operator

    counter++; // pricti 1 k promenne

}

void generateExpression(tASTPointer* AST, tFunctionTracker* functionTracker, char* list_str) {

        char* frame = get_frame(functionTracker);

        tQueue* q = malloc(sizeof(tQueue)); // nova fronta pro generate_expression
        queueInit(q); // inicializuj frontu

        if(AST->LeftPointer == NULL && AST->RightPointer == NULL)
            issingle = true;

        postorder(AST,q,functionTracker, list_str); // rekurzivni postorder stromem
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%assign%d\n",frame,assign),list_str); // cilova hodnota vyrazu, NEXT mozna pojmenovat s counter kvuli originalite
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%assign%d %s@%%%i\n",frame, assign, frame, counter-1),list_str); // do %assign dej posledni hodnotu counteru - po pricteni
        assign++;

        issingle = false;
        free(q); // uvolni frontu

}

void generateCode(char* stackTop, int rules[]) {
    printf("STACK: %s\n",stackTop);
    for(int i = 0; i < 50; i++)
        printf("rule: %d\n",rules[i]);
    printf("GEN FINISHED.\n");
}