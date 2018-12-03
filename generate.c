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
#include "if-generate.h"
#include "predict.h"

//asciistr; // je tu kvuli funkci convert_string, aby se dalo dat free() kdekoliv kde je funkce zavolana
//bool issingle = false; // urceni jestli je single node (strom vel. 1)
char* left_supply; // zastupny symbol za levy uzel Root->LeftPointer->content->name
char* right_supply; // zastupny symbol za pravy uzel Root->RightPointer->content->name
char* funkce[] = {"ord","chr","print","length","inputi","inputf","inputs"}; // seznam podporovanych funkci
bool left = false; // false = neni promenna
bool right = false; // false = neni promenna
bool left_func = false; // Root->Left je funkce
bool right_func = false; // Root->Right je funkce
// zvlastni zpracovani retezcu
bool left_str = false; // Root->Left je string
bool right_str = false; // Root->Right je string
bool left_operator = false; // true = vlevo je operator
bool right_operator = false; // true = vpravo je operator
bool parse_text = false; // jestli je parsovany vyraz "xx"
static int counter = 1; // globalni pocitadlo v uzlech. zaciname na %1

/* to co bylo v if */

char* myIfLabel = "$myIfLabel";
char* myIfEndLabel = "$myIfEndLabel";
char* myWhileLabel = "$myWhileLabel";
char* myWhileEndLabel = "$myWhileEndLabel";
char* myTmpVariable = "$myTmpVariable";

float str2fl(char* str) {
    char* float_rest;
    return strtof(str,&float_rest);
}

/**
 * Function initializes stack to store label numbers.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 */
void tLabelStackInit(tLabelStack* stack) {
    stack->top = 0;
    for (int i = 0; i < 100; i++) {
        stack->numbers[i] = 0;
    }
}

/**
 * Function pushes label number in the stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 * @param labelNumber int number pushed in the stack
 */
void tLabelStackPush(tLabelStack* stack, int labelNumber) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->numbers[stack->top] = labelNumber;
        stack->top++;
    }
}

/**
 * Function pop label number from stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 */
void tLabelStackPop(tLabelStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        if (stack->top > 0) {
            stack->top--;
            stack->numbers[stack->top] = 0;
        }
    }
}

/**
 * Function retrieves top label number from stack.
 *
 * @param stack pointer to tLabelStack structure is initialized stack
 * @return int label number from stack top
 */
int tLabelStackGetTop(tLabelStack* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return 0;
    } else {
        return stack->numbers[stack->top-1];
    }
}

/**
 * Function generates head for if statement.
 *
 * @param AST structure tASTPointer is pointer to AST
 */
void generateIfHead(tASTPointer *AST, tFunctionTracker* functionTracker) {
    if (AST == NULL) {
        errorHandling(99);
    } else {

        if (firstTime == 0) {
            firstTime++;
            labelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(labelStack);
            endLabelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(endLabelStack);
            tmpVariables = malloc(sizeof(tLabelStack));
            tLabelStackInit(tmpVariables);
        }
        if (strcmp(AST->ID, "!=") == 0) {
            // gener
            ifLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d $symb1 $symb2\n", myIfLabel, ifLabelNumber));
            tLabelStackPush(labelStack, ifLabelNumber);

        } else if (strcmp(AST->ID, "==") == 0) {
            generateExpression(AST, functionTracker, list_str, 1);
            ifLabelNumber++;
            char* frame = get_frame(functionTracker);
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ %s%d %s@comp_l$%i %s@comp_r$%i\n", myIfEndLabel, ifLabelNumber, frame, counter-1, frame, counter-1));
            tLabelStackPush(labelStack, ifLabelNumber);

        } else if (strcmp(AST->ID, "<") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"LT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ %s%d TF@%s%d bool@true \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"AND TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d TF@%s%d $symb2 \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, ">") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"GT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ %s%d TF@%s%d bool@true\n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(labelStack, ifLabelNumber);
        } else if (strcmp(AST->ID, ">=") == 0) {
            printf("still need to calculate expression.\n");
            ifLabelNumber++;
            tmpVariableNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"OR TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d TF@%s%d $symb1 \n", myIfLabel, ifLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(labelStack, ifLabelNumber);
        }
    }
}

/**
 * Function generates middle section for if statement.
 */
void generateIfMid() {
    ifEndLabelNumber++;
    generate_to_list2(sprintf(list_str+list_length,"JUMP %s%d \n", myIfEndLabel, ifEndLabelNumber));
    tLabelStackPush(endLabelStack, ifEndLabelNumber);
    generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myIfLabel, tLabelStackGetTop(labelStack)));
    tLabelStackPop(labelStack);
}

/**
 * Function generates end to if statement.
 */
void generateIfEnd() {
    generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myIfEndLabel, tLabelStackGetTop(endLabelStack)));
    tLabelStackPop(endLabelStack);
}

/**
 * Function generates head for while loop.
 *
 * @param AST structure tASTPointer is pointer to AST
 */
void generateWhileHead(tASTPointer *AST) {
    if (AST == NULL) {
        errorHandling(99);
    } else {
        if (firstTime == 0) {
            firstTime++;
            labelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(labelStack);
            endLabelStack = malloc(sizeof(tLabelStack));
            tLabelStackInit(endLabelStack);
        }
        if (strcmp(AST->ID, "!=") == 0) {
            //generateExpression(AST, functionTracker, list_str, 1); // M
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d $symb1 $symb2 \n", myWhileEndLabel, whileEndLabelNumber));
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "==") == 0) {
            //generateExpression(AST, functionTracker, list_str, 1); // M
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"LT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ %s%d TF@%s%d bool@true \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d\n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"AND TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d TF@%s%d $symb2 \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, ">") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"GT TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ %s%d TF@%s%d bool@true \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        } else if (strcmp(AST->ID, "<=") == 0) {
            printf("still need to calculate expression.\n");
            whileLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d\n", myWhileLabel, whileLabelNumber));
            tLabelStackPush(labelStack, whileLabelNumber);
            whileEndLabelNumber++;
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR TF@%s%d\n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"OR TF@%s%d $symb1 $symb2 \n", myTmpVariable, tmpVariableNumber));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ %s%d TF@%s%d $symmb1 \n", myWhileLabel, whileEndLabelNumber, myTmpVariable, tmpVariableNumber));
            tLabelStackPush(endLabelStack, whileEndLabelNumber);
        }
    }
}

