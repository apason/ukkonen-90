#include <stdio.h>  // printf(), NULL 
#include <stdlib.h> // malloc()

#include "init.h"    // STATE, ALPHABET
#include "rb_tree.h" // struct rb_[tree, node], enum COLOR

/* All nil nodes in the tree points to the same sentinel node */
struct rb_node sentinel = {NULL, NULL, NULL, BLACK, 0, 0};
struct rb_node * nil = &sentinel;

/* Static functions. Only used within this translation unit */
static void fixRBTree   (struct rb_tree * const tree, struct rb_node * current);
static void rightRotate (struct rb_tree * const tree, struct rb_node * x);
static void leftRotate  (struct rb_tree * const tree, struct rb_node * x);
static void printSubTree(struct rb_node * node);
static void deleteNode  (struct rb_node * node);

static struct rb_node * newRBNode(ALPHABET key, STATE value);

/* Interface function. Inserts the key to the tree */
void rbInsert(struct rb_tree * const tree, ALPHABET key, STATE value){

    struct rb_node * new = newRBNode(key, value);
    struct rb_node * x   = tree->root;
    struct rb_node * y   = nil;


    while(x != nil){

        y = x;

        if(new->key < x->key)
            x = x->left;
        else
            x = x->right;

        new->parent = y;
    }
    
    if(y == nil)
        tree->root = new;
    else if(new->key < y->key)
        y->left = new;
    else
        y->right = new;

    fixRBTree(tree, new);
}

/* Interface function. Returns the node containin the key or NULL if not found */
struct rb_node * rbSearch(struct rb_tree * tree, ALPHABET key){

    struct rb_node * node = tree->root;

    while(node != nil && key != node->key){

        if(key < node->key)
            node = node->left;
        else
            node = node->right;
    }
    return node;
}

void deleteTree(struct rb_tree * tree){

    if(tree == NULL)
        return;

    deleteNode(tree->root);

    free(tree);
}

static void deleteNode(struct rb_node * node){

    if(node == nil)
        return;

    deleteNode(node->left);
    deleteNode(node->right);

    free(node);
}

struct rb_tree * newTree(){

    struct rb_tree * tree = malloc(sizeof(*tree));

    checkNULL(tree, "malloc");

    tree->root = nil;

    return tree;
}

/* Fixes the possible Red-Black violations after the new node is inserted */
static void fixRBTree(struct rb_tree * const tree, struct rb_node * current){

    struct rb_node * uncle;
    
    while(current->parent->color == RED){

        if(current->parent == current->parent->parent->left){

            uncle = current->parent->parent->right;

            if(uncle->color == RED){

                current->parent->color = BLACK;
                uncle->color = BLACK;
                current->parent->parent->color = RED;
                current = current->parent->parent;
            } else {
                if(current == current->parent->right){
                    current = current->parent;
                    leftRotate(tree, current);
                }
                current->parent->color = BLACK;
                current->parent->parent->color = RED;
                rightRotate(tree, current->parent->parent);
            }

        } else {
            
            uncle = current->parent->parent->left;

            if(uncle->color == RED){

                current->parent->color = BLACK;
                uncle->color = BLACK;
                current->parent->parent->color = RED;
                current = current->parent->parent;
            } else {
                if(current == current->parent->left){
                    current = current->parent;
                    rightRotate(tree, current);
                }
                current->parent->color = BLACK;
                current->parent->parent->color = RED;
                leftRotate(tree, current->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

/* Rotates the subtree starting from node x one cycle to right */
static void rightRotate(struct rb_tree * const tree, struct rb_node * x){

    struct rb_node * y = x->left;
    
    x->left = y->right;
    
    if(y->right != nil)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == nil)
        tree->root = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;

}

/* Rotates the subtree x to left. This is symmetric with rightRotate */
static void leftRotate(struct rb_tree * tree, struct rb_node * x){

    struct rb_node * y = x->right;
    
    x->right = y->left;
    
    if(y->left != nil)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == nil)
        tree->root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;

}

/* Creates and initializes the new node with value key */
static struct rb_node * newRBNode(ALPHABET key, STATE value){

    struct rb_node * new = malloc(sizeof(*new));

    checkNULL(new, "malloc");

    new->parent = nil;
    new->left = nil;
    new->right = nil;
    new->color = RED;
    new->key = key;
    new->value = value;
    
    return new;
}

void printTree(struct rb_tree *tree){
    printSubTree(tree->root);
}

static void printSubTree(struct rb_node *node){
    
    if(node == nil)
        return;

    printSubTree(node->left);
    
    if(node->value != 1)
        printf("(%c, %4d)\t", node->key, node->value);
    
    printSubTree(node->right);

}
