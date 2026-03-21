#!/bin/bash

# validate argument count
if (( $# != 1 )); then
    echo "Usage: $0 <count>"
    exit 1
fi

count=$1
letters=({a..z})

# create folders
for ((i=0; i<count; i++))
do
    letter="${letters[i]}"
    mkdir -p "folder_$letter"
done