/**
 * Function generates an ending for while loop.
 */
void generateWhileEnd() {
    generate_to_list2(sprintf(list_str+list_length,"JUMP %s%d \n", myWhileLabel, tLabelStackGetTop(labelStack)));
    tLabelStackPop(labelStack);
    generate_to_list2(sprintf(list_str+list_length,"LABEL %s%d \n", myWhileEndLabel, tLabelStackGetTop(endLabelStack)));
    tLabelStackPop(endLabelStack);
}

/**
 * Function generates print in IFJcode18
 * @param token input token
 */
void generatePrint(Token* token, char* currentFunction) {

    if (token == NULL) {
        errorHandling(99);
    } else {

        if (token->type == s_string) {
            generate_to_list2(sprintf(list_str+list_length,"WRITE string@%s\n", convert_string(token->content)));
            free(asciistr);
        } else if (token->type == s_int) {
            generate_to_list2(sprintf(list_str+list_length,"WRITE int@%s\n", token->content));
        } else if (token->type == s_float) {
            generate_to_list2(sprintf(list_str+list_length,"WRITE float@%s\n", token->content));
        } else if (token->type == s_id) {
            if (strcmp(currentFunction, "Main") == 0) {
                generate_to_list2(sprintf(list_str+list_length,"WRITE GF@%s\n", token->content));
            } else {
                generate_to_list2(sprintf(list_str+list_length,"WRITE LF@%s\n", token->content));
            }
        }
        else if (token->type == s_exp_int || token->type == s_exp_f ) {
            generate_to_list2(sprintf(list_str+list_length,"WRITE float@%a\n", str2fl(token->content)));
        }
    }
}

/**
 * Function for deciding which generate function will be called.
 *
 * @param token input token
 */
void generateCodeParek(Token* token) {
    if (token == NULL) {
        errorHandling(99);
    } else {
        if (ifStatement != 0) {
            switch (token->type) {
                case kw_if:
                case kw_then:
                    // if-statement header has already been generated
                    break;
                case kw_else:
                    generateIfMid();
                    break;
                case kw_end:
                    generateIfEnd();
                    break;
                default:
                    break;
            }
        }
        if (whileStatement != 0) {
            if (token->type == kw_end) {
                generateWhileEnd();
            }
        }
    }
}

/* to co bylo v if */

void generate_to_list2(int ad) { // generovani do seznamu misto do souboru v2

    if(dyn_length / 2 < list_length) { // uz tam neni dost mista, realloc
        dyn_length *= 2;
/*        char* tmp = malloc(dyn_length+1);
        strcpy(tmp,list_str);
        tmp[strlen(list_str)] = '\0';
        free(list_str);
        list_str = malloc(dyn_length+1);
        strcpy(list_str,tmp);
        list_str[strlen(tmp)] = '\0';
        free(tmp);*/

        void* check = realloc(list_str,dyn_length);
        list_str = check;
    }
    list_length += ad; // pridej nove vygenerovanou delku ze sprintf do ukazatele na pozici v listu
}

char* name_parse(char* str) { // vypreparuje ven z retezce string || promennou pro funkci strlen(string)

    long l = strlen(str);

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
                strcpy(buff,help);
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

    long l = strlen(str);
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
                strcpy(buff,help);

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

    long l = strlen(str);

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
                strcpy(buff,help);
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

    long a = 0;
    char ret[4];
    for(unsigned long i = 0; i < strlen(str); i++) {
        char c = str[i];
        int s = (int)c;
        sprintf(ret,"%d",s);
        if(s <= 32) {
            help[a] = '\\';
            a++;
            help[a] ='0';
            a++;
            if(a < 10) {
                help[a] = '0';
                a++;
                help[a] = ret[0];
                a++;
            }
            else {
                help[a] = ret[0];
                a++;
                help[a] = ret[1];
                a++;
            }
        }
        else if(s == 35) {
            help[a] = '\\';
            a++;
            help[a] = '0';
            a++;
            help[a] = '3';
            a++;
            help[a] = '5';
            a++;
        }
        else if(s == 92) {
            help[a] = '\\';
            a++;
            help[a] = '0';
            a++;
            help[a] = '9';
            a++;
            help[a] = '2';
            a++;
        }
        else if(c == '"')
            continue;
        else {
            help[a] = c;
            a++;
        }
    }
    strcpy(asciistr,help);
    return asciistr;
}

char* get_frame(tFunctionTracker* functionTracker) { // najdi aktualni ramec
    if(!strcmp(tFunctionTrackerGetTop(functionTracker),"Main"))
        return "GF";
    else {
        return "TF";
    }
}

