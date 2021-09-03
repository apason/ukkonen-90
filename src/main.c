#include <stdio.h>   // printf()

#include "init.h"    // STATE, ALPHABET, ALPHABET_MAX
#include "ac.h"      // createMachine(), struct ac_machine, enum input_type
#include "fifo.h"
#include "input.h"   // readInput(), struct key_words
#include "usage.h"   // startTimer(), endTimer()
#include "options.h" // struct options, enum input_type

/* Redundant functions. Only used in development / debugging phase */
/* --------------------------------------------------------------- */

#ifdef DEBUG

static void printDebugInfo   (const struct ac_machine * const acm,  const struct key_words * const keys);
static void printContents    (const        STATE      * const x,    const char * fname);
//static void printGotoFunction(const struct ac_machine * const acm);
static void printKeyWords    (const struct key_words  * keys);

#endif

/* Redundant functions and variables. Only used for extra information */
/* ------------------------------------------------------------------ */


// OMAAN HEADERIIN!!! 
extern void optimizeAlphabet(const struct key_words * keys);
extern void unmap(const struct key_words * keys);



#ifdef INFO
static void printDefs(void);
#endif

/* 
 * Compression is needed only when runnings tests from test set 2 or when INFO is defined.
 * This variable holds the total length of the overlaps of the keywords that are handled.
 */
#if defined(TEST2) || defined(INFO)
size_t cs_compression = 0;
#endif

#ifdef INFO

size_t original_input_length = 0;
size_t reduced_input_length = 0;
#endif

/*
 * Main function.
 *
 * Takes input file as an argument. This file must contain keywords
 * one per line. Empty lines are allowed but the input must not 
 * contain duplicate key words.
 *
 * Outputs the approcimate shortest common superstring.
 */
int main (int argc, char *argv[]){

#ifdef INFO
    startTimer("Total runtime");
#endif

    struct options * opts = getOptions(argc, argv);

    /* Print definitions (macros) of which the binaries are compiled with. */
#ifdef INFO
    printDefs();
#endif

    /* Read the input file and construct an array of all the keywords */
    const struct key_words * keys;

    (void) keys;

    if(opts->type == ONE_PER_LINE)
        keys = readInput(opts->input_file);
    else if (opts->type == SAMPLE_INPUT)
        keys = readSamplesFromFile(opts->input_file, opts->line_length, opts->cut);
    else{
        fprintf(stderr, "ERROR: Input type is not specified\n");
        return EXIT_FAILURE;
    }

    /* Optimize the alphabet */
#ifdef OPTIMIZE_ALPHABET
#ifdef INFO
    startTimer("  Alphabet optimization");
#endif
    optimizeAlphabet(keys);
#ifdef INFO
    endTimer("  Alphabet optimization");
#endif
#endif
    
    freeOptions(opts);

#ifdef DEBUG
    printKeyWords(keys);
#endif

#ifdef SCS    
    
#ifdef INFO
    startTimer("  AC-machine construction");
#endif

    /* Constructs the AC machine as described in Aho & Corasick 1974 */
    const struct ac_machine * const acm = createMachine(keys);

#ifdef INFO
    endTimer("  AC-machine construction");
#endif

    /* Calculates the common superstring of the keywords using the greedy
     * heuristic as described in the publication by Esko Ukkonen 1990 */
#ifdef INFO    
    startTimer("  Path calculation");
#endif
    struct edge *paths = createPath((struct ac_machine *) acm, keys);
#ifdef INFO
    endTimer("  Path calculation");
#endif

#ifdef OPTIMIZE_ALPHABET
    unmap(keys);
#endif
    
#ifdef INFO
    startTimer("  Overlap graph calculation");
#endif
    
    /* Common superstring is not printed if the program is run by test set 2 */
#ifndef TEST2
    printCommonSuperstring(acm, keys, paths);
#endif

#endif /* SCS */    
    
#ifdef INFO

#ifdef SCS    
    endTimer("  Overlap graph calculation");
#endif    

    endTimer("Total runtime");
    printUsageMessages();
#endif    

#ifdef DEBUG
    printDebugInfo(acm, keys);
#endif

#ifdef INFO
    printf("\nLength of the input (reduced set):\t%ld\n", reduced_input_length);
    printf("Length of the input (original input):\t%ld\n", original_input_length);
    printf("Length of the common superstring:\t%ld\n", reduced_input_length - cs_compression);
    printf("Length of the superstring compression:\t");
#endif
    
#if defined(TEST2) || defined(INFO)
    printf("%ld", cs_compression);
#endif
#ifdef INFO
    printf("\nCompression ratio for reduced input:\t%.3lf\n",((double)reduced_input_length - cs_compression)/reduced_input_length);
    printf("Compression ratio for original input:\t%.3lf\n", ((double)reduced_input_length - cs_compression)/original_input_length);    
#endif

    return EXIT_SUCCESS;
}


