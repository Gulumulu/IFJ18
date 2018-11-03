//
// Created by parek on 10/27/18.
//
/**
 * Implemented by Marek Varga, xvarga14
 * Header for table of symbols
 */
#ifndef IFJ_SYMTABLE_H
#define IFJ_SYMTABLE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct BSTNodeContent {
    char* ID;
    int declared;
    int defined;
    char* type;
} BSTNodeContentPtr;

typedef struct BSTNode {
    BSTNodeContentPtr* content;
    struct BSTNode* LeftPtr;
    struct BSTNode* RightPtr;
} *BSTNodePtr;

void BSTInit(BSTNodePtr* root);
void BSTDispense(BSTNodePtr* root);
BSTNodeContentPtr* BSTsearch(BSTNodePtr* root, char* searchedID);
void BSTInsert(BSTNodePtr* root, char* insertedID);

#endif //IFJ_SYMTABLE_H