void call_function(int id, char* frame, tASTPointer* Root, char* list_str) { // napoveda: char* funkce[] = {"ord","chr","print","length","inputi","inputf","inputs"};

    char* macro;

    if (!issingle) {
        macro = "func"; // makro pro nazev promenne (%s%d)

        // vygeneruj zakl promenne, pracuje se s nimi pri vypsani operace
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@func%d\n", frame, counter));
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@temp_%s$%d\n", frame, Root->content->type, counter));
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@type_%s$%d\n", frame, Root->content->type, counter));
    }
    else {
        macro = "%"; // makro pro nazev promenne, pokud je single node (%%%d)
    }

    if(id == 3) { // funkce length(string)
        char*parsed = name_parse(Root->content->name);
        if(parse_text) { // parsovany vyraz byl "xx"
            parse_text = false; // nastav zpet
            generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@%s%d string@%s\n",frame,macro,counter,convert_string(parsed)));
            //free(asciistr);
        }
        else {// parsovany vyraz byla promenna
            generate_to_list2(sprintf(list_str + list_length, "STRLEN %s@%s%d %s@%s\n", frame,macro, counter, frame, Root->content->name));
        }
        //free(parsed);
    }

    else if(id == 1) { // funkce chr(i)
        char* parsed = number_parse(Root->content->name); // ulozena podstatna cast retezce z Root->content->name
        char *ptr; // string s testovou casti
        long ret; // ciselna cast
        ret = strtol(parsed,&ptr,10);
        if(ret < 256 && strlen(ptr) == 0) { // zadano cislo v platnem rozsahu
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro, counter));
            generate_to_list2(sprintf(list_str+list_length,"INT2CHAR %s@%s%d int@%ld\n",frame,macro,counter,ret));
        }
        else if(strlen(ptr) > 0) { // zadana promenna
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro, counter));
            generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d %s@%s\n",frame,macro,counter,frame,parsed));
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter)); // novy bool pro porovnani
            generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@%s%d\n",frame,counter,frame,macro,counter)); // jestli vetsi nez -1 tak true do bool
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter)); // pokud bool false skoc do error
            generate_to_list2(sprintf(list_str+list_length,"GT %s@bool%d int@256 %s@%s%d\n",frame,counter,frame,macro,counter)); // bool true pokud je mensi jak 256
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter)); // pokud bool false smoc na error
            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@newchar%d\n",frame,counter)); // novy znak
            generate_to_list2(sprintf(list_str+list_length,"INT2CHAR %s@newchar%d %s@%s%d\n",frame,counter,frame,macro,counter)); // preved na novy znak
            generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d %s@newchar%d\n",frame,macro,counter,frame,counter)); // uloz novy znak do func
            generate_to_list2(sprintf(list_str+list_length,"JUMP $label_converted%d\n",counter));
            generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter));
            generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
            generate_to_list2(sprintf(list_str+list_length,"LABEL $label_converted%d\n",counter));
        }
        else // zadana spatna ciselna hodnota
            errorHandling(4);

        //free(parsed);
    }

    else if(id == 0) { // funkce ord(s,i)

        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro, counter));

        char *str = arguments_parse(Root->content->name);
        long l = strlen(str);
        char s[l+1];
        char i[l+1];
        for(long a = 0; a < l+1; a++) {
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
        strcpy(i_help,i);
        strcpy(s_help,s);
        char* i_ptr;
        long i_ret = strtol(i_help,&i_ptr,10);

        if(parse_text) { // zpracovavali jsme primo retezec

            if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                if(i_ret > (long)(strlen(s_help)-1)) { // i je mimo rozsah
                    generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d string@nil\n",frame,macro,counter));
                }
                else { // index je v rozsahu
                    generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%s%d string@%s int@%s\n",frame,macro,counter,convert_string(s_help),i_help));
                    //free(asciistr);
                }
            }
            else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i_type%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help));
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_i_ok_%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$len%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help));
                generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@$len%d string@%s\n",frame,counter,convert_string(s_help))); // delku retezce do promenne len
                //free(asciistr);
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter)); // novy bool pro porovnani
                generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter)); // jestli vetsi nez -1 tak true do bool
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter)); // pokud bool false skoc do error
                generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter)); // bool true pokud je mensi jak 256
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter));

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
                generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%s%d string@%s %s@%s\n",frame,macro,counter,convert_string(s_help), frame, i_help));
                //free(asciistr);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_end_%d\n",counter));

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d string@nil\n",frame,macro,counter));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_end_%d\n",counter));
            }

            parse_text = false; // reset parse_text zpatky
        }
        else { // nepracovali jsme s primo textem, ale s promennou

            generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$s_type%d\n",frame,counter));
            generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$s_type%d %s@%s\n",frame,counter,frame,s_help));
            generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_ok_%d %s@$s_type%d string@string\n",counter,frame,counter));
            generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));

            generate_to_list2(sprintf(list_str+list_length,"LABEL $label_ok_%d\n",counter));

            if(strlen(i_ptr) == 0 && i_ret != 0) { // zadal tam platne cislo za i
                if(i_ret > l-1) { // i je mimo rozsah
                    generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d string@nil\n",frame,macro,counter));
                }
                else { // index je v rozsahu
                    generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
                    generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%s%d string@%s int@%s\n",frame,macro,counter,convert_string(s_help),i_help));
                    //free(asciistr);
                }
            }
            else if(strlen(i_ptr) > 0) { // zadal tam promennou misto cisla i
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i_type%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"TYPE %s@$i_type%d %s@%s\n",frame,counter,frame,i_help));
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_i_ok_%d %s@$i_type%d string@int\n",counter,frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_i_ok_%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$len%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@$i%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@$i%d %s@%s\n",frame, counter,frame,i_help));
                generate_to_list2(sprintf(list_str+list_length,"STRLEN %s@$len%d string@%s\n",frame,counter,convert_string(s_help))); // delku retezce do promenne len
                //free(asciistr);
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@bool%d\n",frame,counter)); // novy bool pro porovnani
                generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d int@-1 %s@$i%d\n",frame,counter,frame,counter)); // jestli vetsi nez -1 tak true do bool
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter)); // pokud bool false skoc do error
                generate_to_list2(sprintf(list_str+list_length,"LT %s@bool%d %s@$i%d %s@$len%d\n",frame,counter,frame,counter,frame,counter)); // bool true pokud je mensi jak 256
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error_%d %s@bool%d bool@true\n",counter,frame,counter));

                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
                generate_to_list2(sprintf(list_str+list_length,"GETCHAR %s@%s%d string@%s %s@%s\n",frame,macro,counter,convert_string(s_help), frame, i_help));
                //free(asciistr);
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_end_%d\n",counter));

                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d string@nil\n",frame,macro,counter));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_end_%d\n",counter));
            }
        }

        free(s_help);
        free(i_help);
        //free(str);

    }

    else if(id == 4) { // vestavena funkce inputi
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"READ %s@%s%d int\n",frame,macro,counter)); // nacti do promenne
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@intemp%d\n",frame,counter)); // uloz do tmp
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@intemp%d %s@%s%d\n",frame,counter,frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@intype%d\n",frame,counter));
        generate_to_list2(sprintf(list_str+list_length,"TYPE %s@intype%d %s@%s%d\n",frame,counter,frame,macro,counter)); // uloz typ
        generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_convert%d %s@intype%d string@float\n",counter,frame,counter)); // jestli je float zkonvertuj
        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_wrong_type%d %s@intype%d string@int\n",counter,frame,counter)); // jestli neni ani int tak 0
        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter)); // je to int

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_convert%d\n",counter));
        generate_to_list2(sprintf(list_str+list_length,"FLOAT2INT %s@%s%d %s@intemp%d\n",frame,macro,counter,frame,counter));
        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter));

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_wrong_type%d\n",counter));
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d int@0\n",frame,macro,counter));

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter));
    }
    else if(id == 5) { // vestavena funkce inputf
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"READ %s@%s%d float\n",frame,macro,counter)); // nacti do promenne
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@floatemp%d\n",frame,counter)); // uloz do tmp
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@floatemp%d %s@%s%d\n",frame,counter,frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@floatype%d\n",frame,counter));
        generate_to_list2(sprintf(list_str+list_length,"TYPE %s@floatype%d %s@%s%d\n",frame,counter,frame,macro,counter)); // uloz typ
        generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_convert%d %s@floatype%d string@int\n",counter,frame,counter)); // jestli je int zkonvertuj
        generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_wrong_type%d %s@floatype%d string@float\n",counter,frame,counter)); // jestli neni ani float tak 0.0
        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter)); // je to float

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_convert%d\n",counter));
        generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@%s%d %s@floatemp%d\n",frame,macro,counter,frame,counter));
        generate_to_list2(sprintf(list_str+list_length,"JUMP $label_inok%d\n",counter));

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_wrong_type%d\n",counter));
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d float@0.0\n",frame,macro,counter));

        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter));
    }
    else if(id == 6) { // vestavena funkce inputs
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"READ %s@%s%d string\n",frame,macro,counter)); // nacti do promenne
        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@strtype%d\n",frame,counter));
        generate_to_list2(sprintf(list_str+list_length,"TYPE %s@strtype%d %s@%s%d\n",frame,counter,frame,macro,counter)); // uloz typ
        generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_inok%d %s@strtype%d string@string\n",counter,frame,counter)); // jestli neni ani string tak nil
        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d string@nil\n",frame,macro,counter));
        generate_to_list2(sprintf(list_str+list_length,"LABEL $label_inok%d\n",counter));
    }
    else if(id == 2) { // vestavena funkce print
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

        generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%s%d\n",frame,macro,counter));

        for(int a = 0; a < l+1; a++) { // projizdim znak po znaku retezec s argumenty

            if((!second_time && (str[a] == ',' || str[a] == '\0')) || (second_time && parse_text && (str[a] == ',' || str[a] == '\0'))) { // oddelovac nebo konec, kdyz uz byla nalezena druha '"'

                strcpy(operand,s); // vytvoren retezec operand
                int_val = strtol(operand, &int_rest,10);
                float_val = strtof(operand,&float_rest);

                // tady pracuj s jednim operandem

                if(parse_text) { // operand je textovy retezec
                    generate_to_list2(sprintf(list_str+list_length,"WRITE string@%s\n",convert_string(operand)));
                    free(asciistr);
                    parse_text = false;
                    second_time = false;
                }
                else if(int_val != 0 && !strlen(int_rest)) { // operand je int
                    generate_to_list2(sprintf(list_str+list_length,"WRITE int@%ld\n",int_val));
                }

                else if(float_val != 0 && !strlen(float_rest)) { // operand je float
                    generate_to_list2(sprintf(list_str+list_length,"WRITE float@%a\n",float_val));
                }

                else { // operand je promenna
                    generate_to_list2(sprintf(list_str+list_length,"WRITE %s@%s\n",frame,operand)); // dodelat az to pujde testovat
                } // DODELAT

                // konec prace s operandem

                internal = 0;
                for(int b = 0; b < l+1; b++) // vynulovani pole
                    s[b] = '\0';
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

        generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%s%d nil@nil\n",frame,macro,counter)); // navrat print je vzdy nil@nil

        free(operand);
        //free(str);
        parse_text = false; // kdyby nahodou

    }

    if(!issingle) {
        // vloz do hodnot vysledky, se kterymi se dale bude pracovat
        generate_to_list2(sprintf(list_str + list_length, "MOVE %s@temp_%s$%d %s@%s%d\n", frame, Root->content->type, counter, frame, macro, counter));
        generate_to_list2(sprintf(list_str + list_length, "TYPE %s@type_%s$%d %s@%s%d\n", frame, Root->content->type, counter, frame, macro, counter));
    }
}

void type_control(tASTPointer* Root,char* operation, tQueue* q, char* frame, char* list_str) { // typova kontrola obsahu jednoho root uzlu (L + R)

        if(issingle) { // operace, ktere probihaji nad stromem se single node

            for(int i = 0; i < 7; i++) {
                if (!strcmp(Root->content->type, funkce[i])) {
                    call_function(i, frame, Root, list_str); // zavolej funkci call_function jestli je to nektera z funkci
                    counter++; // nezapomen na counter
                    return; // vyskoc pryc
                }
            }

            if(!strcmp(Root->content->type,"variable")) { // je to prirazeni promenne, eg. a = b
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@%s\n",frame,counter,frame,Root->content->name));
            }
            else { // zbyva prirazeni konstanty
                generate_to_list2(sprintf(list_str+list_length,"DEFVAR %s@%%%d\n",frame,counter));
                generate_to_list2(sprintf(list_str+list_length,"MOVE %s@%%%d %s@%s\n",frame,counter,Root->content->type,Root->content->name));
            }
            counter++; // operace probehla
            return ; // vyskoc ven
        }

        // ODTUD NENI SINGLE NODE

            // DEFINICE CO JE NA KAZDE STRANE: FUNKCE | OPERATOR | PROMENNA | STRING

            int left_func_id; // id funkce v seznamu
            int right_func_id; // id funkce v seznamu

            for(int i = 0; i < 7; i++) {
                if(!strcmp(Root->LeftPointer->content->type,funkce[i])) {
                    left_func = true; // leva je funkce
                    left_func_id = i;
                }

                if(!strcmp(Root->RightPointer->content->type,funkce[i])) {
                    right_func = true; // prava je funkce
                    right_func_id = i;
                }
            }
            if (!strcmp(Root->LeftPointer->content->name,"")) // vlevo je operator
                left_operator = true;
            else if (!strcmp(Root->LeftPointer->content->type, "variable")) // leva strana je VAR
                left = true;
            else if(!strcmp(Root->LeftPointer->content->type,"string")) // leva je retezec
                left_str = true;
            if (!strcmp(Root->RightPointer->content->name,"")) // vpravo je operator
                right_operator = true;
            else if (!strcmp(Root->RightPointer->content->type, "variable")) // prava strana je VAR
                right = true;
            else if(!strcmp(Root->RightPointer->content->type,"string")) // leva je retezec
                right_str = true;


            // GENEROVANI DEFVAR PRO L

            if(left_str) {
                left_supply = "lstr";
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d string@%s\n", frame, left_supply, counter,convert_string(Root->LeftPointer->content->name)));
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d string@%s\n", frame, left_supply, counter,convert_string(Root->LeftPointer->content->name)));
            }
            else if(left_func) {
                left_supply = Root->LeftPointer->content->type; // JMENO FUNKCE JAKO JEJI NAME
            }
            else if(left_operator) {
                int front;
                queueFront(q, &front);
                char buffer[10];
                sprintf(buffer, "%%%d", front);
                left_supply = buffer;
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%%%d\n", frame, front));
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%%%d\n", frame, front));
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front));
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front));
            }
            else if(left) {
                left_supply = Root->LeftPointer->content->name;
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply));
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, frame, left_supply));
            }
            else { // je to konstanta
                left_supply = Root->LeftPointer->content->name;
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, left_supply, counter));
                generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, left_supply, counter, Root->LeftPointer->content->type,left_supply));
                generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, left_supply, counter, Root->LeftPointer->content->type,left_supply));
            }

            // GENEROVANI R

    if(right_str) {
        right_supply = "rstr";
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d string@%s\n", frame, right_supply, counter,convert_string(Root->LeftPointer->content->name)));
        generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d string@%s\n", frame, right_supply, counter,convert_string(Root->LeftPointer->content->name)));
    }
    else if(right_func) {
        right_supply = Root->RightPointer->content->type; // JMENO FUNKCE JAKO JEJI NAME
    }
    else if(right_operator) {
        int front;
        if (left_operator) // pokud byla leva operator, sahni do fronty o jeden dal
            queuePreFront(q, &front);
        else // vlevo nebyl operator, sahni normalne
            queueFront(q, &front);
        char buffer[10];
        sprintf(buffer, "%%%d", front);
        right_supply = buffer;
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%%%d\n", frame, front));
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%%%d\n", frame, front));
        generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%%%d %s@%%%d\n", frame, front, frame, front));
        generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%%%d %s@%%%d\n", frame, front, frame, front));
    }
    else if(right) {
        right_supply = Root->RightPointer->content->name;
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply));
        generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, frame, right_supply));
    }
    else { // je to konstanta
        right_supply = Root->RightPointer->content->name;
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$type_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "DEFVAR %s@$temp_%s$%d\n", frame, right_supply, counter));
        generate_to_list2(sprintf(list_str+list_length, "TYPE %s@$type_%s$%d %s@%s\n", frame, right_supply, counter, Root->RightPointer->content->type,right_supply));
        generate_to_list2(sprintf(list_str+list_length, "MOVE %s@$temp_%s$%d %s@%s\n", frame, right_supply, counter, Root->RightPointer->content->type,right_supply));
    }

        // vypis vestavene funkce pro VSECHNY OPERACE (/*+-)
        if(left_func) { // vlevo je funkce
            call_function(left_func_id, frame,Root->LeftPointer,list_str);
        }
        if(right_func) { // vpravo je funkce
            call_function(right_func_id, frame,Root->RightPointer,list_str);
        }

        // ZPRACOVANI MATEM. OPERACI VE STROMU. pripominka: k vest funkcim se chovame jako k promennym a konst.! maji stejny vystup.
        // PRACUJEME s L R || L OP || OP L || OP OP, protoze operace narozd od zbytku ma jinou stukturu promenne pri tisku

        if(!strcmp(operation,"*") || !strcmp(operation,"-")) { // pro MULL a SUB oba musi byt float/int

            if(!left_operator && !right_operator) { // L R
                if ((left && right) || (left_func && right_func)) { // L R pro funkce a promenne
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n", counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n", counter, frame, right_supply, counter)); // levy je int, otestuj pravy na int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, right_supply, counter)); // pokud pravy neni ani float, chyba
                    generate_to_list2(sprintf(list_str + list_length, "INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply,counter, frame, left_supply)); // pravy je float, levy preved na float
                    generate_to_list2(sprintf(list_str + list_length, "JUMP $label_same_types$%d\n", counter)); // skoc na konec
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_left_not_int$%d\n", counter)); // levy nebyl int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, left_supply, counter)); // zkus jestli neni float, jestli ne tak chyba
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n", counter,frame, right_supply, counter)); // je to float, otestuj jestli neni druha taky float
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter, frame, right_supply, counter)); // otestuj jestli neni druha int
                    generate_to_list2(sprintf(list_str + list_length, "INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply,counter, frame, right_supply)); // druha je int, preved na float
                    generate_to_list2(sprintf(list_str + list_length, "JUMP $label_same_types$%d\n", counter));
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_error$%d\n", counter)); // chyba typu
                    generate_to_list2(sprintf(list_str + list_length, "ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_same_types$%d\n", counter));
                }
                else if(!left && !right && !left_func && !right_func) { // L R pro konstanty
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if (!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, Root->LeftPointer->content->name, counter, frame, Root->LeftPointer->content->name, counter));
                        } else { // int je vpravo, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, Root->RightPointer->content->name, counter, frame, Root->RightPointer->content->name, counter));
                        }
                    }
                }
            }
            else if(left_operator && right_operator) { // OP OP
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n", counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n", counter, frame, right_supply, counter)); // levy je int, otestuj pravy na int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, right_supply, counter)); // pokud pravy neni ani float, chyba
                    generate_to_list2(sprintf(list_str + list_length, "INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, left_supply,counter, frame, left_supply)); // pravy je float, levy preved na float
                    generate_to_list2(sprintf(list_str + list_length, "JUMP $label_same_types$%d\n", counter)); // skoc na konec
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_left_not_int$%d\n", counter)); // levy nebyl int
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter, frame, left_supply, counter)); // zkus jestli neni float, jestli ne tak chyba
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n", counter,frame, right_supply, counter)); // je to float, otestuj jestli neni druha taky float
                    generate_to_list2(sprintf(list_str + list_length, "JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter, frame, right_supply, counter)); // otestuj jestli neni druha int
                    generate_to_list2(sprintf(list_str + list_length, "INT2FLOAT %s@$temp_%s$%d %s@%s\n", frame, right_supply,counter, frame, right_supply)); // druha je int, preved na float
                    generate_to_list2(sprintf(list_str + list_length, "JUMP $label_same_types$%d\n", counter));
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_error$%d\n", counter)); // chyba typu
                    generate_to_list2(sprintf(list_str + list_length, "ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str + list_length, "LABEL $label_same_types$%d\n", counter));
            }
            else { // L OP || OP R
                if(left_operator) {// OP R
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_type$%d %s@$type_%s %s@$type_%s$%d\n",counter,frame, left_supply, frame, right_supply, counter)); // porovnej typ s typem praveho operatoru
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_int$%d %s@$type_%s string@int\n",counter, frame, left_supply)); // porovnej typ s intem
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_float$%d %s@$type_%s string@float\n",counter, frame, left_supply)); // porovnej typ s floatem
                    generate_to_list2(sprintf(list_str+list_length,"EXIT int@4\n")); // chyba 4
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter)); // je to int, preved ho na float
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, left_supply,frame, left_supply));
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_type$%d\n",counter)); // prevedena hodnota promenne je ted v temp_
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_float$%d\n",counter)); // je to float, preved R na float
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, right_supply, counter, frame, right_supply,counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_type$%d\n",counter)); // proved operaci, jsou stejnyho typu
                }
                else {// L OP
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_type$%d %s@$type_%s$%d %s@$type_%s\n",counter,frame, left_supply, counter, frame, right_supply)); // porovnej typ s typem praveho operatoru
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_int$%d %s@$type_%s string@int\n",counter, frame, right_supply)); // porovnej typ s intem
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_type_float$%d %s@$type_%s string@float\n",counter, frame, right_supply)); // porovnej typ s floatem
                    generate_to_list2(sprintf(list_str+list_length,"EXIT int@4\n")); // chyba 4
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_int$%d\n",counter)); // je to int, preved ho na float
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, right_supply,frame, right_supply));
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_type$%d\n",counter)); // prevedena hodnota promenne je ted v temp_
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_type_float$%d\n",counter)); // je to float, preved R na float
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, left_supply, counter, frame, left_supply,counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_type$%d\n",counter)); // proved operaci, jsou stejnyho typu

                }
            }
        }


        else if(!strcmp(operation,"+")) { // pro ADD oba musi byt float/int || string

            if(!left_operator && !right_operator) { // L R
                if((left && right) || (left_func && right_func) || (left_str && right_str)) { // L R promenne a funkce a retezce
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter)); // skoc pokud neni levej string
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter)); // proved concat jestli je pravej taky string
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_concat_op$%d\n",counter)); // skoc na concat
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter)); // je jasne ze to neni retezec
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter)); // levy je int, otestuj pravy na int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter)); // pokud pravy neni ani float, chyba
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n",frame, left_supply, counter, frame, left_supply, counter)); // pravy je float, levy preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter)); // skoc na konec
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter)); // levy nebyl int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter)); // zkus jestli neni float, jestli ne tak chyba
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter)); // je to float, otestuj jestli neni druha taky float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter)); // otestuj jestli neni druha int
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n",frame, right_supply, counter, frame, right_supply, counter)); // druha je int, preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
                else if(!left && !right && !left_func && !right_func && !right_str && !left_str) { // L R konstanty
                    if (strcmp(Root->LeftPointer->content->type, Root->RightPointer->content->type)) { // pokud maji konstanty jiny typ
                        if((!strcmp(Root->LeftPointer->content->type,"string")) || (!strcmp(Root->RightPointer->content->type,"string"))) { // jeden z nich string a nejsou stejne typy, chyba
                            generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                            errorHandling(4);
                        }
                        else if(!strcmp(Root->LeftPointer->content->type, "int")) { // pokud je vlevo int, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n",frame, Root->LeftPointer->content->name, counter, frame, Root->LeftPointer->content->name, counter));
                        } else { // int je vpravo, preved ho na float
                            generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, Root->RightPointer->content->name, counter, frame, Root->RightPointer->content->name, counter));
                        }
                    }
                    else {
                        if((!strcmp(Root->LeftPointer->content->type,"string")) || (!strcmp(Root->RightPointer->content->type,"string"))) {
                            generate_to_list2(sprintf(list_str+list_length,"JUMP $label_concat_op$%d\n",counter)); // skoc na concat
                        }
                    }
                }
            }
            else if(left_operator && right_operator) { // OP OP
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s string@string\n",counter,frame, left_supply)); // skoc pokud neni levej string
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@string\n",counter,frame, right_supply)); // proved concat jestli je pravej taky string
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_concat_op$%d\n",counter)); // skoc na concat
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter)); // je jasne ze to neni retezec
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s string@int\n",counter,frame, left_supply)); // skoc pokud je levy jiny nez int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s string@int\n",counter,frame, right_supply)); // levy je int, otestuj pravy na int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // pokud pravy neni ani float, chyba
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, left_supply, frame, left_supply)); // pravy je float, levy preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter)); // skoc na konec
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter)); // levy nebyl int
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, left_supply)); // zkus jestli neni float, jestli ne tak chyba
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // je to float, otestuj jestli neni druha taky float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@int\n",counter,frame, right_supply)); // otestuj jestli neni druha int
                generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, right_supply, frame, right_supply)); // druha je int, preved na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
            }
            else { // OP R || L OP
                if(left_operator) { // OP R
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s string@string\n",counter,frame, left_supply)); // skoc pokud neni levej string
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@string\n",counter,frame, right_supply, counter)); // proved concat jestli je pravej taky string
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_concat_op$%d\n",counter)); // skoc na concat
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter)); // je jasne ze to neni retezec
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s string@int\n",counter,frame, left_supply)); // skoc pokud je levy jiny nez int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter)); // levy je int, otestuj pravy na int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter)); // pokud pravy neni ani float, chyba
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, left_supply, frame, left_supply)); // pravy je float, levy preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter)); // skoc na konec
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter)); // levy nebyl int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, left_supply)); // zkus jestli neni float, jestli ne tak chyba
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter)); // je to float, otestuj jestli neni druha taky float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@int\n",counter,frame, right_supply, counter)); // otestuj jestli neni druha int
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n",frame, right_supply, counter, frame, right_supply, counter)); // druha je int, preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
                else { // L OP
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_string$%d %s@$type_%s$%d string@string\n",counter,frame, left_supply, counter)); // skoc pokud neni levej string
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@string\n",counter,frame, right_supply)); // proved concat jestli je pravej taky string
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_concat_op$%d\n",counter)); // skoc na concat
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_string$%d\n",counter)); // je jasne ze to neni retezec
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_left_not_int$%d %s@$type_%s$%d string@int\n",counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s string@int\n",counter,frame, right_supply)); // levy je int, otestuj pravy na int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // pokud pravy neni ani float, chyba
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s$%d %s@$temp_%s$%d\n",frame, left_supply, counter, frame, left_supply,counter)); // pravy je float, levy preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter)); // skoc na konec
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_left_not_int$%d\n",counter)); // levy nebyl int
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter)); // zkus jestli neni float, jestli ne tak chyba
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_same_types$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // je to float, otestuj jestli neni druha taky float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@int\n",counter,frame, right_supply)); // otestuj jestli neni druha int
                    generate_to_list2(sprintf(list_str+list_length,"INT2FLOAT %s@$temp_%s %s@$temp_%s\n",frame, right_supply, frame, right_supply)); // druha je int, preved na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
            }
        }

        else { // DIV: musi byt oba float a pravej nesmi byt nula

            if(!left_operator && !right_operator) { // L R
                if((left && right) || (left_func && right_func)) { // L R promenne a funkce
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s$%d int@0\n",counter,frame, right_supply, counter)); // porovnani s int 0
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s$%d float@0.0\n",counter,frame, right_supply, counter)); // porovnani s float 0 // BUG
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply, counter)); // levy je float, otestuj pravy na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter)); // chyba deleni
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
                else if(!left && !right && !left_func && !right_func) { // L R konstanty
                    if(!strcmp(right_supply,"0") || !strcmp(right_supply,"0.0")) { // BUG: pridej i exponencialni tvar
                        generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"));
                        errorHandling(9);
                    }
                }
            }
            else if(left_operator && right_operator) { // OP OP
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s int@0\n",counter,frame, right_supply)); // porovnani s int 0
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s float@0.0\n",counter,frame, right_supply)); // porovnani s float 0 // BUG
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, left_supply)); // skoc pokud je levy jiny nez float
                generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // levy je float, otestuj pravy na float
                generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter)); // chyba deleni
                generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"));
                generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
            }
            else { // OP R || L OP
                if(left_operator) { // OP R
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s$%d int@0\n",counter,frame, right_supply, counter)); // porovnani s int 0
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s$%d float@0.0\n",counter,frame, right_supply, counter)); // porovnani s float 0 // BUG
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, left_supply)); // skoc pokud je levy jiny nez float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, right_supply,counter)); // levy je float, otestuj pravy na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter)); // chyba deleni
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
                else {
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s int@0\n",counter,frame, right_supply)); // porovnani s int 0
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFEQ $label_error_div$%d %s@$temp_%s float@0.0\n",counter,frame, right_supply)); // porovnani s float 0 // BUG
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s$%d string@float\n",counter,frame, left_supply, counter)); // skoc pokud je levy jiny nez float
                    generate_to_list2(sprintf(list_str+list_length,"JUMPIFNEQ $label_error$%d %s@$type_%s string@float\n",counter,frame, right_supply)); // levy je float, otestuj pravy na float
                    generate_to_list2(sprintf(list_str+list_length,"JUMP $label_same_types$%d\n",counter));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error$%d\n",counter)); // chyba typu
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@4\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_error_div$%d\n",counter)); // chyba deleni
                    generate_to_list2(sprintf(list_str+list_length,"ERROR int@9\n"));
                    generate_to_list2(sprintf(list_str+list_length,"LABEL $label_same_types$%d\n",counter));
                }
            }
        }

}