/* Redundant functions. Only used in development / debugging phase */
/* --------------------------------------------------------------- */

#ifdef DEBUG

/* 
 * This function prints the contents of the AC function x of type STATE x[STATE_MAX]
 * Applicable functions: E, d, b, f, first, last, forbidden, leaf
 */
static void printContents(const STATE * const x, const char *fname){

    for(int i = 0; i < STATE_MAX; i++)
        if(x[i] != 0)
            printf("State %5d  has a %s value of %5d \n", i, fname, x[i]);

    printf("\n");
}

static void printKeyWords(const struct key_words * keys){

    for(int i = 0; i < keys->len; i++)
        printf("Key %10d:\t %s\n", i, keys->R[i]);

    printf("\n");
}

/* Incomplete function. Should print more info (functions E, first, last, forbidden as well) */
static void printDebugInfo(const struct ac_machine * const acm, const struct key_words * const keys){
    for(int i = 1; i <= acm->len; i++)
        printf("state %4d is %10s\n", i, acm->leaf[i] ? "leaf" : "internal");
                            
    printGotoFunction(acm->g, acm->len);

    printContents(acm->f, "f");

    printContents(acm->d, "d");
    printContents(acm->b, "b");

    printf("B: %d\n\n", acm->B);

    printf("F:\n");
    for(int i = 1; i < keys->len; i++){
        printf("Keyword %5d  has a %s value of %5d \n", i, "F", acm->F[i]);
    }
    
    printf("\nE:\n");
    for(int i = 1; i <= acm->len; i++){
        printf("State %5d  has a %s value of %5d \n", i, "E", acm->E[i]);
    }

    printf("\nPrinting supporter lists L(s) for every state s:\n");

    for(int i = 1; i <= acm->len; i++){
        printf("State %5d:", i);
        while(!qEmpty(acm->supporters_set[i]))
            printf("%3d\t", qGet(acm->supporters_set[i]));
        printf("\n");
    }
}

#endif


/* Redundant functions and variables. Only used for extra information */
/* ------------------------------------------------------------------ */

#ifdef INFO

#define xstr(x) str(x)
#define str(x) #x

static void printDefs(void){
    printf("****************************************\n");
    printf("*          Defined values:             *\n");
    printf("****************************************\n");
    printf("INFO: %s\n",
#ifdef INFO
           "defined");
#else
           "not defined");
#endif
    printf("DEBUG: %s\n",
#ifdef DEBUG
           "defined");
#else
           "not defined");
#endif
    printf("MAX_LINE: %s\n", xstr(MAX_LINE));
    printf("STATE: %s\n", xstr(STATE));
    printf("STATE_MAX: %s == %ld\n", xstr(STATE_MAX), STATE_MAX);
    printf("ALPHABET: %s\n", xstr(ALPHABET));
    printf("ALPHABET_MAX: %s == %ld\n", xstr(ALPHABET_MAX), ALPHABET_MAX);
    printf("\n\n");
}

#endif /* INFO */
