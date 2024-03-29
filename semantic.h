/**
 * Project for IFJ course - compiler
 * Header for generating AST and appropriated semantic actions.
 *
 * Implemented by:  Gabriel Quirschfeld,    xquirs00
 *                  Marek Varga,            xvarga14
 */
#ifndef IFJ_SEMANTIC_H
#define IFJ_SEMANTIC_H

#include <stdbool.h>

#include "symtable.h"
#include "errors.h"
#include "scanner.h"
#include <string.h>
#include <regex.h>
#define MAX 50
int precedence;
// structure for node
/*typedef struct tNode {
    BSTNodeContentPtr content;
} tNodePointer;*/

// structure for AST
typedef struct tAST {
    char* ID;
    //char* changed;
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
void findVariable(BSTNodePtr node, Token* token, BSTNodeContentPtr* nodeContentPtr);
void makeLeaf(BSTNodePtr node, Token* token, tASTPointer* AST);
void makeTree(char* ID, tASTPointer* leftPointer, tASTPointer* rightPointer, tASTPointer* AST);

// functions for working with stack of ASTs
void tStackASTInit(tStackASTPtr* stack);
void tStackASTPush(tStackASTPtr* stack, BSTNodePtr node, Token* token, char* operator, tASTPointer* leftSide, tASTPointer* righSide);
tASTPointer* tStackASTPop(tStackASTPtr* stack);
void tStackASTDispose(tStackASTPtr* stack);

// helper functions
int matchingTypes(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent, char* ID);
char* decideType(BSTNodeContentPtr *leftContent, BSTNodeContentPtr *rightContent, char* ID);
#endif //IFJ_SEMATNIC_H
