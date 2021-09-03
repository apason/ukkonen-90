#!/bin/bash

# This script measures the duplicates found with sample input method.
# Note that the program must be compiled with SCS macro enabled
# Run this file in scripts/ folder

# This data set is not in the repo
datadir="../data/pizzachili/real/"

# X cuts=(50000 100000 200000 500000 1000000 2000000 5000000 10000000 20000000 40000000)

# comment tis out and uncomment X lines for size (not percentage) defined samples
cuts=(0.0025 0.005 0.01 0.02 0.04 0.06 0.12)
line_lengths=(128 256 512 1024 2048 4096 8192 16384)

# Runtime of this script in haapa was 350 minutes.

for ll in ${line_lengths[@]} ; do

        echo "Find duplicates with line length of $ll" >> duplicate_results
        echo "Find duplicates with line length of $ll"

        for size in ${cuts[@]} ; do

            echo "Find duplicates with input size of $size" >> duplicate_results
            echo "Find duplicates with input size of $size"

            for file in $(ls $datadir) ; do


                # X input_size=$(wc -c "$datadir$file" | cut -d' ' -f 1)

                # X cut=$(bc <<< "scale=10; $size/$input_size")

                echo -n "$file:$ll:$size:" >> duplicate_results

                ../target/scs -b -l "$ll" -c "$cut" -f "$datadir$file" > output

                grep "Removed duplicates:" output | tr '\t' ' ' | tr -s ' ' | cut -d':' -f 2 | tr -d '\n' | tr -d ' ' >> duplicate_results
                echo -n ":" >> duplicate_results
                grep "Compression ratio for reduced" output | tr '\t' ' ' | tr -s ' ' | cut -d':' -f 2 | tr -d '\n' | tr -d ' ' >> duplicate_results
                echo -n ":" >> duplicate_results                
                grep "Compression ratio for original" output | tr '\t' ' ' | tr -s ' ' | cut -d':' -f 2 | tr -d ' ' >> duplicate_results
            done

        done
done


## Use something like this to get the values in csv format
#!/bin/bash
#count=1
#for line in $(cat duplicate_results | grep cere | sort -t ':' -k3 -n -s | cut -d':' -f 4 ) ; do
#        if [ $(($count % 8)) -eq 0 ] ; then
#                echo "$line" ;
#        else  echo -n "$line," ;
#        fi ;
#        ((count++))
#done
