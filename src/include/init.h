#ifndef INIT_H
#define INIT_H

#include <stdio.h>  // perror(), NULL
#include <stdlib.h> // exit(), EXIT_FAILURE
#include <stdint.h> // uintXX_t types
#include <stddef.h> // limits of types

/* Check that the system has all integral data types of size {8, 16, 32, 64} bits */
#ifndef UINT64_MAX
  #error "Type uint64_t not implemented"
#endif
#ifndef UINT32_MAX
  #error "Type uint32_t not implemented"
#endif
#ifndef UINT16_MAX
  #error "Type uint16_t not implemented"
#endif
#ifndef UINT8_MAX
  #error "Type uint8_t not implemented"
#endif

/* Specifies the data type used as an alphabet */
#ifndef ALPHABET
  #define ALPHABET char
#endif

/* Default value for MAX_STATE is 16bits */
#ifndef MAX_STATE
    #define MAX_STATE UINT16_MAX -1
#endif

/*
 * Set STATE datatype so it is the smallest possible that 
 * can address every state in the range 0 ... MAX_STATE
 */
#if MAX_STATE <= 0
  #error "MAX_STATE must be positive"
#elif MAX_STATE > UINT64_MAX -1
  #error "MAX_STATE is too large (Not supported)"
#elif MAX_STATE > UINT32_MAX -1
  #define STATE uint64_t
#elif MAX_STATE > UINT16_MAX -1
  #define STATE uint32_t
#elif MAX_STATE > UINT8_MAX  -1
  #define STATE uint16_t
#else
  #define STATE uint8_t
#endif

/* This is the macro actually used in the code */
#define STATE_MAX (size_t)MAX_STATE


/* Maximum value of the alphabet index. Executed in loop and might be slow!" */
#ifndef MAX_ALPHABET
  #define ALPHABET_MAX ((size_t) (1 << sizeof(ALPHABET) * 8) -2)
#else
  #define ALPHABET_MAX ((size_t) MAX_ALPHABET)
  #define OPTIMIZE_ALPHABET
#endif

/* Size of the alphabet table (child function of the implicit trie in bytes */
#define ALPHABET_BYTES (sizeof(STATE) * ALPHABET_MAX)

#include "goto.h"

/* Helper macro to check if critical return value from malloc(), realloc() of fopen() is NULL */
#define checkNULL(X, Y) do {                      \
                            if((X) == NULL){      \
                              perror((Y));        \
                              exit(EXIT_FAILURE); \
                            }                     \
                        }while(0);


#endif /* INIT_H */
