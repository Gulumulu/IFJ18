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

void call_function(int id, char* frame, tASTPointer* Root, char* list_str);

char* name_parse(char* str);
char* number_parse(char* str);
char* ord_parse(char* str);
char* susbtr_parse(char* str);

#endif //IFJ18_GENERATE_H

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
                long i_ret = strtol(i_help,&i_ptr,10);
                char* n_ptr;
                long n_ret = strtol(n_help,&n_ptr,10);

                ; // DOPLNIT NA KONCI

                parse_text = false;
                free(s_help);
                free(i_help);
                free(n_help);

            }*/

/* minula vetev issingle v if(issingle) v type_control()
 *
 * if(!strcmp(Root->content->type, "length")) { // funkce length(string)
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s%%%d\n",frame,counter),list_str);
                char*parsed = name_parse(Root->content->name);
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
                long ret; // ciselna cast
                ret = strtol(parsed,&ptr,10);
                if(ret < 256 && strlen(ptr) == 0) { // zadano cislo v platnem rozsahu
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"INT2CHAR %s@%%%d int@%ld\n",frame,counter,ret),list_str);
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
                    generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@newchar%d\n",frame,counter,frame,counter),list_str); // uloz novy znak do func
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_converted%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"),list_str);
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_converted%d\n",counter),list_str);
                }
                else // zadana spatna ciselna hodnota
                    errorHandling(4);
            }

            else if(!strcmp(Root->content->type, "ord")) { // funkce ord(s,i)

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame, counter),list_str);

                char *str = arguments_parse(Root->content->name);
                long l = strlen(str);
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
                long i_ret = strtol(i_help,&i_ptr,10);

                if(parse_text) { // zpracovavali jsme primo retezec

                    if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                        if(i_ret > (long)(strlen(s_help)-1)) { // i je mimo rozsah
                            generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d string@nil\n",frame,counter),list_str);
                        }
                        else { // index je v rozsahu
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
                        if(i_ret > l-1) { // i je mimo rozsah
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
                long l = strlen(str); // delka celkoveho retezce
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

            */