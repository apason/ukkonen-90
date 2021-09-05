#ifdef ARRAY_GOTO

#include <stdio.h>  // perror(), NULL 
#include <stdlib.h> // malloc()
#include <string.h> // memset()

#include "init.h"   // STATE, ALPHABET, ALPHABET_BYTES
#include "goto.h"   // goto_function

STATE gotoGet(goto_function g, STATE state, ALPHABET key){

    if(key == 0)
        printf("get request for key 0\n");

    return (*g[state])[key -1];
}

void gotoSet(goto_function g, STATE state, ALPHABET key, STATE value){

    if(key == 0)
        printf("set request for key 0\n");
    
    (*g[state])[key -1] = value;
}

void gotoInit(goto_function g, size_t len){
    g[len] = malloc(ALPHABET_BYTES);

    if(g[len] == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    memset(g[len], 0, ALPHABET_BYTES);
}

// not changed after alphabet_max
void printGotoFunction(goto_function g, size_t len){

    printf("Printing goto function in form (character, state). Goto transitions (from and to) state 1 are not shown.\n");

    for(int i = 1; i <= len; i++){

        printf("State %d/%ld:\n", i, len);

        for(int j = 0; j < ALPHABET_MAX; j++)
            if((*g[i])[j] != 0 && (*g[i])[j] != 1)
                printf("(%c, %4d)\t", j, (*g[i])[j]);

        printf("\n\n");
    }
}

#endif
