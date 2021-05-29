#!/bin/bash

#                    $1             $2
# Usage: ./this <test_folder> <scs_binary>

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

for file in $1/* ; do
    scs=$($2 $file)
    failure_count=0

    echo -n "Testing auto generated instance $(basename $file | cut -d_ -f 2) "
    echo -n "with $(basename $file | cut -d_ -f1) alphabet...  "
    
    while read line ; do
        if ! echo "$scs" | grep -q "$line" ; then
            ((failure_count++))
            echo -en "${red} FAILURE: ${NC} The generated common superstring "
            echo -e  "does not contain a keyword $line."
        fi
        
    done < $file

    if [[ $failure_count -eq 0 ]] ; then
        echo -e "${green}PASSED:${NC} All keywords were found in the common superstring."
    else
        echo -e "file $(basename $file | cut -d_ -f 2) ${red} FAILED ${NC} the tests."
    fi
    
done
