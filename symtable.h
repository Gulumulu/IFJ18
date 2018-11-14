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
    int defined;        // true if defined, otherwise false
    char* type;         //
    char* name;         // the string of the identifier
} BSTNodeContentPtr;

typedef struct BSTNode {
    unsigned long ID;               // hash id of the identifier
    unsigned long func_id;          // if the identifier is function -> 0, otherwise the hash id of the function the identifier belongs under
    BSTNodeContentPtr* content;     // the contents (string, status)
    struct BSTNode* LeftPtr;        // pointer to the left identifier
    struct BSTNode* RightPtr;       // pointer to the right identifier
} *BSTNodePtr;

unsigned long hash_id(char* str);
void BSTInit(BSTNodePtr* root);
void BSTDispose(BSTNodePtr* root);
void BSTInsert(BSTNodePtr* root, BSTNodeContentPtr *content, unsigned long ID, unsigned long func_id);
BSTNodeContentPtr* BSTSearch(BSTNodePtr* root, unsigned long ID);
BSTNodePtr* findNode(BSTNodePtr** array, char* functionName);

#endif //IFJ_SYMTABLE_H