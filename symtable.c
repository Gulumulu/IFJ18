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
unsigned int hash_id(char* str) {
    int c = 0;
    unsigned int hash = 5381;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;        /* hash * 33 + c */

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
void BSTDispense(BSTNodePtr* root) {
    if (root == NULL) {
        return;
    }

    BSTNodePtr disposedNode = *root;
    BSTDispense(&disposedNode->LeftPtr);
    BSTDispense(&disposedNode->RightPtr);
    free(disposedNode);
    (*root) = NULL;
}

/**
 * Function insetrs content into BT depending on the ID
 *
 * @param root pointer to the binary tree
 * @param content the content we want to insert
 * @param ID hash integer serving as a key
 */
void BSTInsert(BSTNodePtr* root, BSTNodeContentPtr *content, unsigned int ID) {
        if (*root == NULL) {
            BSTNodePtr tmp = malloc(sizeof(struct BSTNode));
            if (tmp != NULL) {
                tmp->ID = ID;
                tmp->content = content;
                tmp->LeftPtr = NULL;
                tmp->RightPtr = NULL;
                *root = tmp;
            }
        } else {
            if ((*root)->ID == ID) {
                (*root)->content = content;
            }
            else if ((*root)->ID < ID) {
                BSTInsert(&(*root)->RightPtr, content, ID);
            }
            else if ((*root)->ID > ID) {
                BSTInsert(&(*root)->LeftPtr, content, ID);
            }
        }
}

/**
 * Function searches BST for node with matching ID.
 *
 * @param root pointer to root BSTNode
 * @param searchedID pointer to char (string) for searching ID
 * @return BSTNodeContentPtr pointer to matched node or NULL if node was not found
 */
BSTNodeContentPtr* BSTsearch(BSTNodePtr* root, char* searchedID) {
    if (root == NULL) {
        return NULL;
    }
/*
    BSTNodeContentPtr* content = (*root)->content;
    if (strcmp(content->ID,searchedID) == 0) {                  // searched node is current node
        return content;
    } else if (strcmp(content->ID, searchedID) < 0) {
        BSTNodeContentPtr* tmpReturn = BSTsearch(&(*root)->LeftPtr, searchedID);
        if (tmpReturn != NULL) {
            return tmpReturn;
        }
    } else {
        BSTNodeContentPtr* tmpReturn = BSTsearch(&(*root)->RightPtr, searchedID);
        if (tmpReturn != NULL) {
            return tmpReturn;
        }
    }
*/
    return NULL;
}

