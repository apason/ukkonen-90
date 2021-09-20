#include <stdio.h>  // [f]printf(), NULL
#include <stdlib.h> // [m,re]alloc()
#include <string.h> // memset()

#include "init.h"   // STATE, ALPHABET, ALPHABET_MAX
#include "ac.h"     // struct ac_machine, struct edge
#include "fifo.h"   // newQueue(), qGet(), qPut(), qEmpty(), struct queue[_node]
#include "input.h"  // struct key_words
#include "usage.h"  // startTimer(), endTimer()

/* Static functions. Only used within this compilation unit. */
static void printPath    (const struct ac_machine * const acm, const struct key_words * const keys, int i, struct edge *paths, int first);
static void auxiliaryFunctions (struct ac_machine * const acm, const struct key_words * const keys);
static void gotoFunction       (struct ac_machine * const acm, const struct key_words * const keys);
static void handleKey          (struct ac_machine * const acm, char *key);
static void failureFunction    (struct ac_machine * const acm);
static void createState        (struct ac_machine * const acm);

static struct ac_machine * initMachine(const struct key_words * const keys);

static void initAuxiliaryFunctions(struct ac_machine * const acm, size_t k);

/* Goto function construction as described in Aho & Corasic 1975: Algorithm 2 */
static void gotoFunction(struct ac_machine * const acm, const struct key_words * const keys){

    createState(acm);

    for(int i = 1; i < keys->len; i++)
        handleKey(acm, keys->R[i]);

    for(int i = 1; i <= real_alphabet_size; i++){
        if(gotoGet(acm->g, 1, i) == 0)
            gotoSet(acm->g, 1, i, 1);
    }
}

/* As described in Aho & Corasick 1975: algorithm 2 procedure enter */
static void handleKey(struct ac_machine * const acm, char *key){

    STATE state = 1;
    int j = 0;

    int len = strlen(key);

    while(gotoGet(acm->g, state, key[j]) != 0)
        state = gotoGet(acm->g, state, key[j++]);

    acm->leaf[state] = 0;

    for(int p = j; p < len; p++){
        
        createState(acm);

        gotoSet(acm->g, state, key[p], acm->len);

#ifndef LINKSQ_ARRAY
        if(acm->links[state] == NULL)
            acm->links[state] = linksNewQueue();
#endif

#ifdef LINKSQ_ARRAY        
        linksQPut(&acm->links[state], acm->len);
#else
        linksQPut(acm->links[state], acm->len);
#endif
        state = acm->len;
    }
    acm->leaf[state] = 1;
}

/* Failure function construction as described in Aho & Corasick 1975: algorithm 3 */
static void failureFunction(struct ac_machine * const acm){

    struct queue * const queue = newQueue();
    ALPHABET c;
    STATE r, s, t;

    for(c = 1; c <= real_alphabet_size; c++){

        if(gotoGet(acm->g, 1, c) == 1)
            continue;

        s = gotoGet(acm->g, 1, c);

        qPut(queue, s);
        acm->f[s] = 1;
    }

    while(!qEmpty(queue)){
        r = qGet(queue);

        for(c = 1; c <= real_alphabet_size; c++){

            if(gotoGet(acm->g, r, c) == 0)
                continue;

            s = gotoGet(acm->g, r, c);

            qPut(queue, s);
            t = acm->f[r];

            while(gotoGet(acm->g, t, c) == 0)
                t = acm->f[t];
            
            acm->f[s] = gotoGet(acm->g, t, c);
        }
    }
    freeQueue(queue);
}

