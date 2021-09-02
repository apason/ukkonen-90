#!/bin/bash

#NOTE!!! put this file to the root of the compact superstring! Create results/ and data/dna!

datadir="data/dna/"

for file in $(ls $datadir) ; do

    # Clean up
    rm -f "results/$file.sdsl"
    rm -f "results/$file.sorted"
    rm -f "results/$file-index_breakdown.html"
    rm -f "results/$file-create_index.html"
    rm -f "results/$file-find_superstring.html"

    rm -f usedtime

    echo -n "$file:" >> results/dna_results_time
    echo "Current file is $file"

    
    # Build index
    /usr/bin/time -o usedtime -f "%e" tribble/find-superstring/find-superstring -C -f $datadir$file  --source-format=text -i results/$file.sdsl -s results/$file.sorted 2>/dev/null | tr -d '\n' >> results/dna_results_time

    cat usedtime | tr -d '\n' >> results/dna_results_time
    echo -n " + " >> results/dna_results_time

    rm -f usedtime
    
    # Compute superstring
    /usr/bin/time -o usedtime -f "%e" tribble/find-superstring/find-superstring --find-superstring -i results/$file.sdsl -s results/$file.sorted >> results/$file.superstring 2>/dev/null

    cat usedtime | tr -d '\n' >> results/dna_results_time
    echo -n ":" >> results/dna_results_time

    length=$(wc -c results/$file.superstring | cut -f1 -d' ') >> results/dna_results_time
    original=($(wc -c "$datadir$file" | cut -d' ' -f1) - $(wc -l "$datadir$file" | cut -d' ' -f1))

    ratio=$(bc <<< "scale=10; $length/($original)")

    echo "$ratio" >> results/dna_results_time

done
