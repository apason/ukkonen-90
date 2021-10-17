#ifndef FIFO_H
#define FIFO_H

#include "init.h" // STATE ALPHABET_MAX

struct s_a_pair{
    STATE s;
    ALPHABET c;
};

/* We want the single node to be small enough to be allocated from the heap but
 * big enough to be efficient. With 32 bit STATE and 16Ki NODE_CAPACITY the 
 * size of a single node is 64Ki + 16 bytes which is less than M_MMAP_THRESHOLD  */
#define NODE_CAPACITY (16 * 1024)

struct fifo_node{
    struct fifo_node * next;
    int first;
    int last;
    STATE data[NODE_CAPACITY];
};
    

struct fifo{
    size_t size;
    struct fifo_node * first;
    struct fifo_node * last;
};

extern int fEmpty(const struct fifo * const f);
extern STATE fGet(struct fifo * const f);
extern STATE fGet2(struct fifo * const f);
extern void fPut(struct fifo * const f, STATE s);
extern void freeFifo(struct fifo * const f);

extern struct fifo * newFifo(void);
extern struct fifo_node * newFifoNode(void);

/* The fifo node that is used only internally in queue */
struct queue_node{
    STATE state;
    ALPHABET c;
    struct queue_node *next;
};

/* The fifo handler that is used outside of the fifo.c  */
struct queue{
    int size;
    struct queue_node *first;
    struct queue_node *iterator;
    struct queue_node *last;
};

/* The following functions are called outside of the fifo.c */
extern int   qEmpty   (const struct queue * const q                     );
extern STATE qGet     (      struct queue * const q                     );
extern STATE qGet2    (      struct queue * const q                     );
extern int   qPut     (      struct queue * const q, STATE s, ALPHABET c);
extern void  freeQueue(      struct queue * const q                     );

extern struct s_a_pair qRead(struct queue * const q                     );

extern struct queue * newQueue (void);


struct alphabet_queue{
    STATE first;
    STATE iterator;
    STATE last;

    struct s_a_pair data[];
};

extern int   qAEmpty(const struct alphabet_queue * const q                     );
extern STATE qAGet  (      struct alphabet_queue * const q                     );
extern int   qAPut  (      struct alphabet_queue * const q, STATE s, ALPHABET c);

extern struct s_a_pair qARead(struct alphabet_queue * const q                  );
extern struct alphabet_queue * newAlphabetQueue(void);

#ifdef OPTIMIZE_LINKS
typedef struct alphabet_queue linksQ;
#else
typedef struct queue linksQ;
#endif

extern int      (*linksQPut)        (linksQ * const, STATE, ALPHABET c);
extern int      (*linksQEmpty)(const linksQ * const);
extern STATE    (*linksQGet)        (linksQ * const);
extern linksQ * (*linksNewQueue)    (void);

extern struct s_a_pair (*linksQRead)       (linksQ * const);

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
