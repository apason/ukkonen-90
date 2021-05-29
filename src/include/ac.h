#ifndef AC_H
#define AC_H

#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>


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

/* 
 * Defines the maximum line lenght that can be read from an input file.
 * Lines are read with fgets so the actual maximum number of characters
 * in the line is MAX_LINE -2 (newline and null characters)
 */
#ifndef MAX_LINE
  #define MAX_LINE 2048
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
#ifndef ALPHABET_MAX
  #define ALPHABET_MAX ((size_t) (1 << sizeof(ALPHABET) * 8) -1)
#endif

/* Size of the alphabet table (child function of the implicit trie in bytes */
#define ALPHABET_BYTES (sizeof(STATE) * ALPHABET_MAX)

#ifdef INFO

/* Structure for time-measurement messages that are printed after the execution */
struct output_messages {
    char **messages;
    size_t count;
};

/* This variable is in global scope! */
extern struct output_messages outputs;

extern void addUsageMessage(struct rusage *t1, struct rusage *t2, const char * const msg, struct output_messages *outputs);

#endif

/* Compression value is used from test set 2 */
#ifdef TEST2
extern long cs_compression;
#endif

/*
 * The main structure of the program. This holds all data related to the ac_machine,
 * and its functions as well as all the auxiliary functions used in the calculation
 * of the approximate SCS. All variables are named as they appear in Ukkonen -90.
 */
struct ac_machine {
    STATE (**g)[ALPHABET_MAX];  /* g[0] acts as a failure indicator and should never be evaluated */
    STATE *F;
    STATE B;
    STATE E[STATE_MAX];
    STATE d[STATE_MAX];
    STATE b[STATE_MAX];
    STATE f[STATE_MAX];
    STATE first[STATE_MAX];
    STATE last[STATE_MAX];
    STATE forbidden[STATE_MAX];
    struct queue *supporters_set[STATE_MAX];
    struct queue *P[STATE_MAX];
    struct queue *links[STATE_MAX];
    STATE leaf[STATE_MAX];
    size_t len;
};

/*
 * Struct that holds the key words.
 *
 * At any point of execution there are len pointers allocated
 * in the memory region pointed by R. len is the number of used
 * pointers -1 so R[len] must be allocated but unused. I.e. there
 * are one spare memory slot.
 *
 * R[0] is reserved for error value. Indexing starts from 1. 
 */
struct key_words{
    char **R;
    int *meta;
    int len;
};


/* Structure that describes a half of the edge. */
struct edge {
    STATE next;  // The other side of the edge
    int d;       // Depth of the overlap
};


/* Declarations for functions that are defined in ac.c that are visible to other compilation units */
extern const struct ac_machine * const createMachine(const struct key_words * const keys);
extern struct edge * createPath(struct ac_machine * acm, const struct key_words * const keys);
extern void printCommonSuperstring(const struct ac_machine * const acm, const struct key_words * const keys, struct edge *paths);


#endif
