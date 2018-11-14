//
// Created by parek on 11/6/18.
//
/**
 * Source file for syntax driven compilation.
 * Implemented by Gabriel Quirschfeld, xquirs00
 *                Marek Varga, xvarga14
 */

#include "syntax.h"
#include "scanner.h"
#include "symtable.h"

void doMagic() {

    FILE *file = fopen("..\\test.txt", "r");
    int is_func = 0;
    char* func_num = NULL;
    BSTNodePtr* global_symtable = malloc(sizeof(struct BSTNode));               // global symtable storing the function ids
    BSTInit(global_symtable);
    BSTNodePtr *local_symtable;                                                 // local symtable storing the param ids inside a function
    BSTNodeContentPtr* cnt;

    // first transit of compiler -- filling out symtable
    Token tmpToken = global_token;
    while (global_token.type != ss_eof) {
        token_generate(file);
        cnt = malloc(sizeof(struct BSTNodeContent));
        if (global_token.type == kw_def) {                                      // after the def keyword we get a function id
            is_func = 1;
        }
        if ((is_func == 1) && (global_token.type == s_id)) {                    // push the function id into the global symtable
            cnt->type = NULL;
            cnt->declared = 1;
            cnt->defined = 1;
            cnt->name = global_token.content;
            BSTInsert(global_symtable, cnt, hash_id(global_token.content));
   //         BSTNodePtr *local_symtable = malloc(sizeof(struct BSTNode));        // local symtable storing the param ids inside a function
   //         BSTInit(local_symtable);
            func_num = global_token.content;
            is_func = 0;
        }
        else if (global_token.type == s_id) {                                   // push the id into the local symtable for the specific function
            puts("local hey:");
            puts(func_num);

            cnt->type = NULL;
            cnt->declared = 1;
            cnt->defined = 1;
            cnt->name = global_token.content;
   //         BSTInsert(local_symtable, cnt, hash_id(global_token.content));

        }
    }

    rewind(file);

    // second transit of compiler -- passing tokens to parser
    // helper stacks
    tASTPointer* AST = malloc(sizeof(struct tAST)*10);
    tASTInit(AST);                          // AST - abstract syntax tree - contains expression after precedence SA finished (down top SA)
    tStackPredictive* predictiveStack = malloc(sizeof(tStackPredictive)*10);
    tStackPredictiveInit(predictiveStack);  // contains rules meant to be expanded in predictive SA (top down SA)
    tExpendedStack* expendedStack = malloc(sizeof(tExpendedStack)*10);
    init(expendedStack);                    // contains symbols meant to be simplified in precedence SA
    tStackASTPtr* stackAST = malloc(sizeof(struct tStackAST)*10);
    tStackASTInit(stackAST);                // helper stack for precedence SA, contains nodes meant to be merged together
    global_token = tmpToken;

    while (global_token.type != ss_eof) {
        // call lexical analysis
        token_generate(file);
        // call syntax analysis
        if (ERROR_TYPE == 0) {
            if (precedence == 1 || global_token.type == s_int || global_token.type == s_float || global_token.type == s_exp_int || global_token.type == s_exp_int_s || global_token.type == s_exp_f || global_token.type == s_exp_f_s) {
                // we are dealing with expression => doing down top syntax analysis => need to simulate precedence
                precedence = 1;
                simulatePrecedence(global_token, expendedStack, stackAST);
                if (precedence == 0) {
                    // precedence has finished => need to pop rule "<assign>" from predictive stack
                    tStackPredictivePop(predictiveStack);
                    // assign newly created AST
                    if (stackAST != NULL && ERROR_TYPE == 0) {
                        // result of precedence will be stored in AST - abstract syntax tree
                        *AST = *stackAST->body[stackAST->top];
                        // todo: generate expression
                        /*
                         * Create function generateExpression(tASTPointer* AST) and pass newly created AST.
                         * Generate code for expression stored in AST.
                         * For example a = 2*b   => 2*b will be stored in AST as:
                         *      *
                         *     | |
                         *     2 b
                         *
                         * Generated code: mul a 2 b    -- still need to figure out how to pass variable a
                         */
                        // clear tree after generating
                        AST = malloc(sizeof(struct tAST)*10);
                    }
                }
            }
            if (precedence == 0) {
                // we are not dealing with expression => doing top down syntax analysis => need to simulate predictive SA
                if (global_token.type == s_id ) {
                    // if current token is id => need to call next token to decide whether current token is variable or function id
                    tmpToken = global_token;
                    token_generate(file);
                    tmpToken.type = decideID(global_token);
                    // simulate predictive SA for next token
                    simulatePredictive(tmpToken, predictiveStack);
                }
                // simulate predictive SA for current token
                simulatePredictive(global_token, predictiveStack);
                // todo: generate code
                /*
                 * Create function generateCode(char* predictiveStackTop) and pass top of predictiveStack.
                 * Look at the top of predictiveStack: predictiveStack->content[predictiveStack->top-1] =>
                 * generateCode(predictiveStack->content[predictiveStack->top-1]);
                 * For example if it contains "EOL" and rules 1,2,4,6,7 were applied (stored in rulesApplied[]) => generate function declaration
                 * Generated code: label nasobeni
                 *
                 * P.S. maybe there is no need for checking applied rules
                 */
            }
            if (global_token.type == kw_if) {
                // current token was if-condition => expression will follow => need to simulate precedence
                precedence = 1;
            }
        }
    }

    if (strcmp(predictiveStack->content[predictiveStack->top-1], "$") != 0) {
        errorHandling(2);                       // some rule remained on the stack
    }

    free(tmpToken.content);
    tStackASTDispose(stackAST);
    dispose(expendedStack);
    tStackPredictiveDispose(predictiveStack);
    tASTDispose(AST);

    fclose(file);
}