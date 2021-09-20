/* Included from init.h */

#ifndef GOTO_H  
#define GOTO_H

#include "rb_tree.h"

/* The goto function can be implemented in several ways. The easiest and the fastest would be direct indexing with arrays.   */
/* Since this will waste a lot of memory (there will be ALPHABET_MAX * sizeof(STATE) bytes for each state in the AC machine) */
/* this approach can only be used with small alphabets and limited inputs. Other data structures for goto function would be  */
/* for example many binary search tree variants and hash tables.                                                             */

/* This file describes the "interface" for those data structures and their associated functions which are defined in this    */
/* directory. For each data structure, there is a soruce file containing the needed functions. The used data type is         */
/* specified at the compilation phase by defining one and only one of the related macros.                                    */

/*
 * The functions that must be defined:
 *   gotoSet(struct ac_machine *, STATE, ALPHABET, STATE)
 *   gotoGet(struct ac_machine *, STATE, ALPHABET)
 *   void gotoInit(goto_function g, size_t len)
 *   void printGotoFunction(goto_function g, size_t len)
 */

/*
 * The supported macros are: 
 *   ARRAY_GOTO
 *   RB_TREE_GOTO 
 */


#if defined(ARRAY_GOTO) || defined(RB_TREE_GOTO)
#else
  #error "Goto Function data structure not defined!"
#endif


#ifdef ARRAY_GOTO
typedef STATE (**goto_function)[];
#endif

#ifdef RB_TREE_GOTO
typedef struct rb_tree ** goto_function;
#endif

#endif
