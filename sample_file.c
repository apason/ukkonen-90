#include <stdio.h>  // fprintf(), fopen, feof(), ftell(), fseek(), NULL
#include <string.h> // strlen(), strcpy(), memset()
#include <stdlib.h> // malloc(), realloc(), exit(), EXIT_FAILURE
#include <time.h>   // time()

#include "input.h"  // struct key_words

#include "strquicksort.h"



/* gcc -I src/include sample_file.c src/strquicksort.c */

static struct key_words * removeDuplicates(struct key_words * keys);
static struct key_words * initKeys(void);


int main(int argc, char *argv[]){

    char *filename = argv[1];
    size_t row_length = atol(argv[2]);
    float cut = atof(argv[3]);

    const struct key_words * keys = readSamplesFromFile(filename, row_length, cut);

    char outfile[128];

    sprintf(outfile, "%s_%s_%ld", filename, argv[3], row_length);

    FILE *out = fopen(outfile, "w");

    for(int i = 1; i < keys->len; i++)
        fprintf(out, "%s\n", keys->R[i]);

    fclose(out);

    return EXIT_SUCCESS;
}


/* Read samples of length row_length from the input file. Repeat until cut * |F|, where
 * F is the input file, characters have been read. Return value contains no duplicate
 * key words.
 */
const struct key_words * const readSamplesFromFile(const char * const file_name, size_t row_length, float cut){

    srand(time(NULL));
    //srand(9);
    
    char line[row_length +1];

    /* Initialized after the input file is open */
    size_t file_size;
    size_t iterations;
    size_t length;
    
    FILE *input = fopen(file_name, "r");
    
    struct key_words * keys = initKeys();

    fseek(input, 0, SEEK_END);
    file_size = ftell(input);

    size_t added = 0;
    iterations = (file_size / row_length) * cut;

    while(keys->len != iterations){
        
        size_t batch = iterations - keys->len;

        printf("Size of the next batch:\t%ld\n", batch);
        for(size_t i = 0; i < batch; i++){

            memset(line, '\0', sizeof(line));

            //Get random position from file
            fseek(input, rand()%(file_size -row_length), SEEK_SET);

            //this should never give an error
            if(fread(line, 1, row_length, input) != row_length){
                fprintf(stderr, "Unexpected ERROR: fread\n");
                exit(EXIT_FAILURE);
            }

            /* Add the key to R.. */
            length = strlen(line);

            keys->R[keys->len] = malloc(sizeof(*keys->R[keys->len]) * length +1);

            strcpy(keys->R[keys->len], line);

            keys->len++;
        
            keys->R = realloc(keys->R, sizeof(*keys->R) * (keys->len +1)); //miksi +1 ?

        }

        keys = removeDuplicates(keys); 
    }
    
    return keys;
}

static struct key_words * removeDuplicates(struct key_words *keys){

    /* Move the last item to first. At this point the first element only
     * contains a NULL reference so no information is lost. 
     */
    keys->R[0] = keys->R[keys->len -1]; // This is reversed in the following for loop
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
    }

    if(strcmp(keys->R[keys->len-1], uniqued_keys->R[uniqued_keys->len-1]) != 0)
        add(uniqued_keys, keys->R[keys->len-1]);

    free(keys->R);
    free(keys->meta);
    free(keys);

    uniqued_keys->meta = malloc(sizeof(*uniqued_keys->meta) * uniqued_keys->len);

    memset(uniqued_keys->meta, 0, sizeof(*uniqued_keys->meta) * uniqued_keys->len);

    //BUGI! Tää pitää tehdä myös masteriin (toinen file)
    uniqued_keys->R = realloc(uniqued_keys->R, sizeof(*uniqued_keys->R) * (uniqued_keys->len + 1));

    return uniqued_keys;

}

static struct key_words * initKeys(void){

    struct key_words * keys = malloc(sizeof(*keys));

    keys->R = malloc(sizeof(*keys->R) * 2);

    /* This should never be read */
    keys->R[0] = NULL;
    keys->len = 1;
    keys->meta = NULL;

    return keys;
}
