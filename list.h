/**
 *
 * Project for IFJ course - compiler
 * Header file for global variables used during code generation
 *
 * Implemented by: Michal Plsek          xplsek03
 */
#include <stdbool.h>

#ifndef IFJ_LIST_H
#define IFJ_LIST_H

int dyn_length; // dyn se menici delka listu
int list_length; // pocitadlo delky listu
char* list_str; // list pro tisk
char* asciistr; // urceno pro convert_string() free()
bool issingle; // urceni zda se jedna o single node

#endif //IFJ_LIST_H
