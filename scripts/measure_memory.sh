#!/bin/bash

# This script measures the memory usage of the program.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo. It is already uniqued dataset. 
datadir="../data/dna/"

gcc -o memusage memusage.c
echo "Measure memory with RB tree goto"

for file in $(ls $datadir) ; do

    rm -f usedmem
    (cd .. ; make clobber)

    MAX_STATES=($(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1))

    (cd .. ; make DEFS="-DRB_TREE_GOTO -DMAX_STATE=$MAX_STATES -DMAX_LINE=10002 -DSCS")

    echo -n "$file:$MAX_STATES:   " >> memory_results

    echo "Current file is $file"

    ./memusage -O ../target/scs "$datadir$file"

    cat usedmem >> memory_results

done
