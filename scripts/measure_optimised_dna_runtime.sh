#!/bin/bash

# This script measures the memory usage of the program.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo. Make sure it is already uniqued dataset. 
datadir="../data/dna/"

gcc -o memusage memusage.c
echo "Compare Alanko and Norris implementation"

for file in $(ls $datadir) ; do

    rm -f usedmem
    (cd .. ; make clobber)

    MAX_STATES=($(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1))

    (cd .. ; make DEFS="-DARRAY_GOTO -DMAX_STATE=$MAX_STATES -DSCS -DMAX_ALPHABET=4 -DINFO")

    echo -n "$file:" >> optimized_dna_results

    echo "Current file is $file"

    ./memusage ../target/scs "$datadir$file" > "optimized_dna_$file"

    cat usedmem | tr -d '\n' >> optimized_dna_results
    echo -n "k:" >> optimized_dna_results
    grep "Total runtime" "optimized_dna_$file" | tr '\t' ' ' | tr -s ' ' | cut -d ' ' -f3 | tr -d '\n'  >> optimized_dna_results
    echo -n "sec:" >> optimized_dna_results
    grep "Compression ratio" "optimized_dna_$file" | tr '\t' ' ' | tr -s ' ' | cut -d ' ' -f3  >> optimized_dna_results

done
