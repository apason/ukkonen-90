#ifndef AC_H
#define AC_H

#include "init.h"    // STATE, STATE_MAX, ALPHABET
#include "goto.h"    // goto_function
#include "fifo.h"    // struct queue
#include "input.h"   // struct key_words

/* Compression value is used from test set 2. This variable is defined in main.c */
#if defined(TEST2) || defined(INFO)
extern size_t cs_compression;
#endif /* TEST2 || INFO */

/*
 * The main structure of the program. This holds all data related to the ac_machine,
 * and its functions as well as all the auxiliary functions used in the calculation
 * of the approximate SCS. All variables are named as they appear in Ukkonen -90.
 */
struct ac_machine {
    goto_function g;
    STATE *F;
    STATE B;
    STATE *E;
    STATE *d;
    STATE *b;
    STATE f[STATE_MAX];
    STATE *first;
    STATE *last;
    STATE *forbidden;
    struct queue *supporters_set[STATE_MAX];
    struct queue *P[STATE_MAX];
#ifdef LINKSQ_ARRAY
    linksQ *links;
#else
    linksQ **links;
#endif
    STATE leaf[STATE_MAX];
    size_t len;
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

extern STATE gotoGet (goto_function g, STATE  state, ALPHABET key);
extern  void gotoSet (goto_function g, STATE  state, ALPHABET key, STATE value);
extern  void gotoInit(goto_function g, size_t len);

extern  void printGotoFunction(goto_function g, size_t len);
#endif /* AC_H */
