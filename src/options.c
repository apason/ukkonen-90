#include <stdio.h>  /* fprintf(), sscanf(), NULL        */
#include <errno.h>  /* errno                            */
#include <string.h> /* strcmp()                         */
#include <stdlib.h> /* malloc(), free()                 */
#include <unistd.h> /* optind                           */

#include "options.h" /* enum input_type, struct options */

static struct options *newOptions(void);
static        int      handleOptions(int argc, char **argv, char *optstring, struct options *opts);

struct options *getOptions(int argc, char *argv[]){
    static char optstring[] = "f:l:c:ab";

    struct options *opts = newOptions();

    if(argc == 1){
        fprintf(stderr, "ERROR: no parameters specified\n");
        return NULL;
    }

    // no flags used. argv[1] is the name of the input file
    if(argc == 2){
	opts->input_file = argv[1];
        opts->type = ONE_PER_LINE;
	return opts;
    }

    if(handleOptions(argc, argv, optstring, opts) == -1){
	freeOptions(opts);
	return NULL;
    }

    if(opts->type == SAMPLE_INPUT && ((opts->line_length < 1) || (opts->cut <= .0))){
        fprintf(stderr, "ERROR: line_length must be a positive integer and cut must be a positive real number\n");
        return NULL;
    }

    if(opts->type == UNDEFINED_INPUT){
        fprintf(stderr, "ERROR: no input type specified\n");
        return NULL;
    }

    if(opts->input_file == NULL){
        fprintf(stderr, "ERROR: no input file specified\n");
        return NULL;
    }

    if(opts->cut > 1.){
        fprintf(stderr, "WARNING: cut is larger than 100%%\n");
    }

    return opts;
}

static int handleOptions(int argc, char **argv, char *optstring, struct options *opts){
    size_t line_length = 0;
    double cut = 1.0;
    int optch   = 0;
  
    //reset optind
    optind = 1;

    while((optch = getopt(argc, argv, optstring)) != -1){

	//file flag
	if(optch == 'f'){
            opts->input_file = optarg;
	}

	// length flag
	if(optch == 'l'){
            if(sscanf(optarg, "%ld", &line_length) != 1){
                fprintf(stderr, "ERROR: line_length must be an integer\n");
                return -1;
            }
            opts->line_length = line_length;
        }
        
	// cut flag
	if(optch == 'c'){
	    if(sscanf(optarg, "%lf", &cut) != 1){
		fprintf(stderr, "ERROR: cut must be a floating point value\n");
		return -1;
	    }
	    opts->cut = cut;
	}

        // type flags
        if(optch == 'a'){
            if(opts->type != UNDEFINED_INPUT){
                fprintf(stderr, "ERROR: only one type flag (-a or -b) can be used at the same time\n");
                return -1;
            }
            opts->type = ONE_PER_LINE;
        }
        if(optch == 'b'){
            if(opts->type != UNDEFINED_INPUT){
                fprintf(stderr, "ERROR: only one type flag (-a or -b) can be used at the same time\n");
                return -1;
            }
            opts->type = SAMPLE_INPUT;
        }
    }
    return 0;
}

/* Create options and initialize to defaults */
static struct options *newOptions(void){
    struct options *opts = (struct options*) malloc(sizeof(*opts));

    checkNULL(opts, "malloc");

    opts->input_file  = NULL;
    opts->type  = UNDEFINED_INPUT;
    opts->line_length = -1;
    opts->cut         = .0;

    return opts;
}

void freeOptions(struct options *opts){
    free(opts);
}
