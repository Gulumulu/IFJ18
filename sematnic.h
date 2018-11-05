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
#include "errors.h"

#define MAX 50

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

// stack of ASTs
typedef struct tStackAST{
    tASTPointer* body[MAX];
    int top;
} tStackASTPtr;

// function for working with AST
void tASTInit(tASTPointer* AST);
void tASTDispose(tASTPointer* AST);
tASTPointer* makeLeaf(BSTNodeContentPtr* symtablePointer);
tASTPointer* makeTree(char ID, tASTPointer* leftPointer, tASTPointer* rightPointer);

// functions for working with stack of ASTs
void tStackASTInit(tStackASTPtr* stack);
void tStackASTPush(tStackASTPtr* stack, tASTPointer* AST);
tASTPointer* tStackASTPop(tStackASTPtr* stack);
void tStackASTDispose(tStackASTPtr* stack);

// helper functions
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent);
#endif //IFJ_SEMATNIC_H
