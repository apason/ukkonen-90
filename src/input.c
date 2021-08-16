#include <stdio.h>  // fprintf(), fopen, feof(), ftell(), fseek(), NULL
#include <string.h> // strlen(), strcpy(), memset()
#include <stdlib.h> // malloc(), realloc(), exit(), EXIT_FAILURE

#include "init.h"   // checkNULL macro
#include "input.h"  // struct key_words, MAX_LINE

#include "strquicksort.h"

static struct key_words * removeDuplicates(struct key_words * keys);
static struct key_words * initKeys(void);

/*
 * Assume that input file contains one key word per line
 * No keyword should be over MAX_LINE -2 characters long
 * Return value contains no duplicate key words.
 */
const struct key_words * const readInput(const char * const file_name){

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
        input_length += length -1;
#endif

        keys->len++;
        
        keys->R = realloc(keys->R, sizeof(keys->R) * (keys->len+1));
        checkNULL(keys->R, "realloc");

    }

    keys = removeDuplicates(keys);

    return keys;
}

/* Read samples of length row_length from the input file. Repeat until cut * |F|, where
 * F is the input file, characters have been read. Return value contains no duplicate
 * key words.
 */
const struct key_words * const readSamplesFromFile(const char * const file_name, size_t row_length, float cut){

    srand(10);
    
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
    input_length = iterations * row_length;
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

#ifdef INFO
        duplicate_count++;
#endif
    }

    if(strcmp(keys->R[keys->len-1], uniqued_keys->R[uniqued_keys->len-1]) != 0)
        add(uniqued_keys, keys->R[keys->len-1]);
    else{
#ifdef INFO
        duplicate_count++;
#endif
        ;
    }

    free(keys);

    uniqued_keys->meta = malloc(sizeof(*uniqued_keys->meta) * uniqued_keys->len);
    checkNULL(uniqued_keys->meta, "malloc");

    memset(uniqued_keys->meta, 0, sizeof(*uniqued_keys->meta) * uniqued_keys->len);

#ifdef INFO
    printf("\nRemoved duplicates: %ld\n", duplicate_count);
#endif
    
    return uniqued_keys;

}

static struct key_words * initKeys(void){

    struct key_words * keys = malloc(sizeof(keys));
    checkNULL(keys, "malloc");

    keys->R = malloc(sizeof(keys->R) + 1);
    checkNULL(keys->R, "malloc");

    /* This should never be read */
    keys->R[0] = NULL;
    keys->len = 1;

    return keys;
}
