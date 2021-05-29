#!/bin/bash

#                    $1             $2
# Usage: ./this <test_folder> <scs_binary>

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

for file in $1/* ; do
    compression=$($2 $file)

    echo -n "Testing auto generated instance $(basename $file | cut -d_ -f 2)... "

    # Assume no additional whitespace in the file
    input_size=$(( $(wc -c $file | cut -d' ' -f1) - $(wc -l $file | cut -d' ' -f1)  ))
    opt_len=$(python scs.py $file -n)
    opt_compression=$(expr $input_size - $opt_len)

    limit=$(($opt_compression%2?$opt_compression/2+1:$opt_compression/2))

    if [ $compression -lt $limit ] ; then
        echo -e "${red} FAILURE: ${NC} The compression $compression is less than $limit (1/2 of the optimal compression $opt_compression)"
    elif [ $compression -gt $opt_compression ] ; then
         echo -e "${red}FAILURE: ${NC} The compression $compression is better than optimal compression $opt_compression. Impossible!"
    else
        echo -e "${green}PASSED:${NC} The compression $compression is between [$limit,  $opt_compression] = [(1/2 of the optimal compression), (optimal compression)]"
    fi

done