/* Preprocessing algorithm that calculates the auxiliary data as described in Ukkonen 1990: Algorithm 1 */
static void auxiliaryFunctions(struct ac_machine * const acm, const struct key_words * const keys){
    STATE s;
    
    for(int i = 1; i < keys->len; i++){

        int k = strlen(keys->R[i]);
        ALPHABET *x = keys->R[i];
        s = 1;

        for(int j = 0; j < k; j++){

            s = gotoGet(acm->g, s, x[j]);

            qPut(acm->supporters_set[s], i); 

            if(j == k -1){

                acm->F[i] = s;
                acm->E[s] = i;

                /* Filter out proper prefixes */
                if(!acm->leaf[s]){
                    acm->F[i] = 1;
#ifdef DEBUG
                    printf("State %4d (\"%s\") represents an internal node. Setting F(%d) to 1.\n", s, keys->R[acm->F[acm->E[s]]], s);
#endif
                }
            }
        }
    }
    
    struct queue * q = newQueue();
    qPut(q, 1);
    acm->d[1] = 0;
    acm->B = 1;

    STATE r;
    while(!qEmpty(q)){

        r = qGet(q);

#ifdef LINKSQ_ARRAY
        while(!linksQEmpty(&acm->links[r])){
            STATE s = linksQGet(&acm->links[r]);
#else
        while(!linksQEmpty(acm->links[r])){
            STATE s = linksQGet(acm->links[r]);
#endif
            qPut(q, s);
            acm->d[s] = acm->d[r] +1;
            acm->b[s] = acm->B;
            acm->B = s;

#ifdef DEBUG
            if(acm->E[acm->f[s]] != 0)
                printf("Proper substring \"%s\" detected. Removing corresponding keyword %d\n", keys->R[acm->E[acm->f[s]]] , acm->E[acm->f[s]]);
#endif
            /* Filter out other proper substrings */
            acm->F[acm->E[acm->f[s]]] = 1;
        }
        
#ifndef LINKSQ_ARRAY
        // The queue SHOULD be empty which means we can just free the header
        // free(acm->links[r]);
        
        // Do we want to free these empty queues at all, its is very slow..
#endif
        
                                 
    }
#ifdef LINKSQ_ARRAY
        free(acm->links);
#else
        // Do we want to free these empty queues (just queue headers) at all?
#endif
}

/* Path calculation algorithm as described in Ukkonen 1990: Algorithm 2 */
struct edge * createPath(struct ac_machine * acm, const struct key_words * const keys){

    struct edge * list = malloc(sizeof(struct edge) * keys->len);

    checkNULL(list, "malloc");

    memset(list, 0, sizeof(struct edge) * keys->len);

    for(int j = 1; j < keys->len; j++){

        if(acm->F[j] != 1){
            qPut(acm->P[acm->f[acm->F[j]]], j);
            acm->first[j] = j;
            acm->last[j] = j;
        }
        else
            acm->forbidden[j] = 1;
    }

    STATE s = acm->b[acm->B];

    STATE (*get_fn)(struct queue * const q);

    while(s != 1){
        if(!qEmpty(acm->P[s])){

            for(struct queue_node *n = acm->supporters_set[s]->first; n != NULL; n = n->next){
                int i;
                int j = n->state;

                if(acm->forbidden[j] == 1)
                    continue;

                if(qEmpty(acm->P[s])){
#ifdef DEBUG
                    printf("Queue was empty, which means |P(s)| < |L(s)| for some state s. Continuing..\n");
#endif                    
                    continue;
                }

                i = acm->P[s]->first->state;
                get_fn = qGet;

                /* Detect cycle */
                if(acm->first[i] == j){
                    if(acm->P[s]->size == 1)
                        continue;
                    else{
                        i = acm->P[s]->first->next->state;
                        get_fn = qGet2;
                    }
                }
                
#ifdef DEBUG
                printf("merge: %s - %s\t depth: %d\n", keys->R[i], keys->R[j], acm->d[s]);
#endif

#if defined(TEST2) || defined(INFO)
                cs_compression += acm->d[s];
#endif
                list[i].next = j;
                list[i].d = acm->d[s];

                acm->forbidden[j] = 1;
                (void) get_fn(acm->P[s]);

                acm->first[acm->last[j]] = acm->first[i];
                acm->last[acm->first[i]] = acm->last[j];
            }

            for(struct queue_node *n = acm->P[s]->first; n != NULL; n = n->next)
                qPut(acm->P[acm->f[s]], n->state);
        }
        s = acm->b[s];
    }
    return list;
}

/* Interface function. Creates the AC machine and all auxiliary data */
const struct ac_machine * const createMachine(const struct key_words * const keys){

#ifdef INFO
    startTimer("    Machine initialization");
#endif

    struct ac_machine *acm = initMachine(keys);

#ifdef INFO
    endTimer("    Machine initialization");
    startTimer("    Goto function calculation");
#endif

    gotoFunction(acm, keys);

#ifdef INFO
    endTimer("    Goto function calculation");
    startTimer("    Failure function calculation");
#endif
    
    failureFunction(acm);

#ifdef INFO
    endTimer("    Failure function calculation");
    startTimer("    Calculation of auxiliary functions");
#endif

    initAuxiliaryFunctions(acm, keys->len);
    auxiliaryFunctions(acm, keys);

#ifdef INFO    
    endTimer("    Calculation of auxiliary functions");
#endif
    
    return acm;
}

/* Prints the common superstring to stdout. */
void printCommonSuperstring(const struct ac_machine * const acm, const struct key_words * const keys, struct edge *paths){

#ifdef INFO
    printf("Approcimate SCS:\n");
#endif

    struct queue *q = newQueue();

    for(int i = 1; i < keys->len; i++){
        if(acm->forbidden[i] == 0)
            qPut(q, i);
    }

    while(!qEmpty(q)){
        int i = qGet(q);
        printPath(acm, keys, i, paths, 1);
    }

#ifdef INFO
    printf("\n");
#endif
}

/* Prints a single path. (path that contains a key words with overlaps). There may occur a several such paths in the common superstring */
static void printPath(const struct ac_machine * const acm, const struct key_words * const keys, int i, struct edge *paths, int first){
    
    if(first)
        printf("%s", keys->R[i]);

    for(int k = paths[i].next, d = paths[i].d; k != 0; k = paths[k].next){
        printf("%s", (keys->R[k]) + d);
        d = paths[k].d;
    }
}

/* Creates and initializes a new state to the AC machine acm */
static void createState(struct ac_machine * const acm){

    if(acm->len == STATE_MAX){
        fprintf(stderr, "ERROR: Too many states. Try to increase MAX_STATE and recompile\n");
        exit(EXIT_FAILURE);
    }

    acm->g = realloc(acm->g, sizeof(*acm->g) * (++acm->len +1));
    checkNULL(acm->g, "realloc");

    gotoInit(acm->g, acm->len);
}

/* Allocates sizeof(TYPE) * LEN memory and stores the pointer to acm->X    */
/* The allocated memory region is filled with zeros.                       */
/* Note that this macro assumes that variable acm is available when called */
#define ALLOCATE(X, TYPE, LEN) do{             \
        acm->X = malloc(sizeof(TYPE) * LEN);   \
        checkNULL(acm->X, "malloc");           \
        memset(acm->X, 0, sizeof(TYPE) * LEN); \
    } while (0);
 
