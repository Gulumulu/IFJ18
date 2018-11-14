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
#include <string.h>

typedef struct BSTNodeContent {
    int declared;
    int defined;
    char* type;
    char* name;
} BSTNodeContentPtr;

typedef struct BSTNode {
    int ID;
    BSTNodeContentPtr* content;
    struct BSTNode* LeftPtr;
    struct BSTNode* RightPtr;
} *BSTNodePtr;

unsigned int hash_id(char* str);
void BSTInit(BSTNodePtr* root);
void BSTDispense(BSTNodePtr* root);
void BSTInsert(BSTNodePtr* root, BSTNodeContentPtr *content, unsigned int ID);
BSTNodeContentPtr* BSTsearch(BSTNodePtr* root, char* searchedID);

#endif //IFJ_SYMTABLE_H