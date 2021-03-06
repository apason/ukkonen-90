#include <stdio.h>  // [f]printf(), NULL
#include <stdlib.h> // [m,re]alloc()
#include <string.h> // strlen()
#include <tgmath.h>

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

static void initAuxiliaryFunctions(struct ac_machine * const acm, const struct key_words * const keys);

static size_t estimateStates(long double m, long double l, long double s);
static void estimateLeaves(size_t sigma, size_t m, size_t len);


// number of states estimated for random input
size_t estimated_states;

// estimated_leaves[depth] is the average number of leaves for that depth.
static STATE *estimated_leaves;

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

        if(acm->links[state] == NULL)
            acm->links[state] = linksNewQueue();

        linksQPut(acm->links[state], acm->len, key[p]);

        state = acm->len;
    }
    acm->leaf[state] = 1;
}

struct queue * bfs_q;

/* Failure function construction as described in Aho & Corasick 1975: algorithm 3 */
static void failureFunction(struct ac_machine * const acm){

    bfs_q = newQueue();
    qPut(bfs_q, 1, 0);
    
    ALPHABET c;
    STATE r, s, t;

    for(c = 1; c <= real_alphabet_size; c++){

        if(gotoGet(acm->g, 1, c) == 1)
            continue;

        s = gotoGet(acm->g, 1, c);

        qPut(bfs_q, s, 0);
        acm->f[s] = 1;
    }

    (void) qRead(bfs_q);

    while((r = qRead(bfs_q).s) != 0){

        /* Leaves does not have links array */
        if(acm->leaf[r])
            continue;
        
        for(struct s_a_pair sa = linksQRead(acm->links[r]); sa.s != 0; sa = linksQRead(acm->links[r])){

            qPut(bfs_q, sa.s, 0);

            t = acm->f[r];

            while(gotoGet(acm->g, t, sa.c) == 0)
                t = acm->f[t];
            
            acm->f[sa.s] = gotoGet(acm->g, t, sa.c);
        }
    }

    bfs_q->iterator = bfs_q->first;
    (void)qRead(bfs_q);
}

/* Preprocessing algorithm that calculates the auxiliary data as described in Ukkonen 1990: Algorithm 1 */
static void auxiliaryFunctions(struct ac_machine * const acm, const struct key_words * const keys){
    STATE s;
    
    acm->d[1] = 0;
    acm->B = 1;

#ifdef INFO
    startTimer("      Calculating depth and rBFS");
#endif

    STATE r;
    while(!qEmpty(bfs_q)){

        r = qGet(bfs_q);

        for(int x = acm->leaf[r] ? 0 : acm->links[r]->size; x > 0; x--){
            STATE s = qRead(bfs_q).s;
            acm->d[s] = acm->d[r] +1;
            acm->b[s] = acm->B;
            acm->B = s;

            /* Filter out other proper substrings */
            acm->F[acm->E[acm->f[s]]] = 1;
        }
        // free(acm->links[r]); // takes a very long time. Why it takes longer time here (individually) ?
    }

    freeQueue(bfs_q);

#ifdef INFO
    endTimer("      Calculating depth and rBFS");
    startTimer("      Calculating L, F and E");
#endif
    
    for(STATE s = 1; s <= acm->len; s++)
        acm->supporters_set[s] = newDataSet(estimated_leaves[acm->d[s]]);

    for(int i = 1; i < keys->len; i++){

        int k = strlen(keys->R[i]);
        ALPHABET *x = keys->R[i];
        s = 1;

        for(int j = 0; j < k; j++){

            s = gotoGet(acm->g, s, x[j]);

            sPut(&acm->supporters_set[s], i); 

            if(j == k -1){

                acm->F[i] = s;
                acm->E[s] = i;

                /* Filter out proper prefixes */
                if(!acm->leaf[s]){
                    acm->F[i] = 1;
                }
            }
        }
    }
    
#ifdef INFO
    endTimer("      Calculating L, F and E");    
    startTimer("      Freeing memory");
#endif

    // g still leaks memory if the whole tree is not deallocated (when rb alternative is used)
    // If links and g are initialized to 0, we can loop through estimated_states instead of real len.
    // This may be faster somehow.. ? 
    for(int i = 0; i < estimated_states; i++){
        free(acm->links[i]);
        free(acm->g[i]);
    }
    
    free(acm->links); // leaks memory if every individually allocated linksQ object has not be freed
    free(acm->g); // leaks memory if every individually allocated alphabet table is not freed.
    free(acm->E);
    free(acm->leaf);

#ifdef INFO
    endTimer("      Freeing memory");
#endif

}

/* Path calculation algorithm as described in Ukkonen 1990: Algorithm 2 */
struct edge * createPath(struct ac_machine * acm, const struct key_words * const keys){

    struct edge * list = calloc(keys->len, sizeof(struct edge));

    checkNULL(list, "calloc - createPath, list:");

    for(int j = 1; j < keys->len; j++){

        if(acm->F[j] != 1){
            qPut(acm->P[acm->f[acm->F[j]]], j, 0);
            acm->first[j] = j;
            acm->last[j] = j;
        }
        else
            acm->forbidden[j] = 1;
    }