static void initAuxiliaryFunctions(struct ac_machine * const acm, size_t k){

    ALLOCATE(F, STATE, k);

    acm->B = 0;

    ALLOCATE(E, STATE, MAX_STATE);
    ALLOCATE(d, STATE, MAX_STATE);
    ALLOCATE(b, STATE, MAX_STATE);
    ALLOCATE(first, STATE, k);
    ALLOCATE(last, STATE, k);
    ALLOCATE(forbidden, uint8_t, k);

    
    /* Many calls to malloc due to queue initializations */
    for(int i = 0; i < STATE_MAX; i++){
        acm->supporters_set[i] = newQueue();
        acm->P[i] = newQueue();
    }

}
/* Initializes (allocates memory) and sets the default values  of the ac machine */
static struct ac_machine * initMachine(const struct key_words * const keys){
    
    struct ac_machine * acm = malloc(sizeof(*acm));
    checkNULL(acm, "malloc");

    /* If we had better approximation of the maximum number of states */
    /* we would allocate all needed memory here instead of setting g */
    /* to null and reallocating iteratively in createState() */
    acm->g = NULL;

    // Tarvitaanko tätä ifdefiä? Onko sama asia?
#ifdef LINKSQ_ARRAY
    // size of linksQ does not include the flexible array member!
    acm->links = malloc((sizeof(acm->links) + _Generic(acm->links, struct alphabet_queue *: real_alphabet_size,
                                                                  default: 0
                                                   )) * STATE_MAX);
    checkNULL(acm->links, "malloc");
    /* Here we assume that NULL is 0 */
    memset(acm->links, 0, sizeof(acm->links) * STATE_MAX);
#else
    acm->links = malloc(sizeof(acm->links) * STATE_MAX);
    checkNULL(acm->links, "malloc");
    /* Here we assume that NULL is 0 */
    memset(acm->links, 0, sizeof(acm->links) * STATE_MAX);
#endif
    
    memset(acm->f, 0, sizeof(acm->f));

    memset(acm->leaf, 0, sizeof(acm->leaf));

    acm->len = 0;

    return acm;
}
 
