/**
 *  Source file for generating AST and appropriated semantic actions.
 *  Implemented by Marek Varga, xvarga14
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

/**
 * Function initializes AST.
 *
 * @param AST pointer to AST structure is AST that is initialized
 */
void tASTInit(tASTPointer* AST) {
    AST->content = NULL;
    AST->ID = malloc(3);
    AST->LeftPointer = NULL;
    AST->RightPointer = NULL;
}

/**
 * Function disposes AST.
 *
 * @param AST pointer to AST structure is AST that is disposed
 */
void tASTDispose(tASTPointer* AST) {
    if (AST == NULL) {
        return;
    } else {
        tASTPointer* disposedNode = AST;
        tASTDispose(disposedNode->LeftPointer);
        tASTDispose(disposedNode->RightPointer);
        free(disposedNode);
        //AST = NULL;
    }
}

/**
 * Function searches for pattern in a string
 *
 * @param string
 * @param pattern
 * @return
 */
int match(const char *string, const char *pattern)
{
    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
    int status = regexec(&re, string, 0, NULL, 0);
    regfree(&re);
    if (status != 0) return 0;
    return 1;
}

char* getArguments(char* functionDeclaration) {
    char* arguments = "";
    arguments = strrchr(functionDeclaration, '(');
    if (arguments == NULL) {
        arguments = strchr(functionDeclaration, ' ');
        /*if (arguments == NULL) {
            return NULL;
        } else {
            return arguments;
        }*/
        return functionDeclaration;
    } else {
        return arguments;
    }
}

/**
 * Function resolves function name based on given function name whether it is a build-in or user-defined one
 *
 * @param functionName
 * @return
 */
char* getFunctionName(char* functionName) {
    char* arguments = "";
    if (match(functionName, "(print)+") == 1) {
        return "print";
    } else if (match(functionName, "(length)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\((\".*\"|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*))\\)") != 1 && match(arguments, "(length)((\"[a-z]+\")|[a-z]+)") != 1) {
                return NULL;
            }
            // (([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*) -> variable regex
        }
        return "length";
    } else if (match(functionName, "(inputs)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\(\\)") != 1 && match(arguments, "(inputs)") != 1) {
                return NULL;
            }
        }
        return "inputs";
    } else if (match(functionName, "(inputi)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\(\\)") != 1 && match(arguments, "(inputi)") != 1) {
                return NULL;
            }
        }
        return "inputi";
    } else if (match(functionName, "(inputf)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\(\\)") != 1 && match(arguments, "(inputf)") != 1) {
                return NULL;
            }
        }
        return "inputf";
    } else if (match(functionName, "(substr)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\(((\".*\")|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*) |([0-9]*[0-9]+))\\)") != 1 && match(arguments, "(substr)((\".*\")|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+))") != 1) {
                return NULL;
            }
        }
        return "substr";
    } else if (match(functionName, "(ord)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\(((\".*\")|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+))\\)") != 1 && match(arguments, "(ord)((\".*\")|[a-z]+),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+))") != 1) {
                return NULL;
            }
        }
        return "ord";
    } else if (match(functionName, "(chr)+") == 1) {
        if ((arguments = getArguments(functionName)) == NULL) {
            return NULL;
        } else {
            if (match(arguments, "\\((([0-1]*[0-9]*[0-9]+) | (2[0-5]+[0-5]+) | (([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*))\\)") != 1 && match(arguments, "(ord)((([0-1]*[0-9]*[0-9]+)|(2[0-5]+[0-5]+)|[a-z]+)|((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)))")) {
                return NULL;
            }
        }
        return "chr";
    } else {
        return "function";
    }
}

/**
 * Function searches for variable in symtable based on given token.
 *
 * @param token
 * @return
 */
