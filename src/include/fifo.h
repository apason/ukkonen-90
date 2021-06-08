#ifndef FIFO_H
#define FIFO_H

#include "init.h" // STATE

/* The fifo node that is used only internally in queue */
struct queue_node{
    STATE state;
    struct queue_node *next;
};

/* The fifo handler that is used outside of the fifo.c  */
struct queue{
    int size;
    struct queue_node *first;
    struct queue_node *last;
};

/* The following functions are called outside of the fifo.c */
extern int   qEmpty   (const struct queue * const q         );
extern STATE qGet     (      struct queue * const q         );
extern STATE qGet2    (      struct queue * const q         );
extern int   qPut     (      struct queue * const q, STATE s);
extern void  freeQueue(      struct queue * const q         );

extern struct queue * newQueue (void);

#endif /* FIFO_H */
