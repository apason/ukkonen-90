#ifdef RB_TREE_GOTO

#include <stdio.h>   // NULL

#include "init.h"    // STATE, ALPHABET
#include "goto.h"    // goto_function
#include "rb_tree.h" // rbInsert(), rbSearch(), newTree(), printTree()

void gotoSet(goto_function g, STATE state, ALPHABET key, STATE value){
    
    rbInsert(g[state], key, value);
}

STATE gotoGet(goto_function g, STATE state, ALPHABET key){
    
    struct rb_node * value = rbSearch(g[state], key);

    return value->value;
}

void gotoInit(goto_function g, size_t len){
    
    g[len] = newTree();
}

void printGotoFunction(goto_function g, size_t len){

    printf("Printing goto function in form (character, state). Goto transitions (from and to) state 1 are not shown.\n");
    
    for(int i = 1; i <= len; i++){

        printf("State %d/%ld:\n", i, len);
        
        printTree(g[i]);

        printf("\n\n");
    }
}

#endif
