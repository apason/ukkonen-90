#!/bin/bash

# This script measures the memory usage of the program.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo. Make sure it is already uniqued dataset. 
datadir="../data/real_15_128/"

gcc -o memusage memusage.c
echo "Measuring runtime and memory for the program"

gotos=("ARRAY" "RB_TREE")
links=("-DDUMMY" "-DOPTIMIZE_LINKS")


for goto in ${gotos[@]} ; do
    
    for link in ${links[@]} ; do

        if [ "$link" = "-DDUMMY" ]
        then
            optimized="non-optimized"
        else

            optimized="optimized"
        fi
           
        result_dir="mem-$goto-$optimized-results/"

        echo "Running measures for $goto goto with $optimized link"
        
        mkdir "$result_dir"
        
        for file in $(ls $datadir) ; do

            rm -f usedmem
            (cd .. ; make clobber)

            (cd .. ; make DEFS="-D$goto""_GOTO -DSCS $link")

            echo -n "$file:" >> "$result_dir"results

            echo "Current file is $file"

            ./memusage ../target/scs "$datadir$file"  > "$result_dir$file.superstring"

            # NOTE THAT THE MEASURED MEMORY IS IN KILOBYTES (or kibibytes?)
            cat usedmem | tr -d '\n' >> "$result_dir"results
            echo -n ":" >> "$result_dir"results

            length=$(wc -c "$result_dir$file.superstring" | cut -f1 -d' ')
            original=($(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1))
            ratio=$(bc <<< "scale=10; $length/($original)")
            echo "$ratio" >> "$result_dir"results

        done
    done
done
