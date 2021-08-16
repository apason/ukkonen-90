#ifdef INFO

#include <stdio.h>        // NULL
#include <stdlib.h>       // malloc()
#include <string.h>       // strlen(), strcmp()

#include <sys/resource.h> // getrusage(), struct rusage
#include <sys/time.h>     // struct timeval

#include "init.h"         // checkNULL macro

struct timer{
    char *message;
    struct rusage *t0;
    struct timer *next;
};

/* Structure for time-measurement messages that are printed after the execution */
struct output_messages {
    char **messages;
    size_t count;
};

static struct timer * timers = NULL; 
static struct output_messages outputs = {NULL, 0};

void addUsageMessage(struct rusage *t1, struct rusage *t2, const char * const msg){

    char *format_string = "%5ld.%06ld\t%5ld.%06ld\t%5ld.%06ld\t%s\n";
    const int format_length = 40;
    
    outputs.messages = realloc(outputs.messages, sizeof(*outputs.messages) * (outputs.count +1));
    outputs.messages[outputs.count] = malloc(sizeof(*outputs.messages) * (format_length + strlen(msg)));

    size_t ru_utime_sec = t2->ru_utime.tv_usec > t1->ru_utime.tv_usec || t2->ru_utime.tv_usec == t1->ru_utime.tv_usec ?
        t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec :
        (t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec) -1;
    size_t ru_utime_usec = t2->ru_utime.tv_usec - t1->ru_utime.tv_usec > 0 || t2->ru_utime.tv_usec == t1->ru_utime.tv_usec ?
        t2->ru_utime.tv_usec - t1->ru_utime.tv_usec :
        1000000 - (t1->ru_utime.tv_usec - t2->ru_utime.tv_usec);
    size_t ru_stime_sec = t2->ru_stime.tv_usec > t1->ru_stime.tv_usec || t2->ru_stime.tv_usec == t1->ru_stime.tv_usec ?
        t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec :
        (t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec) -1;
    size_t ru_stime_usec = t2->ru_stime.tv_usec - t1->ru_stime.tv_usec > 0 || t2->ru_stime.tv_usec == t1->ru_stime.tv_usec ?
        t2->ru_stime.tv_usec - t1->ru_stime.tv_usec :
        1000000 - (t1->ru_stime.tv_usec - t2->ru_stime.tv_usec);
    size_t ru_ttime_usec = ru_utime_usec + ru_stime_usec < 1000000 ?
        ru_utime_usec + ru_stime_usec :
        1000000 - (ru_utime_usec - ru_stime_usec);
    size_t ru_ttime_sec = ru_utime_usec + ru_stime_usec < 1000000 ?
        ru_utime_sec + ru_stime_sec :
        ru_utime_sec + ru_stime_sec +1;

        sprintf(outputs.messages[outputs.count++], format_string,
                ru_utime_sec,  ru_utime_usec,
                ru_stime_sec,  ru_stime_usec,
                ru_ttime_sec,  ru_ttime_usec, msg);
    
    /* sprintf(outputs.messages[outputs.count++], format_string, */
    /*         t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec, */
    /*         t2->ru_utime.tv_usec - t1->ru_utime.tv_usec, */
    /*         t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec, */
    /*         t2->ru_stime.tv_usec - t1->ru_stime.tv_usec, */
    /*         t2->ru_utime.tv_sec  - t1->ru_utime.tv_sec + */
    /*         t2->ru_stime.tv_sec  - t1->ru_stime.tv_sec, */
    /*         t2->ru_utime.tv_usec - t1->ru_utime.tv_usec + */
    /*         t2->ru_stime.tv_usec - t1->ru_stime.tv_usec, msg); */

}

void printUsageMessages(){

    printf("\n\n%11s\t%11s\t%11s\t%s\n", "(user)", "(sys)", "(total)", "time spent in");
    printf("--------------------------------------------------------------------------\n");

    for(int i = outputs.count -1; i >= 0; i--)
        printf("%s", outputs.messages[i]);
}

/* Assume no multiple calls with same message */
void startTimer(char *message){

    struct rusage * t0 = malloc(sizeof(*t0));
    checkNULL(t0, "malloc");

    struct timer * new = malloc(sizeof(*new));
    checkNULL(new, "malloc");

    new->t0 = t0;
    new->next = NULL;
    new->message = message;

    getrusage(RUSAGE_SELF, new->t0);

    new->next = timers;

    timers = new;
}

void endTimer(char *message){
    struct timer *find;

    struct rusage t1;

    getrusage(RUSAGE_SELF, &t1);

    for(find = timers; find != NULL; find = find->next)
        if(strcmp(find->message, message) == 0)
            break;

    if(find == NULL){
        fprintf(stderr, "ERROR: usage message \"%s\" not initialized\n!", message);
        return;
    }

    addUsageMessage(find->t0, &t1, message);
}

#endif /* INFO */
