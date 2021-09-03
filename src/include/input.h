#ifndef INPUT_H
#define INPUT_H

#include <stddef.h> /* size_t */

/* 
 * Defines the maximum line lenght that can be read from an input file.
 * Lines are read with fgets so the actual maximum number of characters
 * in the line is MAX_LINE -2 (newline and null characters)
 */
#ifndef MAX_LINE
  #define MAX_LINE 2048
#endif

/*
 * Struct that holds the key words.
 *
 * At any point of execution there are len pointers allocated
 * in the memory region pointed by R. len is the number of used
 * pointers -1 so R[len] must be allocated
 *
 * R[0] is reserved for error value. Indexing starts from 1. 
 */
struct key_words{
    char **R;
    int *meta;
    int len;
};

/* This variable is defined in main.c */
#ifdef INFO
extern size_t original_input_length;
extern size_t reduced_input_length;
#endif

extern const struct key_words * const readInput(const char * const file_name);
extern const struct key_words * const readSamplesFromFile(const char * const file_name, size_t row_length, float cut);

#endif /* INPUT_H */
