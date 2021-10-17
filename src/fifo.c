#include <stdio.h>  // NULL
#include <stdlib.h> // malloc(), free()

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

#ifdef OPTIMIZE_LINKS
int      (*linksQEmpty)(const linksQ * const)                    = qAEmpty;
STATE    (*linksQGet)        (linksQ * const)                    = qAGet;
int      (*linksQPut)        (linksQ * const, STATE, ALPHABET c) = qAPut;
linksQ * (*linksNewQueue)    (void)                              = newAlphabetQueue;
struct s_a_pair(*linksQRead) (linksQ * const)                    = qARead;

#else

int      (*linksQEmpty)(const linksQ * const)                    = qEmpty;
STATE    (*linksQGet)        (linksQ * const)                    = qGet;
int      (*linksQPut)        (linksQ * const, STATE, ALPHABET c) = qPut;
linksQ * (*linksNewQueue)    (void)                              = newQueue;
struct s_a_pair(*linksQRead) (linksQ * const)                    = qRead;

#endif

int fEmpty(const struct fifo * const f){

    if(f->size < 1)
        return 1;

    return 0;    
}

/* If the fifo is empty, the behaviour is undefined */
STATE fGet(struct fifo * const f){

    struct fifo_node * first = f->first;
    STATE s = first->data[first->first++];

    if(first->first == NODE_CAPACITY){
        f->first = first->next;
        free(first);
    }

    f->size--;

    return s;
}

/* If the fifo does not have at least two values,
 * the behaviour is undefined
 */
STATE fGet2(struct fifo * const f){

    struct fifo_node * node;
    int offset;
    
    /* The second value is in the next node */
    if(f->first->first == NODE_CAPACITY -1){
        node = f->first->next;
        offset = 0;
    }
    else{
        node = f->first;
        offset = 1;
    }

    STATE s = node->data[node->first +offset];

    node->data[node->first +offset] = node->data[node->first];

    node->first++;

    f->size--;

    return s;
}

void fPut(struct fifo * const f, STATE s){

    struct fifo_node * node;
    
    if(f->first == NULL){
        f->first = newFifoNode();
        f->last = f->first;
    }

    if(f->last->last == NODE_CAPACITY){
        node = newFifoNode();
        f->last->next = node;
        f->last = node;
    }

    node = f->last;
    
    node->data[node->last++] = s;

    f->size++;
}

void freeFifo(struct fifo * const f){

    struct fifo_node * node;

    while(f->first != NULL){
        node = f->first->next;
        free(f->first);
        f->first = node;
    }
    
    free(f);
}

struct fifo * newFifo(void){

    struct fifo * f = calloc(1, sizeof(*f));

    checkNULL(f, "cmalloc - newFifo()");

    return f;
}

struct fifo_node * newFifoNode(void){

    struct fifo_node * node = malloc(sizeof(*node));

    checkNULL(node, "malloc - newFifoNode()");

    node->first = 0;
    node->last = 0;
    node->next = NULL;

    return node;
}

/* 
 * Returns a new empty fifo
 */
struct queue * newQueue(void){

    struct queue * q = calloc(1, sizeof(*q));

    checkNULL(q, "calloc - newQueue()");

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
 * Reads the next item and increases the iterator.
 * Returns zero if the iterator is at the end.
 */
struct s_a_pair qRead(struct queue * const q){

    struct queue_node *iterator = q->iterator;

    if(iterator == NULL)
        return (struct s_a_pair) {0,0};
    
    STATE s = iterator->state;
    ALPHABET c = iterator->c;
    q->iterator = iterator->next;

    return (struct s_a_pair) {s, c};
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
int qPut(struct queue * const q, STATE s, ALPHABET c){

    struct queue_node *n = malloc(sizeof(*n));

    checkNULL(n, "malloc");

    n->state = s;
    n->next = NULL;

    if(q->size == 0){
        q->first = n;
        q->iterator = n;
    }
    else
        q->last->next = n;

    n->c = c;
    
    q->last = n;
    q->size++;

    return 0;
}

/*
 * Returns 1 if the given fifo is empty, 0 otherwise
 */
int qEmpty(const struct queue * const q){

    if(q == NULL)
        return 1;
    
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

    return q->data[q->first++].s;
}

/*
 * Reads the next item and increases the iterator.
 * Returns zero if the iterator is at the end.
 */
struct s_a_pair qARead(struct alphabet_queue * const q){
    

    if(q->iterator == q->last)
        return (struct s_a_pair) {0, 0};

    return q->data[q->iterator++];
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
int qAPut(struct alphabet_queue * const q, STATE s, ALPHABET c){

    q->data[q->last].s = s;
    q->data[q->last++].c = c;
    return 0;
}

/* 
 * Returns a new empty alphabet_queue
 */
struct alphabet_queue * newAlphabetQueue(void){

    struct alphabet_queue * q = calloc(1, (sizeof(*q) + sizeof(*q->data) * real_alphabet_size));

    checkNULL(q, "calloc - newAlphabetQueue");

    return q;
}

STATE sGet(struct data_set * const s){

    if(s->iterator_r == s->iterator_w)
        return 0;

    return s->data[s->iterator_r++];
}

int sEmpty(const struct data_set * const s){

    if(s == NULL)
        return 1;

    if(s->iterator_r == s->iterator_w)
        return 1;

    return 0;
}


int sPut(struct data_set ** s, STATE state){

    if((*s)->iterator_w == (*s)->capacity){
        *s = realloc(*s, sizeof(**s) + sizeof(*(*s)->data) * ((*s)->capacity +1));
        checkNULL(*s, "realloc");
        (*s)->capacity++;
    }

    (*s)->data[(*s)->iterator_w++] = state;

    return 0;
}

struct data_set * newDataSet(size_t capacity){

    struct data_set * s = calloc(1, (sizeof(*s) + sizeof(*s->data) * capacity));
    checkNULL(s, "calloc - newDataSet");

    s->capacity = capacity;

    return s;
}
