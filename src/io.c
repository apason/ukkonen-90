#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "init.h"    // STATE, ALPHABET, ALPHABET_MAX
#include "input.h"   // struct key_words

static ALPHABET *map_b;

size_t optimizeAlphabet(const struct key_words * keys){

    ALPHABET * map_a = (ALPHABET*) calloc(ALPHABET_MAX, sizeof(*map_a));

    map_b = (ALPHABET*) malloc(sizeof(*map_b) * ALPHABET_MAX);

    /* Zero indicates "not handled" keycode
     * Also, keywords can not contain '\0'
     * Thus, the alphabet indexing starts from 1
     */
    size_t counter = 1;

    for(size_t key_i = 1; key_i < keys->len; key_i++){
        size_t char_max = strlen(keys->R[key_i]);
        for(size_t char_i = 0; char_i < char_max; char_i++){
            if(map_a[keys->R[key_i][char_i]] == 0){
                map_a[keys->R[key_i][char_i]] = counter;
                map_b[counter++] = keys->R[key_i][char_i];
            }
        }
    }

    for(size_t key_i = 1; key_i < keys->len; key_i++){
        size_t char_max = strlen(keys->R[key_i]);
        for(size_t char_i = 0; char_i < char_max; char_i++)
            keys->R[key_i][char_i] = map_a[keys->R[key_i][char_i]];
    }

    return counter -1;
}

void unmap(const struct key_words * keys){

    for(size_t key_i = 1; key_i < keys->len; key_i++){
        size_t char_max = strlen(keys->R[key_i]);
        for(size_t char_i = 0; char_i < char_max; char_i++)
            keys->R[key_i][char_i] = map_b[keys->R[key_i][char_i]];
    }

}