BSTNodeContentPtr* findVariable(BSTNodePtr node, Token* token) {
    if (token == NULL || (token->type == s_id && node == NULL) ) {
        errorHandling(99);
        return NULL;
    } else {
        if (token->type == s_id) {
            // find variable in symtable
            return BSTSearch(&node, hash_id(token->content));
        } else if (token->type == s_int || token->type == s_exp_int || token->type == s_float || token->type == s_exp_f || token->type == s_string || token->type == kw_length || token->type == s_func_expr) {
            // leaf will be a constant therefore creation of new BSTNode is needed
            BSTNodeContentPtr* tmpNode = malloc(sizeof(struct BSTNodeContent));
            if (tmpNode == NULL) {
                errorHandling(99);
                return NULL;
            } else {
                size_t len = strlen(token->content);
                tmpNode->name = malloc(len+1);
                tmpNode->name = memcpy(tmpNode->name,token->content, len);
                tmpNode->name[(int)len] = '\0';
                switch (token->type) {
                    case s_int:
                    case s_exp_int:
                    case s_exp_int_s:
                    case kw_length:
                        tmpNode->type = "int";
                        break;
                    case s_float:
                    case s_exp_f:
                    case s_exp_f_s:
                        tmpNode->type = "float";
                        break;
                    case s_func_expr:
                        tmpNode->type = getFunctionName(token->content);
                        if (tmpNode->type == NULL) {
                            errorHandling(6);               // 4 or 6?
                            return NULL;
                        }
                        break;
                    default:
                        tmpNode->type = "string";
                        break;
                }
                tmpNode->defined = 1;
                return tmpNode;
            }
        } else {                                                // attempting to create wrong leaf
            errorHandling(99);
            return NULL;
        }
    }
}

/**
 * Function creates new leaf for AST.
 *
 * @param symtablePointer pointer to AST structure is AST leaf will be created
 * @return pointer to AST structure is newly created leaf
 */
tASTPointer* makeLeaf(BSTNodeContentPtr* symtablePointer) {
    if (symtablePointer == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        tASTPointer* newLeaf = malloc(sizeof(struct tAST));
        if (newLeaf == NULL) {
            errorHandling(99);
            return NULL;
        } else {
            newLeaf->RightPointer = newLeaf->LeftPointer = NULL;
            newLeaf->content = symtablePointer;
            newLeaf->ID = malloc(3);
            strcpy(newLeaf->ID,"E");
            newLeaf->ID[1] = '\0';
            return newLeaf;
        }
    }
}

/**
 * Function checks whether types of operands of either nodes are matching.
 *
 * @param leftContent pointer to BSTNodeContent is pointer to content in BST
 * @param rightContent pointer to BSTNodeContent is pointer to content in BST
 * @param ID pointer to char is operation type
 * @return zero-value is returned if types are wrong, value 1 returned if either of the types is variable or user-defined function, value 2 is returned if types are correct
 */
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent, char* ID) {
    if (leftContent == NULL || rightContent == NULL) {
        errorHandling(99);
        return 0;
    } else {
        if (leftContent->type == NULL || rightContent->type == NULL) {
            // variable or user-defined function
            return 1;
        } else if (strcmp(leftContent->type, "variable") == 0 || strcmp(rightContent->type, "variable") == 0 || strcmp(leftContent->type, "function") == 0 || strcmp(rightContent->type, "function") == 0) {
            // variable or user-defined function
            return 1;
        } else if (strcmp(ID, "*") == 0 || strcmp(ID, "/") == 0 || strcmp(ID, "-") == 0) {
            // multiplication, division or subtraction with floats or ints
            if (strcmp(ID, "/") == 0 && (leftContent->name != NULL && strcmp(leftContent->name, "0") == 0) ) {
                // division by zero constant
                errorHandling(9);
                return 0;
            }
            if (strcmp(leftContent->type, "string") == 0 || strcmp(rightContent->type, "string") == 0 || strcmp(leftContent->type, "inputs") == 0 || strcmp(rightContent->type, "inputs") == 0 || strcmp(leftContent->type, "chr") == 0 || strcmp(rightContent->type, "chr") == 0 || strcmp(leftContent->type, "substr") == 0 || strcmp(rightContent->type, "substr") == 0) {
                // operation with string(s)
                errorHandling(4);
                return 0;
            }

            return 2;
        } else if (strcmp(ID, "+") != 0) {
            if (strcmp(ID, "!=") != 0 && strcmp(ID, "==") != 0) {
                // <, >, <=, >= need to have either of the number types or strings
                char* numberTypes[6] = {"int", "float", "length", "ord", "inputi", "inputf"};
                char* stringTypes[4] = {"string", "inputs", "substr", "chr"};
                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 4; j++) {
                        if ((strcmp(leftContent->type, numberTypes[i]) == 0 && strcmp(rightContent->type, stringTypes[j]) == 0) || (strcmp(rightContent->type, numberTypes[i]) == 0 && strcmp(leftContent->type, stringTypes[j]) == 0) ){
                            errorHandling(4);
                            return 0;
                        }
                    }
                }
                /*if (strcmp(leftContent->type, "int") == 0 && (strcmp(rightContent->type, "int") != 0 && strcmp(rightContent->type, "float") != 0 && strcmp(rightContent->type, "length") != 0 && strcmp(rightContent->type, "ord") != 0)) {
                    errorHandling(4);
                    return 0;
                }
                if (strcmp(leftContent->type, "float") == 0 && (strcmp(rightContent->type, "int") != 0 && strcmp(rightContent->type, "float") != 0 && strcmp(rightContent->type, "length") != 0 && strcmp(rightContent->type, "ord") != 0)) {
                    errorHandling(4);
                    return 0;
                }
                if (strcmp(rightContent->type, "int") == 0 && (strcmp(leftContent->type, "int") != 0 && strcmp(leftContent->type, "float") != 0 && strcmp(leftContent->type, "length") != 0 && strcmp(leftContent->type, "ord") != 0)) {
                    errorHandling(4);
                    return 0;
                }
                if (strcmp(rightContent->type, "float") == 0 && (strcmp(leftContent->type, "int") != 0 && strcmp(leftContent->type, "float") != 0 && strcmp(leftContent->type, "length") != 0 && strcmp(leftContent->type, "ord") != 0)) {
                    errorHandling(4);
                    return 0;
                }*/
                return 2;
            } else {
                // !=, == don't need to check types, typecast will be done
                return 2;
            }
        } else {
            // addition or concatenation
            char* numberTypes[6] = {"int", "float", "inputi", "inputs", "length", "ord"};
            char* stringTypes[4] = {"string", "inputs", "substr", "chr"};
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 4; j++) {
                    if ((strcmp(leftContent->type, numberTypes[i]) == 0 && strcmp(rightContent->type, stringTypes[j]) == 0) || (strcmp(rightContent->type, numberTypes[i]) == 0 && strcmp(leftContent->type, stringTypes[j]) == 0) ){
                        errorHandling(4);
                        return 0;
                    }
                }
            }
            return 2;
        }
    }
}

