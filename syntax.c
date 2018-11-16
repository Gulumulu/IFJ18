/**
 * Source file for syntax driven compilation
 *
 * Implemented by Gabriel Quirschfeld   xquirs00
 *                Marek Varga           xvarga14
 */

#include "syntax.h"
#include "scanner.h"
#include "symtable.h"

void doMagic() {

    FILE *file = fopen("../test.txt", "r");

    int is_func = 0;            // true if token was def, expecting function id next
    int undef = 0;              // true if token was equals sign, expecting only defined identifiers
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
        if (global_token.type == kw_def) {  // after the def keyword we are expecting the function id
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
        else if (global_token.type == ss_eol) { // the undefined region resets after eol
            undef = 0;
        }
        else if (global_token.type == s_eq || global_token.type == kw_if || global_token.type == kw_while) {   // after an equals sign, if and while only already defined ids can be used
            undef = 1;
        }
        else if ((undef == 1) && (global_token.type == s_id)) { // controls if ids after the equals sign, if and while statements are defined
            if (BSTSearch(&array[arr_id-1], hash_id(global_token.content)) == NULL) {   // if the identifier was not used before, it is not defined
                errorHandling(3);
                return;
            }
            else {  // if the identifier was used before, it is defined
                cnt->type = NULL;
                cnt->defined = 1;
                cnt->name = global_token.content;
                BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
            }
        }
        else if (global_token.type == s_id) {   // push the id into the local symtable for the specific function
            cnt->type = NULL;
            cnt->defined = 1;
            cnt->name = global_token.content;
            BSTInsert(&array[arr_id-1], cnt, hash_id(global_token.content), func_id);
        }
    }

    rewind(file);   // rewinding the file for another transit
    //if (ERROR_TYPE == 0) {
        // second transit of compiler -- passing tokens to parser
        // helper stacks
        tASTPointer *AST = malloc(sizeof(struct tAST) * 10);
        tASTInit(AST);                          // AST - abstract syntax tree - contains expression after precedence SA finished (down top SA)
        tStackPredictive *predictiveStack = malloc(sizeof(tStackPredictive) * 10);
        tStackPredictiveInit(predictiveStack);  // contains rules meant to be expanded in predictive SA (top down SA)
        tExpendedStack *expendedStack = malloc(sizeof(tExpendedStack) * 10);
        init(expendedStack);                    // contains symbols meant to be simplified in precedence SA
        tStackASTPtr *stackAST = malloc(sizeof(struct tStackAST) * 10);
        tStackASTInit(stackAST);                // helper stack for precedence SA, contains nodes meant to be merged together
        global_token = tmpToken;
        char *currentFunction = "";

        while (global_token.type != ss_eof && ERROR_TYPE == 0) {
            // call lexical analysis
            token_generate(file);
            // call syntax analysis
            //if (ERROR_TYPE == 0) {
                if (precedence == 1 || global_token.type == s_int || global_token.type == s_float ||
                    global_token.type == s_exp_int || global_token.type == s_exp_int_s ||
                    global_token.type == s_exp_f || global_token.type == s_exp_f_s) {
                    // we are dealing with expression => doing down top syntax analysis => need to simulate precedence
                    precedence = 1;
                    simulatePrecedence(global_token, expendedStack, stackAST, findNode(array, currentFunction));
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
                            AST = malloc(sizeof(struct tAST) * 10);
                        }
                    }
                }
                if (precedence == 0) {
                    // we are not dealing with expression => doing top down syntax analysis => need to simulate predictive SA
                    if (global_token.type == s_id) {
                        // if current token is id => need to call next token to decide whether current token is variable or function id
                        tmpToken = global_token;
                        token_generate(file);
                        tmpToken.type = decideID(global_token);
                        if (tmpToken.type == s_func_id) {
                            // helper to keep track in which function we are in
                            currentFunction = malloc(strlen(tmpToken.content) + 1);
                            strcpy(currentFunction, tmpToken.content);
                        }
                        // simulate predictive SA for current token
                        simulatePredictive(tmpToken, predictiveStack);
                        if (precedence == 1) {
                            simulatePrecedence(tmpToken, expendedStack, stackAST, findNode(array, currentFunction));
                        }
                    }
                    if (precedence == 0) {
                        // simulate predictive SA for next token
                        simulatePredictive(global_token, predictiveStack);
                    } else {
                        simulatePrecedence(global_token, expendedStack, stackAST, findNode(array, currentFunction));
                    }
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
            //}
        }

        if (strcmp(predictiveStack->content[predictiveStack->top - 1], "$") != 0) {
            errorHandling(2);                       // some rule remained on the stack
        }

        free(tmpToken.content);
        tStackASTDispose(stackAST);
        dispose(expendedStack);
        tStackPredictiveDispose(predictiveStack);
        tASTDispose(AST);
    //}
    fclose(file);
}