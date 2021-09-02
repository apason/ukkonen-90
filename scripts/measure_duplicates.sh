#!/bin/bash

# This script measures the duplicates found with sample input method.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo
datadir="../data/pizzachili/real/"

cuts=(50000 100000 200000 500000 1000000 2000000 5000000 10000000 20000000 40000000)
line_lengths=(128 256 512 1024 2048 4096 8192 16384)

for ll in ${line_lengths[@]} ; do

        echo "Find duplicates with line length of $ll" >> duplicate_results
        for size in ${cuts[@]} ; do

                echo "Find duplicates with input size of $size" >> duplicate_results

                for file in $(ls $datadir) ; do

                        input_size=$(wc -c "$datadir$file" | cut -d' ' -f 1)

                        cut=$(bc <<< "scale=10; $size/$input_size")

                        echo -n "$file:$ll:$size:$cut:  " >> duplicate_results

                        ../target/scs -b -l "$ll" -c "$cut" -f "$datadir$file" > output

                        grep "Removed duplicates:" output | tr '\t' ' ' | tr -s ' ' | cut -d' ' -f 3 | tr -d '\n' >> duplicate_results
                        grep "Compression ratio:" output | tr '\t' ' ' | tr -s ' ' | cut -d' ' -f 3 >> duplicate_results
                done
                echo ""
        done
done
