/**
 * Source file for syntax driven compilation
 *
 * Implemented by Gabriel Quirschfeld   xquirs00
 *                Marek Varga           xvarga14
 */
#include "syntax.h"
#include "scanner.h"
#include "symtable.h"
#include "queue.h"
#include "generate.h"
#include "if-generate.h"

#define BUF_SIZE 1024

/**
 * Function initializes stack for function tracking.
 *
 * @param stack
 */
void tFunctionTrackerInit(tFunctionTracker* stack) {
    stack->top = 1;
    stack->function[0] = malloc(5);
    stack->function[0] = "Main";
}

/**
 * Function disposes stack.
 *
 * @param stack
 */
void tFunctionTrackerDispose(tFunctionTracker* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        while (stack->top != 0) {
            stack->function[stack->top] = NULL;
            stack->top--;
        }
    }
}

/**
 * Function pushes new function in the stack.
 *
 * @param stack
 * @param pushedFunction
 */
void tFunctionTrackerPush(tFunctionTracker* stack, char* pushedFunction) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->function[stack->top] = malloc(sizeof(strlen(pushedFunction))+1);
        checkMalloc(stack->function[stack->top]);
        stack->function[stack->top] = pushedFunction;
        stack->top++;
    }
}

/**
 * Function pops function from stack.
 *
 * @param stack
 */
void tFunctionTrackerPop(tFunctionTracker* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        stack->function[stack->top-1] = NULL;
        stack->top--;
    }
}

/**
 * Function gets top function from function tracker.
 *
 * @param stack
 * @return pointer to char is name of top function in funciton tracker.
 */
char* tFunctionTrackerGetTop(tFunctionTracker* stack) {
    if (stack == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        return stack->function[stack->top-1];
    }
}

/**
 * Function that does all the work - syntax driven compilation.
 */
