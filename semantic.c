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
    AST->ID = '\0';
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
 * Function searches for variable in symtable based on given token.
 *
 * @param token
 * @return
 */
/*BSTNodeContentPtr* findVariable(BSTNodePtr node) {
    if (node == NULL) {
        errorHandling(99);
        return NULL;
    } else {
        *//*if (token->type == s_id) {                              // find variable in symtable
            return BSTsearch(, token->content);
        } else if (token->type == s_int || token->type == s_exp_int || token->type == s_float) {
            // leaf will be a constant therefore creating new BSTNode is needed
            BSTNodeContentPtr* tmpNode = malloc(sizeof(struct BSTNodeContent));
            if (tmpNode == NULL) {
                errorHandling(99);
                return NULL;
            } else {
                tmpNode->type = "int";
                tmpNode->value = 3;
                tmpNode->defined = 1;
                return tmpNode;
            }
        } else {                                                // attempting to create wrong leaf
            errorHandling(99);
            return NULL;
        }*//*

    }
}*/

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
            newLeaf->ID = 'E';
            return newLeaf;
        }
    }
}

/**
 * Function checks whether types of operands of either nodes are matching.
 *
 * @param leftContent pointer to BSTNodeContent is pointer to content in BST
 * @param rightContent pointer to BSTNodeContent is pointer to content in BST
 * @return non-zero value when types are matching otherwise zero value is returned
 */
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent) {
    if (leftContent == NULL || rightContent == NULL) {
        errorHandling(99);
        return 0;
    } else {
        if (strcmp(leftContent->type,rightContent->type) == 0) {
            return 1;
        } else {
            return 0;
        }
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
    if (matchingTypes(leftContent, rightContent) == 0) {
        errorHandling(4);
        return 0;
    }
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
tASTPointer* makeTree(char ID, tASTPointer* leftPointer, tASTPointer* rightPointer) {
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
            newTree->ID = ID;
            if (correctSemantics(leftPointer->content, rightPointer->content) != 0) {
                BSTNodeContentPtr* tmpContent = malloc(sizeof(struct BSTNodeContent));
                if (tmpContent == NULL) {
                    errorHandling(99);
                    return NULL;
                } else {
                    tmpContent->type = malloc(strlen(leftPointer->content->type)+1);
                    memcpy(tmpContent->type, leftPointer->content->type, strlen(leftPointer->content->type));
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
    if (stack == NULL || stack->top == MAX) {
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
