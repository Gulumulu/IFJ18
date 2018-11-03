//
// Created by parek on 11/3/18.
//
/**
 *  Header for generating AST and appropriated semantic actions.
 *  Implemented by Marek Varga, xvarga14
 */
#ifndef IFJ_SEMATNIC_H
#define IFJ_SEMATNIC_H

#include "symtable.h"

// structure for node
/*typedef struct tNode {
    BSTNodeContentPtr content;
} tNodePointer;*/

// structure for AST
typedef struct tAST {
    char ID;
    //tNodePointer* content;
    BSTNodeContentPtr* content;
    struct tAST* LeftPointer;
    struct tAST* RightPointer;
} tASTPointer;

// function for working with AST
void tASTInit(tASTPointer* AST);
tASTPointer* makeLeaf(BSTNodeContentPtr* symtablePointer);
tASTPointer* makeTree(char ID, tASTPointer* leftPointer, tASTPointer* rightPointer);

// helper functions
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *righContent);
#endif //IFJ_SEMATNIC_H