void postorder(tASTPointer* Root, tQueue* q, tFunctionTracker* functionTracker, char* list_str) { // rekurzivni postorder pro postupne generovani vyrazu v generate_expression(AST)

    if (Root == NULL)
        return ;
    postorder(Root->LeftPointer,q, functionTracker, list_str);
    postorder(Root->RightPointer,q, functionTracker, list_str);

    char* frame = get_frame(functionTracker); // vyhledej ve ktere jsme funkci
    if(issingle || (Root->LeftPointer != NULL && Root->RightPointer != NULL)) // vylouceni listu z type_control, krome single node
        type_control(Root, Root->ID,q,frame, list_str); // typova kontrola

    char* op;
    bool printOp = false; // vytiskni operaci pro +-*/
    if(!strcmp(Root->ID,"+")) {
        op = "ADD";
        printOp = true;
    }
    else if(!strcmp(Root->ID,"-")) {
        op = "SUB";
        printOp = true;
    }
    else if(!strcmp(Root->ID,"*")) {
        op = "MUL";
        printOp = true;
    }
    else if(!strcmp(Root->ID,"/")) {
        op = "DIV";
        printOp = true;
    }
    else if(!strcmp(Root->ID,"==")) {
        ;
    }
    else {
        return;
    }

    if(!strcmp(Root->ID,"==")) {

        int leftvar;
        int rightvar;
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@comp_l$%i\n", frame, counter));
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@comp_r$%i\n", frame, counter));

        if (!left_operator && !right_operator) { // ani jeden z L R neni operator, tisk operace
            generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_l$%i %s@$temp_%s$%d\n", frame, counter, frame, left_supply, counter));
            generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_r$%i %s@$temp_%s$%d\n", frame, counter, frame, right_supply, counter));
        } else if ((left_operator && !right_operator) || (!left_operator && right_operator)) { // jeden z L R je operace
            if (left_operator) { // L je operator
                queueGet(q, &leftvar);
                generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_l$%i %s@$temp_%%%i\n", frame, counter, frame, leftvar));
                generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_r$%i %s@$temp_%s$%d\n", frame, counter, frame, right_supply, counter));
            } else { // R je operator
                queueGet(q, &rightvar);
                generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_l$%i %s@$temp_%s$%d\n", frame, counter, frame, left_supply, counter));
                generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_r$%i %s@$temp_%%%i\n", frame, counter, frame, rightvar));
            }
        } else { // tisk operace kdyz je operator L i R
            queueGet(q, &leftvar);
            queueGet(q, &rightvar);
            generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_l$%i %s@$temp_%%%i\n", frame, counter, frame, leftvar));
            generate_to_list2(sprintf(list_str + list_length, "MOVE %s@comp_r$%i %s@$temp_%%%i\n", frame, counter, frame, rightvar));
        }
    }

    if(printOp) {
        generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@%%%i\n", frame, counter)); // operace, chystam tedy novou promennou
        int leftvar; // leva strana
        int rightvar; // prava strana

        // TISK OPERACE START
        if (!left_operator && !right_operator) { // ani jeden z L R neni operator, tisk operace
            generate_to_list2(sprintf(list_str + list_length, "%s %s@%%%i %s@$temp_%s$%d %s@$temp_%s$%d\n", op, frame, counter, frame, left_supply, counter, frame, right_supply, counter));
        } else if ((left_operator && !right_operator) || (!left_operator && right_operator)) { // jeden z L R je operace
            // tisk operace kdyz je pouze jedna strana (L || R) operaator
            if (left_operator) { // L je operator
                queueGet(q, &leftvar);
                generate_to_list2(sprintf(list_str + list_length, "%s %s@%%%i %s@$temp_%%%i %s@$temp_%s$%d\n", op, frame, counter, frame, leftvar, frame, right_supply, counter));
            } else { // R je operator
                queueGet(q, &rightvar);
                generate_to_list2(sprintf(list_str + list_length, "%s %s@%%%i %s@$temp_%s$%d %s@$temp_%%%i\n", op, frame, counter, frame, left_supply, counter, frame, rightvar));
            }
        } else { // tisk operace kdyz je operator L i R
            queueGet(q, &leftvar);
            queueGet(q, &rightvar);
            generate_to_list2(sprintf(list_str + list_length, "%s %s@%%%i %s@%%%i %s@%%%i\n", op, frame, counter, frame, leftvar, frame, rightvar));
        }

        if(!strcmp(Root->ID,"+")) { // concat ukaz jen kdyz je tam +
            generate_to_list2(sprintf(list_str + list_length, "JUMP $label_op_finished$%i\n", counter)); // skoc tesne za concat op

            // FAKE CONCAT START
            generate_to_list2(sprintf(list_str + list_length, "LABEL $label_concat_op$%i\n", counter));
            generate_to_list2(sprintf(list_str + list_length, "DEFVAR %s@%%%i\n", frame, counter));
            if (!left_operator && !right_operator) { // ani jeden z L R neni operator, tisk operace
                generate_to_list2(sprintf(list_str +
                                          list_length, "CONCAT %s@%%%i %s@$temp_%s$%d %s@$temp_%s$%d\n", frame, counter, frame, left_supply, counter, frame, right_supply, counter));
            } else if ((left_operator && !right_operator) ||
                       (!left_operator && right_operator)) { // jeden z L R je operace
                // tisk operace kdyz je pouze jedna strana (L || R) operaator
                if (left_operator) { // L je operator
                    queueGet(q, &leftvar);
                    generate_to_list2(sprintf(list_str +
                                              list_length, "CONCAT %s@%%%i %s@$temp_%%%i %s@$temp_%s$%d\n", frame, counter, frame, leftvar, frame, right_supply, counter));
                } else { // R je operator
                    queueGet(q, &rightvar);
                    generate_to_list2(sprintf(list_str +
                                              list_length, "CONCAT %s@%%%i %s@$temp_%s$%d %s@$temp_%%%i\n", frame, counter, frame, left_supply, counter, frame, rightvar));
                }
            } else { // tisk operace kdyz je operator L i R
                queueGet(q, &leftvar);
                queueGet(q, &rightvar);
                generate_to_list2(sprintf(list_str +
                                          list_length, "CONCAT %s@%%%i %s@%%%i %s@%%%i\n", frame, counter, frame, leftvar, frame, rightvar));
            }
            // FAKE CONCAT END

            generate_to_list2(sprintf(list_str + list_length, "LABEL $label_op_finished$%i\n", counter));
        }

        // TISK OPERACE END

    }

    queueUp(q,counter); // nahravas do fronty pokazde kdyz delas vyraz, kde je root operator
    // reset promennych pro typ tisku operace
    left_operator = false;
    right_operator = false;
    left_func = false;
    right_func = false;
    left_str = false;
    right_str = false;
    left = false;
    right = false;
    counter++; // pricti 1 k promenne
}

