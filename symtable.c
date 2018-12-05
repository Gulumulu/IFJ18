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

    while ( (c = *str++) ) {
        hash = ((hash << 5) + hash) + c;        // hash * 33 + c
    }

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
        free((*root)->content->type);
        free((*root)->content->name);
        free((*root)->content->var);
        free((*root)->content);
        free(*root);
        *root = NULL;
    }
}

void BSTContentDispose(BSTNodeContentPtr* content) {
    if (content != NULL) {
        free(content->type);
        content->type = NULL;
        free(content->var);
        content->var = NULL;
        free(content->name);
        content->name = NULL;
        //free(content);
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
        //BSTNodePtr tmp = malloc(sizeof(struct BSTNode));    // allocating space for the node
        (*root) = malloc(sizeof(struct BSTNode));
        (*root)->content = malloc(sizeof(struct BSTNodeContent));
        //if (tmp != NULL) {  // filling the node with information
        (*root)->ID = ID;
        (*root)->func_id = func_id;
        (*root)->content->type = malloc(sizeof(char) * (strlen(content->type)+1));
        (*root)->content->type = strcpy((*root)->content->type, content->type);
        (*root)->content->type[strlen((*root)->content->type)] = '\0';
        (*root)->content->defined = content->defined;
        (*root)->content->func_params = content->func_params;
        (*root)->content->name = malloc(sizeof(char) * (strlen(content->name)+1));
        (*root)->content->name = strcpy((*root)->content->name, content->name);
        (*root)->content->name[strlen((*root)->content->name)] = '\0';
        /*(*root)->content->var = malloc(sizeof(char) * (strlen(content->var)+1));
        (*root)->content->var = strcpy((*root)->content->var, content->var);
        (*root)->content->var[strlen((*root)->content->var)] = '\0';*/
        (*root)->LeftPtr = NULL;
        (*root)->RightPtr = NULL;
            //*root = tmp;
//            free(content->var);
//            free(content->name);
//            free(content->type);
//            free(content);
        //}
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
    if ((*root) == NULL) {      // if root is empty returns NULL
        return NULL;
    }

    if ((*root)->ID == ID) {        // if ID is the same as the node returns node
        return (*root)->content;
    }
    else if ((*root)->ID < ID) {    // if id we are searching for is higher than node id we move to the right
        return BSTSearch(&(*root)->RightPtr, ID);
    }
    else if ((*root)->ID > ID) {    // if id we are searching for is lower than node id we move to the left
        return BSTSearch(&(*root)->LeftPtr, ID);
    }
    else {
        return NULL;
    }
}

/**
 * Function finds the correct local symtable
 *
 * @param array array of local symtables we are searching in
 * @param functionName function for which we wanna get the local symtable
 * @return pointer to the local symtable
 */
BSTNodePtr* findNode(struct BSTNode** array, BSTNodePtr* globalSymtable, char* functionName) {
    if (strcmp(functionName, "") == 0) {
        errorHandling(99);
        return NULL;
    } else {
        if (strcmp(functionName, "Main") == 0) {
            // variable is defined in global symtable
            return globalSymtable;
        } else {
            // variable is defined in local symtable
            if ((*array) == NULL) {
                return NULL;
            } else {
                unsigned long f_id = hash_id(functionName);
                for (int i = 0; i < 1000; i++) {
                    if (array[i] != NULL) {
                        if (array[i]->func_id == f_id) {
                            return &array[i];
                        }
                    }
                }
                return NULL;
            }
        }
    }
}
