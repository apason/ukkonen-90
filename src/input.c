#include <stdio.h>  // fprintf(), fopen, feof(), NULL
#include <string.h> // strlen(), strcpy(), memset()
#include <stdlib.h> // malloc(), realloc(), exit(), EXIT_FAILURE

#include "init.h"   // checkNULL macro
#include "input.h"  // struct key_words, MAX_LINE

/*
 * Assume that input file contains one key word per line
 * No keyword should be over MAX_LINE -2 characters long
 */
const struct key_words * const readInput(const char * const file_name){

    struct key_words * const keys = malloc(sizeof(keys));

    checkNULL(keys, "malloc");

    FILE *input = fopen(file_name, "r");
    
    checkNULL(input, "fopen");

    char line[MAX_LINE];
    int  length;
    
    keys->R = malloc(sizeof(keys->R) + 1);

    checkNULL(keys->R, "malloc");

    /* This should never be read */
    keys->R[0] = NULL;
    keys->len = 1;

    while(!feof(input)){
        memset(line, '\0', MAX_LINE);
        if(fgets(line, MAX_LINE, input) == NULL){
            if(ferror(input)){
                fprintf(stdout, "ERROR: read error while reading the input file");
                exit(EXIT_FAILURE);
            }
            break;
        }

        if(strlen(line) == MAX_LINE -1 && line[MAX_LINE -2] != '\n')
            fprintf(stderr, "Warning: Input file contains longer lines than MAX_LINE (%d)\n", MAX_LINE);

        /* Skip empty lines */
        if(strlen(line) < 1 || line[0] == '\n')
            continue;

        length = strlen(line);

        keys->R[keys->len] = malloc(sizeof(*keys->R[keys->len]) * length);
        checkNULL(keys->R[keys->len], "malloc");
         

        line[length-1] = '\0';
        strcpy(keys->R[keys->len], line);
        keys->R[keys->len][length-1] = '\0';

        keys->len++;
        
        keys->R = realloc(keys->R, sizeof(keys->R) * (keys->len+1));
        checkNULL(keys->R, "realloc");

    }

    keys->meta = malloc(sizeof(*keys->meta) * keys->len);
    checkNULL(keys->meta, "malloc");
    
    memset(keys->meta, 0, sizeof(*keys->meta) * keys->len);
    
    return keys;
}
