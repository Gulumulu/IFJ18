//
// Created by parek on 11/3/18.
//
/**
 *  Source file for generating AST and appropriated semantic actions.
 *  Implemented by Marek Varga, xvarga14
 */
#include <memory.h>
#include "sematnic.h"
#include "symtable.h"
#include "stdio.h"
#include "stdlib.h"
#include "errors.h"

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
 * @param righContent pointer to BSTNodeContent is pointer to content in BST
 * @return non-zero value when types are matching otherwise zero value is returned
 */
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *righContent) {
    if (leftContent == NULL || righContent == NULL) {
        errorHandling(99);
        return 0;
    } else {
        if (strcmp(leftContent->type,righContent->type) == 0) {
            return 1;
        } else {
            return 0;
        }
    }
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
            newTree->content = NULL;
            if (matchingTypes(leftPointer->content, rightPointer->content) != 0) {
                return newTree;
            } else {
                errorHandling(4);
                return NULL;
            }
        }
    }
}
