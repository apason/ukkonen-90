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
    STATE *F; // entry for every key. all values are different. not possible to optimize
    STATE B;
    STATE E[STATE_MAX]; // Only leaf nodes has nonzero value. Could be optimized using a tree.
    STATE d[STATE_MAX]; // Depth of the state. All states has nonzero value.
    STATE b[STATE_MAX]; // reverse ordering od the bfs. Not possible to optimize.
    STATE f[STATE_MAX]; // failure link. Many zero values? might be optimized with a tree?
    STATE first[STATE_MAX]; // only |key set| elements!
    STATE last[STATE_MAX]; // only |key set| elements!
    STATE forbidden[STATE_MAX]; // binary variable. Why the type is STATE? basically all states are first zero, then 1. Tree structure does not help? 
    struct queue *supporters_set[STATE_MAX]; // Set of indices of the keywords whose goto path goes through this node. For low depth nodes this is quite a large set. For high depth this is very thin.
    struct queue *P[STATE_MAX]; // list of key word indicies for which state is on a failure path that starts from F(i) AND hampath to be created does not yet contain an overlap (xi, xj) for any j.
    linksQ **links; // set of states that has ingoing goto transition from this node. Can not exeed ALPHABET_MAX. 
    STATE leaf[STATE_MAX]; // binary variable. Why is this type of STATE? (does not matter in single byte alphabets though)
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
