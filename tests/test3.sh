#!/bin/bash

#                  $1        $2         $3
# Usage: ./this <binary1> <binary2> <test_folder>

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

for file in $3/* ; do

    echo -n "Testing auto generated instance $(basename $file | cut -d_ -f 2) "
    echo -n "with $(basename $file | cut -d_ -f1) alphabet...  "

    array_hash=$($1 $file | md5sum | cut -d' ' -f1)
    tree_hash=$($2 $file | md5sum | cut -d' ' -f1)


    if [ "$array_hash" != "$tree_hash" ] ; then
        echo -en "${red} FAILURE: ${NC} The generated common superstring "
        echo -e "is not the same. Hash values (array, tree): ($array_hash, $tree_hash)"
    else
        echo -e "${green}PASSED:${NC} All superstrings were same regardless of the goto structure"
    fi
    
done
