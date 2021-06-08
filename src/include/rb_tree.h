#ifndef RB_TREE_H
#define RB_TREE_H

#include "init.h" // STATE, ALPHABET

enum COLOR { RED, BLACK };

struct rb_node{
    struct rb_node * parent;
    struct rb_node * left;
    struct rb_node * right;
    enum COLOR color;
    ALPHABET key;
    STATE value;
};

struct rb_tree {
    struct rb_node * root;
};

extern struct rb_node * NIL;

extern void rbInsert(struct rb_tree * const tree, ALPHABET key, STATE value);

extern struct rb_tree * newTree();

extern struct rb_node * rbSearch(struct rb_tree * tree, ALPHABET key);

extern void printTree(struct rb_tree *tree);

#endif /* RB_TREE_H */
