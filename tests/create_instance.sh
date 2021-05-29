#!/bin/sh

#                  $1          $2           $3          $4       $5
# usage: ./this <alphabet> <row_length> <row_count> <file_count> <dir> where
# alphabet is one of the following: binary, dna, hex, alnum and the
# other parameters are integers except of the dir which is the target directory

echo "$1 $2 $3 $4 $5"

case $1 in
    "binary")
        alphabet="01" ;;
    
    "dna")
        alphabet="acgt" ;;

    "hex")
        alphabet="0-9A-F" ;;

    "alnum")
        alphabet="a-zA-Z0-9" ;;
    *)
        echo "Invalid alphabet" ;;
esac

for i in $(seq 1 $4); do
    filename="$(mktemp $5/$1_XXXXXXX)"
    for j in $(seq 1 $3); do
        echo "$(tr -dc $alphabet < /dev/urandom | head -c $2)" >> "$filename"
    done

    sort $filename | uniq > tmp
    mv tmp $filename
    
done

