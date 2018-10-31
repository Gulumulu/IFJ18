//
// Created by parek on 10/27/18.
//
/**
 * Implemented by Marek Varga, xvarga14
 * Table of symbols
 */
#include "symtable.h"
#include <string.h>

/**
 * Function initializes root node in BST.
 *
 * @param root pointer to root BSTNode
 */
void BSTInit(BSTNodePtr* root) {
    (*root)->content = 0;
    (*root)->LeftPtr = NULL;
    (*root)->RightPtr = NULL;
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

/*void BSTInsert(BSTNodePtr* root, char* insertedID) {
    BSTNodePtr insertedNode;

    if ((insertedNode = malloc(sizeof(struct BSTNode))) != NULL) {
        insertedNode->content = insertedID;
        insertedNode->LeftPtr = NULL;
        insertedNode->RightPtr = NULL;

        if ((*root) == NULL) {                                      // assigning new root
            (*root) = insertedNode;
        } else {
            BSTNodePtr descendant = *root;
            while (descendant != NULL) {
                if (insertedID < descendant->content) {
                    if (descendant->LeftPtr != NULL) {
                        descendant = descendant->LeftPtr;
                    } else {
                        descendant->LeftPtr = insertedNode;
                    }
                } else {
                    if (descendant->RightPtr != NULL) {
                        descendant = descendant->RightPtr;
                    } else {
                        descendant->RightPtr = insertedNode;
                    }
                }
            }
        }
    }
}*/

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

    return NULL;
}

