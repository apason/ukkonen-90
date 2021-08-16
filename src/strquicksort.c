#include <stdlib.h> // malloc(), realloc()
#include <string.h> // strlen()

#include "input.h"  // struct key_words, MAX_LINE

struct key_words * NewArray(int size){
    struct key_words *A = malloc(sizeof(*A));
    A->R = (char **) malloc(size * sizeof(char*));
    A->len = size;

    return A;
}

void add(struct key_words *R, char *S){
    R->R = (char **) realloc (R->R, (R->len + 1) * sizeof (char*));
    R->R[R->len] = S;
    R->len = R->len + 1;
}

static struct key_words * Combine(struct key_words * A1, struct key_words * A2, struct key_words * A3, struct key_words * A4){

    struct key_words *R = NewArray(0);

    for(int i = 0; i < A1->len; i++)
	add(R, A1->R[i]);

    for(int i = 0; i < A2->len; i++)
	add(R, A2->R[i]);

    for(int i = 0; i < A3->len; i++)
	add(R, A3->R[i]);

    for(int i = 0; i < A4->len; i++)
	add(R, A4->R[i]);

    free(A1->R);
    free(A2->R);
    free(A3->R);
    free(A4->R);

    return R;
}

struct key_words * StringQuickSort(struct key_words *R, int l){

    if(R->len <= 1)
	return R;

    struct key_words * R_null = NewArray(0);
    struct key_words * R_less = NewArray(0);
    struct key_words * R_equ  = NewArray(0);
    struct key_words * R_gre  = NewArray(0);
    
    char *pivot = R->R[R->len/2];
    
    for(int i = 0; i < R->len; i++){
	char *S = R->R[i];

	if(strlen(S) == l){
	    add(R_null, S);
	    continue;
	}
	

	if(S[l] < pivot[l]){
	    add(R_less, S);
	    continue;
	}

	if(S[l] == pivot[l]){
	    add(R_equ, S);
	    continue;
	}

	if(S[l] > pivot[l]){
	    add(R_gre, S);
	    continue;
	}
    }

    free(R->R);

    return Combine(R_null,
		   StringQuickSort(R_less, l),
		   StringQuickSort(R_equ,  l+1),
		   StringQuickSort(R_gre,  l));
}