/**
 * Function decides resulting type from expression.
 *
 * @param leftContent pointer to BSTNodeContent is pointer to content in BST
 * @param rightContent pointer to BSTNodeContent is pointer to content in BST
 * @param ID pointer to char is operation type
 * @return int, float, string, boolean or NULL type is returned
 */
char* decideType(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent, char* ID) {
    if (leftContent == NULL || rightContent == NULL || ID == NULL) {
        errorHandling(99);
        return NULL;
    } else if (strcmp(ID, "+") == 0 || strcmp(ID, "-") == 0 || strcmp(ID, "*") == 0 || strcmp(ID, "/") == 0) {
        if (strcmp(leftContent->type, "string") == 0 || strcmp(leftContent->type, "inputs") == 0 || strcmp(leftContent->type, "substr") == 0 || strcmp(leftContent->type, "chr") == 0) {
            // result of operation will be of string type
            return "string";
        } else if (strcmp(rightContent->type, "string") == 0 || strcmp(rightContent->type, "inputs") == 0 || strcmp(rightContent->type, "substr") == 0 || strcmp(rightContent->type, "chr") == 0) {
            return "string";
        } else if (strcmp(leftContent->type, "float") == 0 || strcmp(rightContent->type, "float") == 0 || strcmp(leftContent->type, "inputf") == 0 || strcmp(rightContent->type, "inputf") == 0) {
            // result of operation will be of float type
            return "float";
        } else if (strcmp(leftContent->type, "print") == 0 || strcmp(rightContent->type, "print") == 0) {
            // attempting to create a tree with nill descedant
            errorHandling(4);
            return NULL;
        }

        // result will be int
        return "int";
    } else {
        // <, <=, >, >=, !=, == operations
        return "boolean";
    }
}

/**
 * Function checks wheter variables were defined.
 *
 * @param leftContent pointer to BSTNodeContent is pointer to content in BST
 * @param rightContent pointer to BSTNodeContent is pointer to content in BST
 * @return non-zero value when variables are defined otherwise zero value is returned
 */
int definedVariables(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent) {
    if (leftContent == NULL || rightContent == NULL) {
        errorHandling(99);
        return 0;
    } else {
        if (leftContent->defined == 1 && rightContent->defined == 1) {
            return 1;
        } else {
            return 0;
        }
    }
}

