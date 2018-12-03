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
    AST->ID = NULL;
    //AST->changed = NULL;
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

        //tASTPointer* disposedNode = AST;
        tASTDispose(AST->LeftPointer);
        tASTDispose(AST->RightPointer);
        free(AST->ID);
        //free(disposedNode->changed);
        free((*AST).content->type);
        free(AST->content->name);
        free(AST->content->var);
        free(AST->content);
        free(AST);
        //disposedNode = NULL;

        AST = NULL;
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
            if (match(arguments, "\\(((\".*\")|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+))\\)") != 1 && match(arguments, "(substr)((\".*\")|(([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+)),((([a-z]|_){1}([0-9]|[a-z]|[A-Z]|_)*)|([0-9]*[0-9]+))") != 1) {
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
void findVariable(BSTNodePtr node, Token* token, BSTNodeContentPtr* nodeContentPtr) {
    if (token == NULL || (token->type == s_id && node == NULL) ) {
        errorHandling(99);
        nodeContentPtr = NULL;
    } else {
        if (token->type == s_id) {
            // find variable in symtable
            nodeContentPtr->func_params = BSTSearch(&node, hash_id(token->content))->func_params;
            nodeContentPtr->defined = BSTSearch(&node, hash_id(token->content))->defined;
            nodeContentPtr->name = malloc(sizeof(char)*(strlen(BSTSearch(&node, hash_id(token->content))->name)+1));
            nodeContentPtr->name = strcpy(nodeContentPtr->name, BSTSearch(&node, hash_id(token->content))->name);
            nodeContentPtr->name[strlen(BSTSearch(&node, hash_id(token->content))->name)] = '\0';
            nodeContentPtr->type = malloc(sizeof(char)*(strlen(BSTSearch(&node, hash_id(token->content))->type)+1));
            nodeContentPtr->type = strcpy(nodeContentPtr->type, BSTSearch(&node, hash_id(token->content))->type);
            nodeContentPtr->type[strlen(BSTSearch(&node, hash_id(token->content))->type)] = '\0';
            if (BSTSearch(&node, hash_id(token->content))->var != NULL) {
                nodeContentPtr->var = malloc(sizeof(char)*(strlen(BSTSearch(&node, hash_id(token->content))->var)+1));
                nodeContentPtr->var = strcpy(nodeContentPtr->var, BSTSearch(&node, hash_id(token->content))->var);
                nodeContentPtr->var[strlen(BSTSearch(&node, hash_id(token->content))->var)] = '\0';
            } else {
                nodeContentPtr->var = malloc(sizeof(char)*2);
                nodeContentPtr->var = strcpy(nodeContentPtr->var, "x");
                nodeContentPtr->var[1] = '\0';
            }

        } else if (token->type == s_int || token->type == s_exp_int || token->type == s_float || token->type == s_exp_f || token->type == s_string || token->type == kw_length || token->type == s_func_expr) {
            // leaf will be a constant therefore creation of new BSTNode is needed
            //BSTNodeContentPtr* tmpNode = malloc(sizeof(struct BSTNodeContent));
            /*if (tmpNode == NULL) {
                errorHandling(99);
                nodeContentPtr =  NULL;
            } else {*/
                size_t len = strlen(token->content);
                nodeContentPtr->name = malloc(len+1);
                nodeContentPtr->name = memcpy(nodeContentPtr->name,token->content, len);
                nodeContentPtr->name[(int)len] = '\0';
                switch (token->type) {
                    case s_int:
                    case s_exp_int:
                    case s_exp_int_s:
                    case kw_length:
                        nodeContentPtr->type = malloc(sizeof(char)*4);
                        nodeContentPtr->type = strcpy(nodeContentPtr->type, "int");
                        nodeContentPtr->type[3] = '\0';
                        break;
                    case s_float:
                    case s_exp_f:
                    case s_exp_f_s:
                        nodeContentPtr->type = malloc(sizeof(char)*6);
                        nodeContentPtr->type = strcpy(nodeContentPtr->type, "float");
                        nodeContentPtr->type[5] = '\0';
                        break;
                    case s_func_expr:
                        if (getFunctionName(token->content) == NULL) {
                            errorHandling(6);               // 4 or 6?
                            nodeContentPtr->type = malloc(sizeof(char)*2);
                            nodeContentPtr->type = strcpy(nodeContentPtr->type, "x");
                            nodeContentPtr->type[1] = '\0';
                        } else {
                            nodeContentPtr->type = malloc(sizeof(char)*(strlen(getFunctionName(token->content))+1));
                            nodeContentPtr->type = strcpy(nodeContentPtr->type, getFunctionName(token->content));
                            nodeContentPtr->type[strlen(getFunctionName(token->content))] = '\0';
                        }
                        break;
                    default:
                        nodeContentPtr->type = malloc(sizeof(char)*7);
                        nodeContentPtr->type = strcpy(nodeContentPtr->type, "string");
                        nodeContentPtr->type[6] = '\0';
                        break;
                }
                //tmpNode->defined = 1;
                nodeContentPtr->defined = 1;
                nodeContentPtr->func_params = 0;
                nodeContentPtr->var = malloc(sizeof(char)*2);
                nodeContentPtr->var = strcpy(nodeContentPtr->var, "x");
                nodeContentPtr->var[1] = '\0';
                //nodeContentPtr->type = malloc(sizeof(char)*strlen(tmpNode->type));
                //nodeContentPtr->type = strcpy(nodeContentPtr->type, tmpNode->type);
                //nodeContentPtr->type[strlen(tmpNode->type)] = '\0';
                /*nodeContentPtr->var = malloc(sizeof(char)*strlen(tmpNode->var));
                nodeContentPtr->var = strcpy(nodeContentPtr->var, tmpNode->var);
                nodeContentPtr->var[strlen(tmpNode->var)] = '\0';*/
                //nodeContentPtr->name = malloc(sizeof(char)*strlen(tmpNode->name));
                //nodeContentPtr->name = strcpy(nodeContentPtr->name, tmpNode->name);
                //nodeContentPtr->name[strlen(tmpNode->name)] = '\0';
                //free(tmpNode);
                //tmpNode=NULL;
            //}
        } else {                                                // attempting to create wrong leaf
            errorHandling(99);
            nodeContentPtr = NULL;
        }
    }
}

/**
 * Function creates new leaf for AST.
 *
 * @param symtablePointer pointer to AST structure is AST leaf will be created
 * @return pointer to AST structure is newly created leaf
 */
void makeLeaf(BSTNodePtr node, Token* token, tASTPointer* AST) {
    if (node == NULL) {
        errorHandling(99);
        AST = NULL;
    } else {
        //tASTPointer* newLeaf = malloc(sizeof(struct tAST));
        /*if (newLeaf == NULL) {
            errorHandling(99);
            AST =  NULL;
        } else {*/
            //newLeaf->RightPointer = newLeaf->LeftPointer = NULL;
            AST->LeftPointer = AST->RightPointer = NULL;
            //BSTNodeContentPtr* tmpNodeContent = malloc(sizeof(struct BSTNodeContent));
            findVariable(node, token, AST->content);
            //newLeaf->content = malloc(sizeof(struct BSTNodeContent));
            //newLeaf->content->defined = tmpNodeContent->defined;
            //AST->content->defined = tmpNodeContent->defined;
            //newLeaf->content->func_params = tmpNodeContent->func_params;
            //AST->content->func_params = tmpNodeContent->func_params;
            //AST->content->name = malloc(sizeof(char)*(strlen(tmpNodeContent->name)+1));
            //AST->content->name = strcpy(AST->content->name, tmpNodeContent->name);
            //AST->content->name[strlen(tmpNodeContent->name)] = '\0';
            //AST->content->type = malloc(sizeof(char)*(strlen(tmpNodeContent->type)+1));
            //AST->content->type = strcpy(AST->content->type, tmpNodeContent->type);
            //AST->content->type[strlen(tmpNodeContent->type)] = '\0';
            //free(tmpNodeContent);
            //tmpNodeContent=NULL;
            AST->ID = malloc(sizeof(char)*2);
            strcpy(AST->ID,"E");
            AST->ID[1] = '\0';
            //AST->changed = NULL;
            //AST = newLeaf;
            //AST->LeftPointer = newLeaf->LeftPointer;
            //AST->RightPointer = newLeaf->RightPointer;
            //AST->content = malloc(sizeof(struct BSTNodeContent));
            //AST->content = newLeaf->content;
            //AST->content->defined = newLeaf->content->defined;
            //AST->content->func_params = newLeaf->content->func_params;
            //AST->content->name = malloc(sizeof(char)*(strlen(newLeaf->content->name)+1));
            //AST->content->name = strcpy(AST->content->name, newLeaf->content->name);
            //AST->content->name[strlen(newLeaf->content->name)] = '\0';
            //AST->content->type = malloc(sizeof(char)*(strlen(newLeaf->content->type)+1));
            //AST->content->type = strcpy(AST->content->type, newLeaf->content->type);
            //AST->content->type[strlen(newLeaf->content->type)] = '\0';
            //AST->ID = malloc(sizeof(char)*strlen(newLeaf->ID));
            //AST->ID = strcpy(AST->ID, newLeaf->ID);
            //AST->ID[strlen(newLeaf->ID)] = '\0';
            /*AST->changed = malloc(sizeof(char)*strlen(newLeaf->changed));
            AST->changed = strcpy(AST->changed, newLeaf->changed);
            AST->changed[strlen(newLeaf->changed)] = '\0';*/
            //free(newLeaf->content);
            //newLeaf->content = NULL;
            //free(newLeaf);
            //newLeaf = NULL;
        //}
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
void makeTree(char* ID, tASTPointer* leftPointer, tASTPointer* rightPointer, tASTPointer* AST) {
    if (leftPointer == NULL || rightPointer == NULL) {
        errorHandling(99);
        AST = NULL;
    } else {
        //tASTPointer* newTree = malloc(sizeof(struct tAST));
        /*if (newTree == NULL) {
            errorHandling(99);
            AST = NULL;
        } else {*/
            //newTree->LeftPointer = leftPointer;
            //newTree->RightPointer = rightPointer;
            AST->RightPointer = rightPointer;
            AST->LeftPointer = leftPointer;
            if (strcmp(ID, "l") == 0) {
                AST->ID = malloc(sizeof(char)*2);
                strcpy(AST->ID, "<");
                AST->ID[1] = '\0';
            } else if (strcmp(ID, "g") == 0) {
                AST->ID = malloc(sizeof(char)*2);
                strcpy(AST->ID, ">");
                AST->ID[1] = '\0';
            } else if (strcmp(ID, ".") == 0) {
                AST->ID = malloc(sizeof(char)*3);
                strcpy(AST->ID, "<=");
                AST->ID[2] = '\0';
            } else if (strcmp(ID, ",") == 0) {
                AST->ID = malloc(sizeof(char)*3);
                strcpy(AST->ID, ">=");
                AST->ID[2] = '\0';
            } else if (strcmp(ID, "?") == 0) {
                AST->ID = malloc(sizeof(char)*3);
                strcpy(AST->ID, "==");
                AST->ID[2] = '\0';
            } else if (strcmp(ID, "!") == 0) {
                AST->ID = malloc(sizeof(char)*3);
                strcpy(AST->ID, "!=");
                AST->ID[2] = '\0';
            } else {
                AST->ID = malloc(sizeof(char)*(strlen(ID)+1));
                strcpy(AST->ID, ID);
                AST->ID[strlen(ID)] = '\0';
            }
            if (correctSemantics(leftPointer->content, rightPointer->content) != 0) {
                //BSTNodeContentPtr* tmpContent = malloc(sizeof(struct BSTNodeContent));
                /*if (tmpContent == NULL) {
                    errorHandling(99);
                    AST = NULL;
                } else {*/
                    switch (matchingTypes(rightPointer->content, leftPointer->content, ID) ) {
                        case 1:
                            // either operand is variable or user-defined function => not changing type
                            //tmpContent->type = NULL;
                            AST->content->type = malloc(sizeof(char)*2);
                            AST->content->type = strcpy(AST->content->type, "x");
                            AST->content->type[1] = '\0';
                            break;
                        case 2:
                            // every operation  => changing type
                            if (decideType(leftPointer->content, rightPointer->content, ID) != NULL) {
                                size_t typeLen = strlen(decideType(leftPointer->content, rightPointer->content, ID));
                                AST->content->type = malloc(sizeof(char)*(typeLen + 2));
                                memcpy(AST->content->type, decideType(leftPointer->content, rightPointer->content, ID),typeLen);
                                AST->content->type[typeLen] = '\0';
                            } else {
                                AST->content->type = malloc(sizeof(char)*2);
                                AST->content->type = strcpy(AST->content->type, "x");
                                AST->content->type[1] = '\0';
                            }
                            break;
                        default:
                            // error
                            AST->content->type = malloc(sizeof(char)*2);
                            AST->content->type = strcpy(AST->content->type, "x");
                            AST->content->type[1] = '\0';
                            break;
                    }
                    //tmpContent->defined = 1;
                    AST->content->var = malloc(sizeof(char)*2);
                    AST->content->var = strcpy(AST->content->var, "x");
                    AST->content->var[1] = '\0';
                    AST->content->name = malloc(sizeof(char)*2);
                    AST->content->name = strcpy(AST->content->name, "x");
                    AST->content->name[1] = '\0';
                    AST->content->defined = 1;
                    //AST->changed= NULL;
                    //newTree->content = tmpContent;
                    //free(tmpContent);
                    //tmpContent=NULL;
                    //AST->RightPointer = newTree->RightPointer;
                    //AST->LeftPointer = newTree->LeftPointer;
                    //AST->content = newTree->content;
                    //AST->ID = malloc(sizeof(char)*strlen(newTree->ID));
                    //AST->ID = strcpy(AST->ID, newTree->ID);
                    //AST->ID[strlen(newTree->ID)] = '\0';
                    //free(newTree);
                    //newTree=NULL;
                //}
            } else {
                errorHandling(4);
                //AST = NULL;
                AST->content->type = malloc(sizeof(char) * 2);
                AST->content->type = strcpy(AST->content->type, "x");
                AST->content->type[1] = '\0';
                AST->content->name = malloc(sizeof(char) * 2);
                AST->content->name = strcpy(AST->content->name, "x");
                AST->content->name[1] = '\0';
                AST->content->defined = 1;
                AST->content->var = malloc(sizeof(char) * 2);
                AST->content->var = strcpy(AST->content->var, "x");
                AST->content->var[1] = '\0';
                AST->ID = malloc(sizeof(char) * 2);
                AST->ID = strcpy(AST->ID, "x");
                AST->ID[1] = '\0';
            }
        //}
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
void tStackASTPush(tStackASTPtr* stack, BSTNodePtr node, Token* token, char* operator, tASTPointer* leftSide, tASTPointer* righSide) {
    if (stack == NULL || stack->top == MAX || node == NULL) {
        errorHandling(99);
    } else {
        stack->top++;
        //tASTPointer* tmpAST = malloc(sizeof(struct tAST));
        //tmpAST->content = malloc(sizeof(struct BSTNodeContent));
        stack->body[stack->top] = malloc(sizeof(struct tAST)*30);
        stack->body[stack->top]->content = malloc(sizeof(struct BSTNodeContent));
        if (leftSide == NULL || righSide == NULL) {
            makeLeaf(node, token, stack->body[stack->top]);
        } else {
            makeTree(operator, leftSide, righSide, stack->body[stack->top]);
        }
        //stack->body[stack->top] = tmpAST;

        //free(tmpAST->content);
        //tmpAST->content = NULL;
        //free(tmpAST);
        //tmpAST = NULL;
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
    if (stack != NULL) {
        while (stack->top > 0) {
            tASTDispose(stack->body[stack->top]->LeftPointer);
            tASTDispose(stack->body[stack->top]->RightPointer);
            //free(stack->body[stack->top]->changed);
            free(stack->body[stack->top]->ID);
            free(stack->body[stack->top]->content->var);
            free(stack->body[stack->top]->content->name);
            free(stack->body[stack->top]->content->type);
            free(stack->body[stack->top]->content);
            free(stack->body[stack->top]);
            stack->body[stack->top] = NULL;
            stack->top--;
        }
    } else {
        errorHandling(99);
    }
}
