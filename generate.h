//
// Created by root on 11/23/18.
//

#include "queue.h"
#include "semantic.h"
#include "syntax.h"
#include "scanner.h"
#include <stdio.h>

#ifndef IFJ18_GENERATE_H
#define IFJ18_GENERATE_H

static int assign = 1; // pocitadlo assign zacina na 1

char* convert_string(char* str);
char* get_frame(tFunctionTracker* functionTracker);

void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, char* list_str);
void generateExpression(tASTPointer* AST,tFunctionTracker* functionTracker, char* list_str, bool comp);
void generateCode(char* stackTop, int rules[]);

void type_control(tASTPointer* Root, char* operation, tQueue* q, char* frame, char* list_str);

void generate_to_list(int ad, int* ptr_list_length, int* ptr_dyn_length, char* list_str);
void generate_to_list2(int ad);

void call_function(int id, char* frame, tASTPointer* Root, char* list_str);

char* name_parse(char* str);
char* number_parse(char* str);
char* ord_parse(char* str);
char* susbtr_parse(char* str);

// stack for if labels
typedef struct tLabelStack {
    int top;
    int numbers[100];
} tLabelStack;

tLabelStack* labelStack;         // stack for if labels
tLabelStack* endLabelStack;      // stack for if end labels
tLabelStack* tmpVariables;      // stack for tmp variables

float str2fl(char* str);
char* str2str(char * str);

// helper vars
int firstTime;

int ifLabelNumber;
int ifEndLabelNumber;
int ifStatement;

int whileLabelNumber;
int whileEndLabelNumber;
int whileStatement;

int tmpVariableNumber;

// functions for generating
void generateCodeParek(Token* token);
void generateIfHead(tASTPointer *AST, tFunctionTracker* functionTracker);
void generateIfMid();
void generateIfEnd();
void generatePrint(Token* token, char* currentFunction);

void generateWhileHead(tASTPointer *AST,tFunctionTracker* functionTracker);
void generateWhileEnd();

// functions for working with stack
void tLabelStackInit(tLabelStack* stack);
void tLabelStackPush(tLabelStack* stack, int labelNumber);
void tLabelStackPop(tLabelStack* stack);
int tLabelStackGetTop(tLabelStack* stack);

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