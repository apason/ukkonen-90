#!/bin/bash

# This script measures the memory usage of the program.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo. Make sure it is already uniqued dataset. 
datadir="../data/dna/"

gcc -o memusage memusage.c
echo "Measuring runtime and memory for the program"

gotos=("ARRAY" "RB_TREE")
alphabets=("-DDUMMY" "-DMAX_ALPHABET=4")


for goto in ${gotos[@]} ; do
    
    for alphabet in ${alphabets[@]} ; do

        if [ "$alphabet" = "-DDUMMY" ]
        then
            optimized="non-optimized"
        else

            optimized="optimized"
        fi
           
        result_dir="$goto-$optimized-results/"

        echo "Running measures for $goto goto with $optimized alphabet"
        
        mkdir "$result_dir"
        
        for file in $(ls $datadir) ; do

            rm -f usedmem
            (cd .. ; make clobber)

            MAX_STATES="$(bc <<< "$(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1)")"

            (cd .. ; make DEFS="-D$goto""_GOTO -DMAX_STATE=$MAX_STATES -DSCS $alphabet")

            echo -n "$file:" >> "$result_dir"results

            echo "Current file is $file"

            # Do we want to measure the memory parallel with the time measurements? Does this slow down the program
            #./memusage ../target/scs "$datadir$file" > "$result_dir$file"
            /usr/bin/time -o usedtime -f "%e" ../target/scs "$datadir$file" > "$result_dir$file.superstring"
            
            #cat usedmem | tr -d '\n' >> "$result_dir"results
            #echo -n "k:" >> "$result_dir"results
            #grep "Total runtime" "$result_dir$file" | tr '\t' ' ' | tr -s ' ' | cut -d ' ' -f4 | tr -d '\n'  >> "$result_dir"results
            #echo -n "sec:" >> "$result_dir"results
            #grep "Compression ratio" "$result_dir$file" | tr '\t' ' ' | tr -s ' ' | cut -d ' ' -f3  >> "$result_dir"results
            cat usedtime | tr -d '\n' >> "$result_dir"results
            echo -n ":" >> "$result_dir"results

            length=$(wc -c "$result_dir$file.superstring" | cut -f1 -d' ')
            original=($(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1))
            ratio=$(bc <<< "scale=10; $length/($original)")
            echo "$ratio" >> "$result_dir"results

        done
    done
done
