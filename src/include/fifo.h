#ifndef FIFO_H
#define FIFO_H

#include "init.h" // STATE ALPHABET_MAX

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


struct alphabet_queue{
    STATE first;
    STATE last;
    STATE data[];
};

extern int   qAEmpty(const struct alphabet_queue * const q         );
extern STATE qAGet  (      struct alphabet_queue * const q         );
extern int    qAPut (      struct alphabet_queue * const q, STATE s);

extern struct alphabet_queue * newAlphabetQueue(void);

#ifdef OPTIMIZE_LINKS
typedef struct alphabet_queue linksQ;
#else
typedef struct queue linksQ;
#endif

extern int      (*linksQPut)        (linksQ * const, STATE);
extern int      (*linksQEmpty)(const linksQ * const);
extern STATE    (*linksQGet)        (linksQ * const);
extern linksQ * (*linksNewQueue)    (void);

struct data_set{
    STATE iterator_r;
    STATE iterator_w;
    STATE capacity;
    STATE data[];
};

extern struct data_set * newDataSet(size_t capacity);
extern int sPut(struct data_set ** s, STATE state);
extern int sEmpty(const struct data_set * const s);
extern STATE sGet(struct data_set * const s);

#endif /* FIFO_H */