void generateExpression(tASTPointer* AST, tFunctionTracker* functionTracker, char* list_str, bool comp) {

        char* frame = get_frame(functionTracker);
        tQueue* q = malloc(sizeof(tQueue)); // nova fronta pro generate_expression
        queueInit(q); // inicializuj frontu
        if(AST->LeftPointer == NULL && AST->RightPointer == NULL) {
            issingle = true;
        }

        postorder(AST,q,functionTracker, list_str); // rekurzivni postorder stromem

        if(!comp) { // pokud se nejedna o porovnavani
            generate_to_list2(sprintf(list_str +list_length, "DEFVAR %s@%%assign%d\n", frame, assign)); // cilova hodnota vyrazu, NEXT mozna pojmenovat s counter kvuli originalite
            generate_to_list2(sprintf(list_str + list_length, "MOVE %s@%%assign%d %s@%%%i\n", frame, assign, frame,counter - 1)); // do %assign dej posledni hodnotu counteru - po pricteni
            ++assign;
        }

        issingle = false;
        free(q); // uvolni frontu

}

void generateCode(char* stackTop, int rules[]) {
    printf("STACK: %s\n",stackTop);
    for(int i = 0; i < 50; i++)
        printf("rule: %d\n",rules[i]);
    printf("GEN FINISHED.\n");
}