#include <stdio.h>  // NULL
#include <stdlib.h> // malloc(), free()
#include <string.h> // memset()

#include "init.h"   // STATE ALPHABET_MAX
#include "fifo.h"   // struct queue[_node]

/* 
 * This file contains an implementation of a very simple fifo functionality and structure.
 * The functions newQueue() and freeQueue() acts as a constructor and destructor like functions
 * handling the memory management. In addition there are functions for inserting an element
 * to the (end of the) fifo and getting (and removing) the element from its head.
 *
 * All operations performs in constant time. However, since each node of the queue is (de)allocated
 * separately, there may be a significant overhead from continuous calls to malloc() and free().
 * An efficient approach may be implemented later using array and (de)allocating bigger chuncks
 * of memory at a time.
 */


/* 
 * Returns a new empty fifo
 */
struct queue * newQueue(void){

    struct queue * q = malloc(sizeof(*q));

    checkNULL(q, "malloc");

    q->size = 0;
    q->first = NULL;
    q->last = NULL; 

    return q;
}

/*
 * Removes and returns the first element of the fifo
 */
STATE qGet(struct queue * const q){

    if(q->size == 0)
        return 0;

    struct queue_node *first = q->first;
    
    STATE s = first->state;
    q->first = first->next;
    q->size--;

    free(first);

    return s;
}

/*
 * Removes and returns the second element of the fifo
 */
STATE qGet2(struct queue * const q){

    if(q->size < 2)
        return 0;

    struct queue_node *second = q->first->next;

    STATE s = second->state;
    q->first->next = second->next;
    q->size--;

    free(second);

    return s;
}

/*
 * Inserts an element to the end of the fifo
 */
int qPut(struct queue * const q, STATE s){

    struct queue_node *n = malloc(sizeof(*n));

    checkNULL(n, "malloc");

    n->state = s;
    n->next = NULL;

    if(q->size == 0)
        q->first = n;
    else
        q->last->next = n;
    
    q->last = n;
    q->size++;

    return 0;
}

/*
 * Returns 1 if the given fifo is empty, 0 otherwise
 */
int qEmpty(const struct queue * const q){
 
   if(q->size < 1)
        return 1;

    return 0;
}

/*
 * Destructs the fifo and all its nodes
 */
void freeQueue(struct queue * const q){

    for(struct queue_node *n = q->first; n != NULL;){
        
        struct queue_node *m = n->next;
        free(n);
        n = m;
    }

    free(q);
}


/*
 * Removes and returns the first element of the fifo
 */
STATE qAGet(struct alphabet_queue * const q){

    if(q->first == q->last)
        return 0;

    return q->data[q->first++];
}

/*
 * Returns 1 if the given fifo is empty, 0 otherwise
 */
int qAEmpty(const struct alphabet_queue * const q){

    if(q == NULL)
        return 1;

    if(q->first == q->last)
        return 1;

    return 0;
}

/*
 * Inserts an element to the end of the fifo.
 */
int qAPut(struct alphabet_queue * const q, STATE s){

    q->data[q->last++] = s;

    return 0;
}

/* 
 * Returns a new empty alphabet_queue
 */
struct alphabet_queue * newAlphabetQueue(void){

    struct alphabet_queue * q = malloc(sizeof(*q));

    checkNULL(q, "malloc");

    q->first = 0;
    q->last = 0;
    memset(q->data, 0, sizeof(q->data));

    return q;
}
