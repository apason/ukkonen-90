#include <stdio.h>  // fprintf(), fopen, feof(), ftell(), fseek(), NULL
#include <string.h> // strlen(), strcpy(), memset()
#include <stdlib.h> // malloc(), realloc(), exit(), EXIT_FAILURE
#include <time.h>   // time()

#include "init.h"   // checkNULL macro
#include "input.h"  // struct key_words, MAX_LINE
#include "usage.h"  // startTimer(), endTimer()

#include "strquicksort.h"

static struct key_words * removeDuplicates(struct key_words * keys);
static struct key_words * initKeys(void);

/*
 * Assume that input file contains one key word per line
 * No keyword should be over MAX_LINE -2 characters long
 * Return value contains no duplicate key words.
 */
const struct key_words * const readInput(const char * const file_name){

#ifdef INFO
    startTimer("  Reading of the input");
#endif

    char line[MAX_LINE];
    int  length;

    FILE *input = fopen(file_name, "r");
    checkNULL(input, "fopen");

    struct key_words *keys = initKeys();

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
#ifndef LONG_KEYS
        if(strlen(line) > 256)
            fprintf(stderr, "WARNING: LONG_KEYS not defined but keys over 256 are found\n");
#endif

        /* Skip empty lines */
        if(strlen(line) < 1 || line[0] == '\n')
            continue;

        length = strlen(line);

        keys->R[keys->len] = malloc(sizeof(*keys->R[keys->len]) * length);
        checkNULL(keys->R[keys->len], "malloc");

        line[length-1] = '\0'; // Override the newline
        strcpy(keys->R[keys->len], line);


#ifdef INFO
        /* Set the global value to be used with statistics */
        original_input_length += length -1;
#endif

        keys->len++;
        
        keys->R = realloc(keys->R, sizeof(keys->R) * (keys->len+1));
        checkNULL(keys->R, "realloc");

    }

    fclose(input);

#ifdef INFO
    endTimer("  Reading of the input");
    startTimer("  Removal of the duplicates");
#endif    
    keys = removeDuplicates(keys);

#ifdef INFO
    endTimer("  Removal of the duplicates");
#endif

    return keys;
}

/* Read samples of length row_length from the input file. Repeat until cut * |F|, where
 * F is the input file, characters have been read. Return value contains no duplicate
 * key words.
 */
const struct key_words * const readSamplesFromFile(const char * const file_name, size_t row_length, float cut){

#ifndef LONG_KEYS
    if(row_length > 256)
        fprintf(stderr, "WARNING: LONG_KEYS is not defined but row_length is over 256");
#endif
    
    srand(time(NULL));
    
    char line[row_length +1];

    /* Initialized after the input file is open */
    size_t file_size;
    size_t iterations;
    size_t length;
    
    FILE *input = fopen(file_name, "r");
    checkNULL(input, "fopen");
    
    struct key_words * keys = initKeys();

    fseek(input, 0, SEEK_END);
    file_size = ftell(input);
    iterations = (file_size / row_length) * cut;


#ifdef INFO
    /* Set the global value to be used for statistics */
    original_input_length = iterations * row_length;
#endif
    
    for(size_t i = 0; i < iterations; i++){

        memset(line, '\0', row_length +1);

        //Get random position from file
        fseek(input, rand()%(file_size -row_length), SEEK_SET);

        //this should never give an error
        if(fread(line, sizeof(ALPHABET), row_length, input) != sizeof(ALPHABET)*row_length){
            fprintf(stderr, "Unexpected ERROR: fread\n");
            exit(EXIT_FAILURE);
        }
        
        /* Add the key to R.. */
        
        length = strlen(line);

        keys->R[keys->len] = malloc(sizeof(*keys->R[keys->len]) * length +1);
        checkNULL(keys->R[keys->len], "malloc");

        strcpy(keys->R[keys->len], line);

        keys->len++;
        
        keys->R = realloc(keys->R, sizeof(keys->R) * (keys->len+1));
        checkNULL(keys->R, "realloc");

    }

    keys = removeDuplicates(keys);
    
    return keys;
}

static struct key_words * removeDuplicates(struct key_words *keys){

#ifdef INFO
    size_t duplicate_count = 0;
    reduced_input_length = original_input_length;
#endif

    /* Move the last item to first. At this point the first element only
     * contains a NULL reference so no information is lost. 
     */
    keys->R[0] = keys->R[keys->len -1];
    keys->len--;

    /* Sort keywords */
    keys = StringQuickSort(keys, 0);

    /* Remove duplicates */
    struct key_words * uniqued_keys = NewArray(0);
    add(uniqued_keys, NULL);
    
    char * prev = keys->R[0];
    for(int i = 1; i < keys->len; i++){

        if(strcmp(prev, keys->R[i]) != 0){
            add(uniqued_keys, prev);
            prev = keys->R[i];
            continue;
        }
        free(keys->R[i]);
#ifdef INFO
        duplicate_count++;
        reduced_input_length -= strlen(prev);
#endif
    }

    if(strcmp(keys->R[keys->len-1], uniqued_keys->R[uniqued_keys->len-1]) != 0)
        add(uniqued_keys, keys->R[keys->len-1]);
    else{
#ifdef INFO
        duplicate_count++;
        reduced_input_length -= strlen(keys->R[keys->len-1]);
#endif
        ;
    }

    free(keys->R);
    free(keys->meta);
    free(keys);

    uniqued_keys->meta = calloc(uniqued_keys->len, sizeof(*uniqued_keys->meta));
    checkNULL(uniqued_keys->meta, "malloc");

#ifdef INFO
    printf("Removed duplicates: %ld\n\n", duplicate_count);
#endif
    
    return uniqued_keys;

}

static struct key_words * initKeys(void){

    struct key_words * keys = malloc(sizeof(*keys));
    checkNULL(keys, "malloc");

    keys->R = malloc(sizeof(keys->R) * 2);
    checkNULL(keys->R, "malloc");

    /* This should never be read */
    keys->R[0] = NULL;
    keys->len = 1;

    return keys;
}
