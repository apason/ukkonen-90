#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>

#include "ac.h"
#include "fifo.h"
#include "input.h"

/* Redundant functions. Only used in development / debugging phase */
/* --------------------------------------------------------------- */

#ifdef DEBUG

static void printDebugInfo   (const struct ac_machine * const acm,  const struct key_words * const keys);
static void printContents    (const        STATE      * const x,    const char * fname);
static void printGotoFunction(const struct ac_machine * const acm);
static void printKeyWords    (const struct key_words  * keys);

#endif

/* Redundant functions and variables. Only used for extra information */
/* ------------------------------------------------------------------ */

#ifdef INFO

static void printUsageMessages(struct output_messages *outputs);
static void printDefs(void);

struct output_messages outputs = {NULL, 0};

#endif

/* 
 * Compression is needed only when runnings tests from test set 2
 * This variable holds the total length of the overlaps of the 
 * keywords that are handled.
 */
#ifdef TEST2
long cs_compression = 0;
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

    /* Print definitions (macros) of which the binaries are compiled with. */
#ifdef INFO
    printDefs();
#endif

    /* Read the input file and construct an array of all the keywords */
    const struct key_words * const keys = readInput(argv[1]);

#ifdef DEBUG
    printKeyWords(keys);
#endif

#ifdef INFO
    struct rusage t1;
    struct rusage t2;
    getrusage(RUSAGE_SELF, &t1);
#endif

    /* Constructs the AC machine as described in Aho & Corasick 1974 */
    const struct ac_machine * const acm = createMachine(keys);
    
#ifdef INFO
    getrusage(RUSAGE_SELF, &t2);
    addUsageMessage(&t1, &t2, "AC-machine construction", &outputs);
    getrusage(RUSAGE_SELF, &t1);
#endif

    /* Calculates the common superstring of the keywords using the greedy
     * heuristic as described in the publication by Esko Ukkonen 1990 */
    struct edge *paths = createPath((struct ac_machine *) acm, keys);
    
#ifdef INFO
    getrusage(RUSAGE_SELF, &t2);
    addUsageMessage(&t1, &t2, "Overlap graph calculation", &outputs);
    getrusage(RUSAGE_SELF, &t1);
#endif

    /* Common superstring is not printed if the program is run by test set 2 */
#ifndef TEST2
    printCommonSuperstring(acm, keys, paths);
#endif

#ifdef INFO
    printf("\n\n");
    getrusage(RUSAGE_SELF, &t2);
    addUsageMessage(&t1, &t2, "Printing the output", &outputs);
    getrusage(RUSAGE_SELF, &t1);

    printUsageMessages(&outputs);
#endif    

#ifdef DEBUG
    printDebugInfo(acm, keys);
#endif

#ifdef TEST2    
    printf("%ld", cs_compression);
#endif

    return 0;
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

static void printGotoFunction(const struct ac_machine * const acm){

    for(int i = 1; i <= acm->len; i++){

        printf("State %d:(from 1 to %ld)\n", i, acm->len);

        for(int j = 0; j < ALPHABET_MAX; j++)
            printf("%d\t", (*acm->g[i])[j]);

        printf("\n\n");
    }
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
                            
    printGotoFunction(acm);

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
    fflush(NULL);
}

void addUsageMessage(struct rusage *t1, struct rusage *t2, const char * const msg, struct output_messages *outputs){

    char *format_string = "%5ld.%06ld\t%5ld.%06ld\t%5ld.%06ld\t%s\n";
    const int format_length = 40;
    
    outputs->messages = realloc(outputs->messages, sizeof(*outputs->messages) * (outputs->count +1));
    outputs->messages[outputs->count] = malloc(sizeof(*outputs->messages) * (format_length + strlen(msg)));
    sprintf(outputs->messages[outputs->count++], format_string,
            t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec,
            t2->ru_utime.tv_usec - t1->ru_utime.tv_usec,
            t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec,
            t2->ru_stime.tv_usec - t1->ru_stime.tv_usec,
            t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec +
            t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec,
            t2->ru_utime.tv_usec - t1->ru_utime.tv_usec +
            t2->ru_stime.tv_usec - t1->ru_stime.tv_usec, msg);
}

static void printUsageMessages(struct output_messages *outputs){
    
    printf("%11s\t%11s\t%11s\t%s\n", "(user)", "(sys)", "(total)", "time spent in");
    printf("--------------------------------------------------------------------------\n");

    for(int i = 0; i < outputs->count; i++)
        printf("%s", outputs->messages[i]);
}

#endif