/**
 * Function performs semantic actions such as variable definition, type matching, etc.
 *
 * @param leftContent pointer to BSTNodeContent is pointer to content in BST
 * @param rightContent pointer to BSTNodeContent is pointer to content in BST
 * @return non-zero value when semantics are correct otherwise zero value is returned
 */
int correctSemantics(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent) {
    /*if (matchingTypes(leftContent, rightContent) == 0) {
        errorHandling(4);
        return 0;
    }*/
    if (definedVariables(leftContent, rightContent) == 0) {
        errorHandling(3);
        return 0;
    }

    return 1;
}

/**
 * Function creates new AST tree by combining two nodes.
 *
 * @param ID char is ID of AST
 * @param leftPointer pointer to AST structure is pointer to left sub tree/leaf
 * @param rightPointer pointer to AST structure is pointer to right sub tree/leaf
 * @return pointer to AST structure is newly created tree
 */
tASTPointer* makeTree(char* ID, tASTPointer* leftPointer, tASTPointer* rightPointer) {
    if (leftPointer == NULL || rightPointer == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        tASTPointer* newTree = malloc(sizeof(struct tAST));
        if (newTree == NULL) {
            errorHandling(99);
            return NULL;
        } else {
            newTree->LeftPointer = leftPointer;
            newTree->RightPointer = rightPointer;
            if (strcmp(ID, "l") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, "<");
            } else if (strcmp(ID, "g") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, ">");
            } else if (strcmp(ID, ".") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, "<=");
            } else if (strcmp(ID, ",") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, ">=");
            } else if (strcmp(ID, "?") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, "==");
            } else if (strcmp(ID, "!") == 0) {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, "!=");
            } else {
                newTree->ID = malloc(3);
                strcpy(newTree->ID, ID);
            }
            if (correctSemantics(leftPointer->content, rightPointer->content) != 0) {
                BSTNodeContentPtr* tmpContent = malloc(sizeof(struct BSTNodeContent));
                if (tmpContent == NULL) {
                    errorHandling(99);
                    return NULL;
                } else {
                    switch (matchingTypes(rightPointer->content, leftPointer->content, ID) ) {
                        case 1:
                            // either operand is variable or user-defined function => not changing type
                            tmpContent->type = NULL;
                            break;
                        case 2:
                            // every operation  => changing type
                            if (decideType(leftPointer->content, rightPointer->content, ID) != NULL) {
                                size_t typeLen = strlen(decideType(leftPointer->content, rightPointer->content, ID));
                                tmpContent->type = malloc(typeLen + 1);
                                memcpy(tmpContent->type, decideType(leftPointer->content, rightPointer->content, ID),typeLen);
                                tmpContent->type[typeLen] = '\0';
                            }
                            break;
                        default:
                            // error
                            break;
                    }
                    tmpContent->defined = 1;
                    newTree->content = tmpContent;
                    return newTree;
                }
            } else {
                //errorHandling(4);
                return NULL;
            }
        }
    }
}

/**
 * Function initializes stack of ASTs.
 *
 * @param stack pointer to tStackAST structure is initialized stack
 */
void tStackASTInit(tStackASTPtr* stack) {
    stack->top = 0;
}

/**
 * Function pushes AST into the stack of ASTs.
 *
 * @param stack pointer to tStackAST structure is stack to which AST will be pushed
 * @param AST pointer to tAST structure is AST that is pushed onto the stack
 */
void tStackASTPush(tStackASTPtr* stack, tASTPointer* AST) {
    if (stack == NULL || stack->top == MAX || AST == NULL) {
        errorHandling(99);
    } else {
        stack->top++;
        stack->body[stack->top] = AST;
    }
}

/**
 * Function pops ASt from stack of ASTs.
 *
 * @param stack pointer to tStackAST structure is stack from which AST will be poped
 * @return pointer to tAST structure is AST that is poped from the stack
 */
tASTPointer* tStackASTPop(tStackASTPtr* stack) {
    if (stack == NULL || stack->top == 0) {
        errorHandling(99);
        return NULL;
    } else {
        tASTPointer* tmp = stack->body[stack->top];
        stack->body[stack->top] = NULL;
        stack->top--;
        return tmp;
    }
}

/**
 * Function disposes stack of ASTs.
 *
 * @param stack pointer to tStackAST structure is stack that will be disposed
 */
void tStackASTDispose(tStackASTPtr* stack) {
    if (stack == NULL) {
        errorHandling(99);
    } else {
        if (stack->top >= 0) {
            free(stack->body);
        }
    }
}
