//
// Created by parek on 11/6/18.
//
/**
 * Source file for syntax driven compilation
 *
 * Implemented by Gabriel Quirschfeld, xquirs00
 *                Marek Varga, xvarga14
 */

#include "syntax.h"
#include "scanner.h"
#include "symtable.h"

void doMagic() {

    FILE *file = fopen("..\\test.txt", "r");

    int is_func = 0;            // true if token was def and next token is a function id
    int arr_id = 0;             // id for the array holding local symtables
    unsigned long func_id = 0;  // function id to be put into local id content

    BSTNodePtr* global_symtable = malloc(sizeof(struct BSTNode));       // global symtable storing the function ids
    BSTInit(global_symtable);

    struct BSTNode **array;         // array storing local symtables
    array = malloc(10000 * sizeof(struct BSTNode *));

    BSTNodeContentPtr* cnt;

    // first transit of compiler -- filling out symtable
    Token tmpToken = global_token;
    while (global_token.type != ss_eof) {
        token_generate(file);   // calling lexical analysis to get another token
        cnt = malloc(sizeof(struct BSTNodeContent));
        if (global_token.type == kw_def) {  // after the def keyword we get a function id
            is_func = 1;
        }
        if ((is_func == 1) && (global_token.type == s_id)) {    // push the function id into the global symtable
            cnt->type = "function";
            cnt->defined = 1;
            cnt->name = global_token.content;
            BSTInsert(global_symtable, cnt, hash_id(global_token.content), 0);  // inserts the function id into the global symtable
            BSTNodePtr* local_symtable = malloc(sizeof(struct BSTNode));    // allocating memory for a new local symtable storing local identifiers
            BSTInit(local_symtable);    // initialization of the local symtable
            array[arr_id] = *local_symtable;    // storing the local symtable into the array of local symtables
            arr_id++;
            func_id = hash_id(global_token.content);    // storing the hash of function id for later use
            is_func = 0;    // no longer expecting a function id
        }
        else if (global_token.type == s_id) {   // push the id into the local symtable for the specific function
            cnt->type = NULL;
            cnt->defined = 1;
            cnt->name = global_token.content;
            BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
        }
    }

    rewind(file);   // rewinding the file for another transit

    // second transit of compiler -- passing tokens to parser
    tASTPointer* AST = malloc(sizeof(struct tAST)*10);
    tASTInit(AST);
    tStackPredictive* predictiveStack = malloc(sizeof(tStackPredictive)*10);
    tStackPredictiveInit(predictiveStack);
    tExpendedStack* expendedStack = malloc(sizeof(tExpendedStack)*10);
    init(expendedStack);
    tStackASTPtr* stackAST = malloc(sizeof(struct tStackAST)*10);
    tStackASTInit(stackAST);
    global_token = tmpToken;

    while (global_token.type != ss_eof) {
        // call lexical anal
        token_generate(file);
        // call top down anal
        if (ERROR_TYPE == 0) {
            if (precedence == 1 || global_token.type == s_int || global_token.type == s_float || global_token.type == s_exp_int || global_token.type == s_exp_int_s || global_token.type == s_exp_f || global_token.type == s_exp_f_s) {
                precedence = 1;
                simulatePrecedence(global_token, AST, expendedStack, stackAST);
                if (precedence == 0) {
                    tStackPredictivePop(predictiveStack);
                }
            }
            // && strcmp(predictiveStack->content[predictiveStack->top-1], "<assign>") == 0
            if (precedence == 0) {
                if (global_token.type == s_id ) {
                    tmpToken = global_token;
                    token_generate(file);
                    tmpToken.type = decideID(global_token);
                    simulatePredictive(tmpToken, AST, predictiveStack);
                }
                simulatePredictive(global_token, AST, predictiveStack);
            }
            if (global_token.type == kw_if) {
                precedence = 1;
            }
        }
    }
    // todo: what if something remained in predictive stack? e.g. '<function-tail>'
    free(tmpToken.content);
    tStackASTDispose(stackAST);
    dispose(expendedStack);
    tStackPredictiveDispose(predictiveStack);
    tASTDispose(AST);
    /*
    // if lexical analysis passed ERROR_TYPE should still be 0
    if (ERROR_TYPE == 0) {
        tASTPointer* AST = malloc(sizeof(struct tAST));
        if (AST == NULL) {
            errorHandling(99);
            //return ERROR_TYPE;
        }
        tASTInit(AST);
        char* tmp[14] = {"def", "function-id", "(", "id", ")", "EOL", "id", "=", "id", "*", "2", "EOL", "end", "$"};
        tStackPredictive* predictiveStack;
        predictiveStack = malloc(sizeof(tStackPredictive));
        tStackPredictiveInit(predictiveStack);
        int i = 0;
        do {
            simulatePredictive(tmp[i], AST, predictiveStack);
            if (strcmp(predictiveStack->content[predictiveStack->top-1], "<expr>") == 0) {
                tStackPredictive* tmpStack = malloc(sizeof(tStackPredictive));
                tStackPredictiveInit(tmpStack);
                for (int j = i; strcmp(tmp[i], "EOL") != 0; j++, i++) {
                    tStackPredictivePush(tmpStack, tmp[j]);
                }
                for (int k = 2; k < tmpStack->top; k++) {
                    simulatePrecedence(tmpStack->content[k], AST);
                }
                tStackPredictiveDispose(tmpStack);
            }
            i++;
        } while (strcmp(tmp[i], "$") != 0);
        //simulatePredictive("i+i*i", AST);                     // predictive SA
        //simulatePrecedence("i+i*i$", AST);                      // tokens "i+i*i$" will be rewritten to AST
        tASTDispose(AST);
    }*/

    fclose(file);
}