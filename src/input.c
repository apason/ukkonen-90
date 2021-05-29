#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "input.h"

/*
 * Assume that input file contains one key word per line
 * No keyword should be over MAX_LINE characters long
 */
const struct key_words * const readInput(const char * const file_name){

    struct key_words * const keys = malloc(sizeof(keys));

    if(keys == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    FILE *input = fopen(file_name, "r");
    
    if(input == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    int  length;
    
    keys->R = malloc(sizeof(keys->R) + 1);
    if(keys->R == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* This should never be read */
    keys->R[0] = NULL;
    keys->len = 1;

    while(!feof(input)){
        memset(line, '\0', MAX_LINE);
        fgets(line, MAX_LINE, input);

        if(strlen(line) == MAX_LINE -1 && line[MAX_LINE -2] != '\n')
            fprintf(stderr, "Warning: Input file contains longer lines than MAX_LINE (%d)\n", MAX_LINE);

        /* Skip empty lines */
        if(strlen(line) < 1 || line[0] == '\n')
            continue;

        length = strlen(line);

        keys->R[keys->len] = malloc(sizeof(*keys->R) * length);
        if(keys->R[keys->len] == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
                     
        strncpy(keys->R[keys->len], line, length);
        keys->R[keys->len][length-1] = '\0';

        keys->len++;
        
        keys->R = realloc(keys->R, sizeof(keys->R) * (keys->len+1));
        if(keys->R == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    keys->meta = malloc(sizeof(*keys->meta) * keys->len);
    if(keys->meta == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    memset(keys->meta, 0, sizeof(*keys->meta) * keys->len);
    
    return keys;
}