    free(acm->F);

    STATE s = acm->b[acm->B];

    STATE (*get_fn)(struct queue * const q);

    while(s != 1){
        if(!qEmpty(acm->P[s])){
            
            struct data_set * ds = acm->supporters_set[s];

            while(!sEmpty(ds)){
                int i;
                int j = sGet(ds);

                if(acm->forbidden[j] == 1)
                    continue;

                if(qEmpty(acm->P[s])){
                    // should break instead!!!
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

            /* Linked list. why not just copy one link? Do we still neet P[s] ? */
            for(struct queue_node *n = acm->P[s]->first; n != NULL; n = n->next)
                qPut(acm->P[acm->f[s]], n->state, 0);
        }
        s = acm->b[s];
    }

#ifdef INFO
    startTimer("      Freeing last memory");
#endif
    
    free(acm->d);
    free(acm->b);
    free(acm->f);

    for(size_t ind = 1; ind <= acm->len; ind++)
        free(acm->supporters_set[ind]);
    
    free(acm->supporters_set); // leaks memory
    free(acm->first);
    free(acm->last);

#ifdef INFO
    endTimer("      Freeing last memory");
#endif
    
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

    initAuxiliaryFunctions(acm, keys);
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
            qPut(q, i, 0);
    }

    /* How long is this queue? Should we use another fifo? */
    while(!qEmpty(q)){
        int i = qGet(q);
        printPath(acm, keys, i, paths, 1);
    }

    free(acm->forbidden);
    
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

    if(acm->len == estimated_states){
        fprintf(stderr, "ERROR: Too many states :(\n");
        exit(EXIT_FAILURE);
    }

    acm->len++;
    
    gotoInit(acm->g, acm->len);
}

#define xstr(x) str(x)
#define str(x) #x

/* Allocates sizeof(TYPE) * LEN memory and stores the pointer to acm->X     */
/* The allocated memory region is filled with zeros with CALLOCATE.         */
/* Note that these macros assume that variable acm is available when called */
#define ALLOCATE(X, LEN) do{                        \
        acm->X = malloc(sizeof(*acm->X) * (LEN));   \
        checkNULL(acm->X, "malloc - ALLOCATE");     \
    } while (0);
#define CALLOCATE(X, LEN) do{                       \
        acm->X = calloc((LEN), sizeof(*acm->X));    \
        checkNULL(acm->X, "calloc - CALLOCATE");    \
    } while (0);


void initAdditionalfunctions(struct ac_machine * const acm, size_t k){
    ALLOCATE(first, k);
    ALLOCATE(last, k);
    CALLOCATE(forbidden, k);
    ALLOCATE(P, acm->len +1);    
    
    /* Many calls to malloc due to queue initializations */
    for(int i = 0; i <= acm->len; i++)
        acm->P[i] = newQueue();
}

static void initAuxiliaryFunctions(struct ac_machine * const acm, const struct key_words * const keys){

    ALLOCATE(F, keys->len);

    acm->B = 0;

    CALLOCATE(E, acm->len +1);
    ALLOCATE(d, acm->len +1);
    ALLOCATE(b, acm->len +1);
    ALLOCATE(supporters_set, acm->len +1);

    estimateLeaves(real_alphabet_size, keys->len -1, strlen(keys->R[1]));

}
/* Initializes (allocates memory) and sets the default values  of the ac machine */
/* Estimate the number of states and initialize memory for that estimate number. */
static struct ac_machine * initMachine(const struct key_words * const keys){
    
    struct ac_machine * acm = malloc(sizeof(*acm));
    checkNULL(acm, "malloc - initMachine, acm");

    estimated_states = estimateStates(keys->len -1, strlen(keys->R[1]), real_alphabet_size);

    CALLOCATE(g, estimated_states);
    // Why is f allocated for estimated_states. We already know the actual number of states when
    // f is first used. Links, leaf and g are however used in the goto calculation.
    CALLOCATE(f, estimated_states);
    CALLOCATE(links, estimated_states);
    CALLOCATE(leaf, estimated_states);

    acm->len = 0;

    return acm;
}

/* Calculates the estimated number of leaves for subtrees of each depth */
void estimateLeaves(size_t sigma, size_t m, size_t len){

    estimated_leaves = malloc(sizeof(*estimated_leaves) * (len +2));
    checkNULL(estimated_leaves, "malloc - estimateLeaves");

    long double result;
    
    for(int i = 0; i <= len; i++){
        result = (long double)m/powl(sigma, i);
        result++;
        estimated_leaves[i] = result;
    }
    estimated_leaves[len+1] = 0;
}

size_t estimateStates(long double m, long double l, long double s){

    long double pr = 0;
    long double add = 0;
    for(int i = 0; i <= l; i++){
        add = powl(s, i) * (1- powl((powl(s, i) -1)/powl(s, i), m));
        if(add == 0)
            add = m;
        if(isnan(add))
           add = m;
        pr = pr + add;
    }

    // For very short inputs the estimated size might be smaller so we add +500 to it.
    return (size_t) pr +500;
}
