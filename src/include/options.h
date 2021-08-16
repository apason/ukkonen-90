#ifndef OPTIONS_H
#define OPTIONS_H

#include "init.h"

/* There are currently two methods to feed the input to the program. The simplest method 
 * is one keyword per line. This approach has the drawback that there can not be newlines
 * in the keywords. The other input type SAMPLE_INPUT uses two additional options: 
 * line_length and cut. line_length determines the length of a single keyword (which are
 * all at the same length) and the cut parameter specifies the coverage of the input file.
 * e.g. if the input file is 10Mi long and the cut parameter is set to 0.1f then the 
 * sampled input is 1Mi long. The sampling works by reading line_length characters from
 * the random position of the file until the combined length of the keywords is
 * filesize * cut.
 */

enum input_type { ONE_PER_LINE, SAMPLE_INPUT, UNDEFINED_INPUT };

struct options {
    char *input_file;
    enum input_type type;
    size_t line_length;
    double cut;
};

extern struct options *getOptions (int argc, char *argv[]);
extern        void     freeOptions(struct options *opts);

#endif /* OPTIONS_H */