void doMagic() {

    if (feof(stdin))
        printf("file reached eof\n");
    void *content = malloc(BUF_SIZE);
    FILE *fp = fopen("test.txt", "w");
    if (fp == 0)
        printf("...something went wrong opening file...\n");

    int read;
    while ((read = fread(content, 1, BUF_SIZE, stdin))) {
        printf("Read %d bytes", read);
        fwrite(content, read, 1, fp);
        printf("Writing %d\n", read);
    }
    if (ferror(stdin))
        printf("There was an error reading from file");

    printf("Done writing\n");

    fclose(fp);

    FILE *list = fopen("./list.txt", "w+");
    FILE *file = fopen("./test.txt", "r");

    //FILE *file = fopen("../test.txt", "r");
    // zacatek programu

    fprintf(list,".ifjcode18\n");
    fprintf(list,"CREATEFRAME\n");

    BSTNodeContentPtr* tmp;
    int not_int = 0;            // true if variable is float or string
    int after_eq = 0;           // true if we are assigning something to a variable
    int num_of_func_params = 0; // stores the numbers of called params a function has
    int func_params = 0;        // true if function has params
    int is_stat = 0;            // true if we are inside of a statement
    int is_global = 1;          // true if we are outside of a function
    int is_func = 0;            // true if token was def, expecting function id next
    int undef = 0;              // true if token was equals sign, expecting only defined identifiers
    int arr_id = 0;             // id for the array holding local symtables
    unsigned long func_id = 0;  // function id to be put into local id content
    unsigned long var_id = 0;   // variable id to be used when assigning variable a type (int, float or string)

    BSTNodePtr* global_symtable = malloc(sizeof(struct BSTNode));       // global symtable storing the function ids
    BSTInit(global_symtable);

    struct BSTNode **array;         // array storing local symtables
    array = malloc(10000 * sizeof(struct BSTNode *));
    for (int i = 0; i < 10000; i++) {
        array[i] = NULL;
    };

    BSTNodeContentPtr* cnt;

    // first transit of compiler -- filling out symtable
    Token tmpToken = global_token;
    while (global_token.type != ss_eof) {
        token_generate(file);   // calling lexical analysis to get another token
        cnt = malloc(sizeof(struct BSTNodeContent));
        if (global_token.type == kw_def) {  // after the def keyword we are expecting the function id, all id in function are local
            is_func = 1;
            is_global = 0;
        }
        else if (global_token.type == kw_if || global_token.type == kw_while) {  // we are inside an if or while statement until we get an end
            is_stat = 1;
        }
        else if (global_token.type == kw_end && is_stat == 0) {  // ending the function
            is_global = 1;
        }
        else if (global_token.type == kw_end && is_stat == 1) {  // ending the if or while statements
            is_stat = 0;
        }
        if ((is_func == 1) && (global_token.type == s_id)) {    // push the function id into the global symtable
            cnt->type = "function";
            cnt->defined = 1;
            cnt->name = global_token.content;
            cnt->func_params = 0;
            BSTInsert(global_symtable, cnt, hash_id(global_token.content), 0);  // inserts the function id into the global symtable
            BSTNodePtr* local_symtable = malloc(sizeof(struct BSTNode));    // allocating memory for a new local symtable storing local identifiers
            BSTInit(local_symtable);    // initialization of the local symtable
            array[arr_id] = *local_symtable;    // storing the local symtable into the array of local symtables
            arr_id++;
            func_id = hash_id(global_token.content);    // storing the hash of function id for later use
            is_func = 0;    // no longer expecting a function id
            func_params = 1;
            num_of_func_params = 0;
        }
        else if ((func_params == 1) && (global_token.type == ss_eol)) {     // if there are no more function params
            func_params = 0;
            num_of_func_params = 0;
        }
        else if (global_token.type == ss_eol) { // the undefined region resets after eol
            undef = 0;
            after_eq = 0;
            not_int = 0;
        }
        else if (global_token.type == s_eq || global_token.type == kw_if || global_token.type == kw_while) {   // after an equals sign, if and while only already defined ids can be used
            undef = 1;
            if (global_token.type == s_eq) {
                after_eq = 1;
            }
        }
        else if ((func_params == 1) && (global_token.type == s_id)) {   // pushes function params into local symtable and changes the number of params in global symtable
            cnt->type = "func_parameter";
            cnt->defined = 1;
            cnt->name = global_token.content;
            cnt->func_params = 0;
            BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
            num_of_func_params++;
            cnt = malloc(sizeof(struct BSTNodeContent));
            cnt->type = "function";
            cnt->defined = 1;
            cnt->name = (*global_symtable)->content->name;
            cnt->func_params = num_of_func_params;
            BSTInsert(global_symtable, cnt, func_id, 0);
        }
        else if ((undef == 1) && (global_token.type == s_id)) { // controls if ids after the equals sign, if and while statements are defined
            if (((array[0] == NULL) && (*global_symtable == NULL)) || ((array[0] == NULL) && (BSTSearch(global_symtable, hash_id(global_token.content)) == NULL))) {
                errorHandling(3);
                return;
            }
            else if (arr_id > 0) {
                if ((BSTSearch(&array[arr_id-1], hash_id(global_token.content)) == NULL) && (BSTSearch(global_symtable, hash_id(global_token.content)) == NULL)) {   // if the identifier was not used before, it is not defined
                    errorHandling(3);
                    return;
                }
                else {  // if the identifier was used before, it is defined
                    if (BSTSearch(global_symtable, hash_id(global_token.content)) != NULL) {    // if the identifier is a function id it is added as such
                        cnt->type = "function";
                        cnt->defined = 1;
                        cnt->name = global_token.content;
                        cnt->func_params = 0;
                        BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
                    }
                    else if ((tmp = BSTSearch(&array[arr_id-1], hash_id(global_token.content))) != NULL) {  // if the identifier is already in the local symtable
                        BSTInsert(&array[arr_id-1], tmp, hash_id(global_token.content), func_id);
                    }
                    else {  // otherwise it is added as a variable
                        cnt->type = "variable";
                        cnt->defined = 1;
                        cnt->name = global_token.content;
                        cnt->func_params = 0;
                        BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
                    }
                }
            }
        }
        else if ((is_global == 1) && (global_token.type == s_id)) {     // pushes the global ids into global symtable
            if (BSTSearch(global_symtable, hash_id(global_token.content)) == NULL) {
                cnt->type = "variable";
                cnt->defined = 1;
                cnt->name = global_token.content;
                cnt->func_params = 0;
                BSTInsert(global_symtable, cnt, hash_id(global_token.content), 0);  // inserts the function id into the global symtable
                var_id = hash_id(global_token.content);
            }
        }
        else if (global_token.type == s_id) {   // push the id into the local symtable for the specific function
            if (BSTSearch(global_symtable, hash_id(global_token.content)) != NULL) {    // if the identifier is a function id it is added as such
                cnt->type = "function";
                cnt->defined = 1;
                cnt->name = global_token.content;
                cnt->func_params = 0;
                BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
            }
            else if ((tmp = BSTSearch(&array[arr_id-1], hash_id(global_token.content))) != NULL) {  // if the identifier is already in the local symtable
                BSTInsert(&array[arr_id-1], tmp, hash_id(global_token.content), func_id);
            }
            else {  // otherwise it is added as a variable
                cnt->type = "variable";
                cnt->defined = 1;
                cnt->name = global_token.content;
                cnt->func_params = 0;
                BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
                var_id = hash_id(global_token.content);
            }
        }
        else if ((after_eq == 1) && (global_token.type == s_string)) {
            if ((tmp = BSTSearch(global_symtable, var_id)) != NULL) {
                tmp->var = "string";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
            else if ((tmp = BSTSearch(&array[arr_id-1], var_id)) != NULL) {
                tmp->var = "string";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
            not_int = 1;
        }
        else if ((after_eq == 1) && (global_token.type == s_float || global_token.type == s_exp_f)) {
            if ((tmp = BSTSearch(global_symtable, var_id)) != NULL) {
                tmp->var = "float";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
            else if ((tmp = BSTSearch(&array[arr_id-1], var_id)) != NULL) {
                tmp->var = "float";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
            not_int = 1;
        }
        else if ((after_eq == 1) && (not_int != 1) && (global_token.type == s_int || global_token.type == s_exp_int)) {
            if ((tmp = BSTSearch(global_symtable, var_id)) != NULL) {
                tmp->var = "integer";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
            else if ((tmp = BSTSearch(&array[arr_id-1], var_id)) != NULL) {
                tmp->var = "integer";
                BSTInsert(global_symtable, tmp, var_id, 0);  // inserts the function id into the global symtable
            }
        }
    }

    rewind(file);   // rewinding the file for another transit

        // second transit of compiler -- passing tokens to parser
        // helper stacks
        tASTPointer *AST = malloc(sizeof(struct tAST) * 2);
        tASTInit(AST);                          // AST - abstract syntax tree - contains expression after precedence SA finished (down top SA)
        tStackPredictive *predictiveStack = malloc(sizeof(tStackPredictive) * 30);
        tStackPredictiveInit(predictiveStack);  // contains rules meant to be expanded in predictive SA (top down SA)
        tExpendedStack *expendedStack = malloc(sizeof(tExpendedStack) * 30);
        init(expendedStack);                    // contains symbols meant to be simplified in precedence SA
        tStackASTPtr *stackAST = malloc(sizeof(struct tStackAST) * 20);
        tStackASTInit(stackAST);                // helper stack for precedence SA, contains nodes meant to be merged together
        global_token = tmpToken;
        //char *currentFunction = "";
        tFunctionTracker* functionTracker = malloc(sizeof(struct tFT) * 12);            // helper stack to keep track of the function name we are currently in
        tFunctionTrackerInit(functionTracker);

        while (global_token.type != ss_eof && ERROR_TYPE == 0) {
            // call lexical analysis
            token_generate(file);
            // call syntax analysis
                if (precedence == 1 || ((global_token.type == s_int || global_token.type == s_float || global_token.type == s_exp_int || global_token.type == s_exp_int_s || global_token.type == s_exp_f || global_token.type == s_exp_f_s) && checkingArgs == 0)) {
                    // we are dealing with expression => doing down top syntax analysis => need to simulate precedence
                    precedence = 1;
                    if (global_token.type == s_id) {
                        // if current token is id => need to call next token to decide whether current token is variable or function id
                        tmpToken = global_token;
                        token_generate(file);
                        tmpToken.type = decideID(global_token);
                        // simulate predictive SA for previous token
                        simulatePrecedence(tmpToken, expendedStack, stackAST, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)), global_symtable);
                    }
                    // simulate predictive SA for current token
                    simulatePrecedence(global_token, expendedStack, stackAST, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)), global_symtable);

		    // PRECEDENCNI ANALYZA EXPRESSION (SEM  SE VOLA Z PREDIKTIVNI)
                    if (precedence == 0) {
                        // precedence has finished => need to pop rule from predictive stack
                        tStackPredictivePop(predictiveStack);
                        // assign newly created AST
                        if (stackAST != NULL && ERROR_TYPE == 0) {
                            // result of precedence will be stored in AST - abstract syntax tree
                            *AST = *stackAST->body[stackAST->top];
                            if (ifStatement == 1 && global_token.type == kw_then) {
                                generateIfHead(stackAST->body[stackAST->top]);
                            }
                            if (whileStatement == 1 && global_token.type == kw_do) {
                                generateWhileHead(stackAST->body[stackAST->top]);
                            }

                            //generateExpression(AST,functionTracker,list); // vygeneruj do seznamu instrukce vyrazu

                            // po vygenerovani vyrazu ho prirad zadane promenne
                            //char *frame = get_frame(functionTracker);
                            //fprintf(list, "MOVE %s@%s %s@%%assign%d\n", frame, tmpToken.content, frame,assign);
                            assign++;

                            // clear tree after generating
                            AST = malloc(sizeof(struct tAST) * 2);
                        }
                    }
                }

		// PREDIKTIVNI (AUTOMATICKA)
                if (precedence == 0) {
                    // we are not dealing with expression => doing top down syntax analysis => need to simulate predictive SA
                    if (global_token.type == s_id) {
                        // if current token is id => need to call next token to decide whether current token is variable or function id
                        tmpToken = global_token;
                        token_generate(file);
                        tmpToken.type = decideID(global_token);
                        if (tmpToken.type == s_func_id && strcmp(predictiveStack->content[predictiveStack->top-1], "<assign>") != 0) {
                            // helper tracker stack of function names to keep track in which function we are in
                            tFunctionTrackerPush(functionTracker, tmpToken.content);
                        }
                        // simulate predictive SA for current token
                        simulatePredictive(tmpToken, predictiveStack, global_symtable, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)));
                        if (precedence == 1) {
                            simulatePrecedence(tmpToken, expendedStack, stackAST, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)), global_symtable);
                        }
                    }
                    if (printing == 1) {
                        // need to print this expression
                        generatePrint(&tmpToken);
                        //generateCodeParek(&tmpToken);
                        // todo: generate code
                        /*
                         * Previous token was print => generate stuff that needs to be printed. Current token (global_token.content) contains expression for printing.
                         */
                        // pop <print-expr> rule from stack
                        tStackPredictivePop(predictiveStack);
                        // we will not be printing anymore
                        printing = 0;
                    }
                    if (precedence == 0) {
                        // simulate predictive SA for next token
                        simulatePredictive(global_token, predictiveStack, global_symtable, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)));

                    }
                    if (precedence == 1){
                        simulatePrecedence(global_token, expendedStack, stackAST, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)), global_symtable);
                        //simulatePrecedence(global_token, expendedStack, stackAST, findNode(array, global_symtable, currentFunction));
                        if (precedence == 0) {
                            // precedence has finished => need to pop rule from predictive stack
                            tStackPredictivePop(predictiveStack);
                            // assign newly created AST
                            if (stackAST != NULL && ERROR_TYPE == 0) {
                                // result of precedence will be stored in AST - abstract syntax tree
                                *AST = *stackAST->body[stackAST->top];
                                if (ifStatement == 1 && global_token.type == kw_then) {
                                    generateIfHead(stackAST->body[stackAST->top]);
                                }
                                if (whileStatement == 1 && global_token.type == kw_do) {
                                    generateWhileHead(stackAST->body[stackAST->top]);
                                }

                                //generateExpression(AST); // vygeneruj do seznamu instrukce vyrazu

                                // clear tree after generating
                                AST = malloc(sizeof(struct tAST) * 2);
                            }
                            simulatePredictive(global_token, predictiveStack, global_symtable, findNode(array, global_symtable, tFunctionTrackerGetTop(functionTracker)));
                        }
                    }
                    generateCodeParek(&global_token);

                    /*
                     * Create function generateCode(char* predictiveStackTop) and pass top of predictiveStack.
                     * Look at the top of predictiveStack: predictiveStack->content[predictiveStack->top-1] =>
                     *
                     * For example if it contains "EOL" and rules 1,2,4,6,7 were applied (stored in rulesApplied[]) => generate function declaration
                     * Generated code: label nasobeni
                     *
                     * P.S. maybe there is no need for checking applied rules
                     */
                     //generateCode(predictiveStack->content[predictiveStack->top-1],rulesApplied);


                    /*if(!strcmp(predictiveStack->content[predictiveStack->top-1],"<assign>")) { // bude nasledovat expression, vytvor promennou na prirazeni
                        char *frame = get_frame(functionTracker);
                        fprintf(list, "MOVE %s@%s %s@%%assign\n", frame, tmpToken.content, frame);
                    } // PROC NEZOBRAZUJE NIC PRI FUNKCI*/

                }

              	// NEJAKEJ PRINTING
		        if (printing == 1 ) {
                    // need to print this expression
                    generatePrint(&global_token);
                    //generateCodeParek(&global_token);
                    // todo: generate code
                    /*
                     * Previous token was print => generate stuff that needs to be printed. Current token (global_token.content) contains expression for printing.
                     */
                    // pop <print-expr> rule from stack
                    tStackPredictivePop(predictiveStack);
                    // we will not be printing anymore
                    printing = 0;
                }
                if (global_token.type == kw_if) {
                    // current token was if-condition or while-loop => expression will follow => need to simulate precedence
                    precedence = 1;
                    ifStatement = 1;
                }
                if (global_token.type == kw_while) {
                    precedence = 1;
                    whileStatement = 1;
                }
                if (global_token.type == ss_eol || global_token.type == s_rbrac) {
                    if (checkMainFunction() == 1 && strcmp(tFunctionTrackerGetTop(functionTracker), "Main") != 0) {
                        // we are in main function => rule 3 was applied => unset tracker of current function
                        //currentFunction = "";
                        tFunctionTrackerPop(functionTracker);
                    }
                    // clearing applied rules at the of one line
                    clearRulesApplied();
                }


        }

        if (strcmp(predictiveStack->content[predictiveStack->top - 1], "$") != 0) {
            errorHandling(2);                       // some rule remained on the stack
        }

        free(tmpToken.content);
        tStackASTDispose(stackAST);
        dispose(expendedStack);
        tStackPredictiveDispose(predictiveStack);
        //tASTDispose(AST);
        tFunctionTrackerDispose(functionTracker);

        /*fclose(list);
        if(ERROR_TYPE == 0) { // tisk obsahu souboru pokud se neobjevila chyba

            fclose(list);
            FILE *list = fopen("list.txt", "r");
            char c;
            while((c = fgetc(list)) != EOF)
                printf("%c",c);
            fclose(list);
        }*/

    fclose(file);

}

