/**
 * Table of symbols functions
 *
 * Implemented by Marek Varga           xvarga14
 *                Gabriel Quirschfeld   xquirs00
 */

#include "symtable.h"
#include <string.h>

/**
 * Function generates a hash from the string
 *
 * @param str the identifier string
 * @return hash integer
 */
unsigned long hash_id(char* str) {
    int c = 0;
    unsigned long hash = 5381;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;        // hash * 33 + c

    return hash;
}

/**
 * Function initializes root node in BST.
 *
 * @param root pointer to root BSTNode
 */
void BSTInit(BSTNodePtr* root) {
    *root = NULL;
}

/**
 * Function dispenses all nodes in BST.
 *
 * @param root pointer to root BSTNode
 */
void BSTDispose(BSTNodePtr* root) {
    if (*root != NULL) {
        BSTDispose(&(*root)->LeftPtr);
        BSTDispose(&(*root)->RightPtr);
        free(*root);
        *root = NULL;
    }
}

/**
 * Function inserts content into BT depending on the ID
 *
 * @param root pointer to the binary tree
 * @param content the content we want to insert
 * @param ID hash integer serving as a key
 * @param func_id id of the function the local id belongs to, 0 if function id
 */
void BSTInsert(BSTNodePtr* root, BSTNodeContentPtr* content, unsigned long ID, unsigned long func_id) {
    if (*root == NULL) {    // if the tree is empty we need to create a new root
        BSTNodePtr tmp = malloc(sizeof(struct BSTNode));
        if (tmp != NULL) {
            tmp->ID = ID;
            tmp->func_id = func_id;
            tmp->content = content;
            tmp->LeftPtr = NULL;
            tmp->RightPtr = NULL;
            *root = tmp;
        }
    } else {
        if ((*root)->ID == ID) {    // if there already is a root with this id we just update the content
            (*root)->content = content;
        } else if ((*root)->ID < ID) {  // if the id is higher than the id of the root we are in we move to the right subtree
            BSTInsert(&(*root)->RightPtr, content, ID, func_id);
        } else if ((*root)->ID > ID) {  // if the id is lower than the id of the root we are in we move to the right subtree
            BSTInsert(&(*root)->LeftPtr, content, ID, func_id);
        }
    }
}

/**
 * Function searches BST for node with matching ID
 *
 * @param root pointer to root BSTNode
 * @param ID id of the node we are looking for
 * @return BSTNodeContentPtr pointer to matched node or NULL if node was not found
 */
BSTNodeContentPtr* BSTSearch(BSTNodePtr* root, unsigned long ID) {
    if ((*root) == NULL) {
        return NULL;
    }

    if ((*root)->ID == ID) {
        return (*root)->content;
    }
    else if ((*root)->ID < ID) {
        return BSTSearch(&(*root)->RightPtr, ID);
    }
    else if ((*root)->ID > ID) {
        return BSTSearch(&(*root)->LeftPtr, ID);
    }
    else {
        return NULL;
    }
}

BSTNodePtr* findNode(struct BSTNode** array, char* functionName) {
    if ((*array) == NULL || strcmp(functionName, "") == 0) {
        return NULL;
    } else {
        unsigned long f_id = hash_id(functionName);
        for (int i = 0; i < 10000; i++) {
            if (array[i] != NULL) {
                if (array[i]->func_id== f_id) {
                    return &array[i];
                }
            }
        }
        return NULL;
    }
}
