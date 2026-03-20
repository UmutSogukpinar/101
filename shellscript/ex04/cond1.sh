#!/bin/bash

IFS=',' read -r -a array <<< "$1"

acc=0

for num in "${array[@]}"
do
    if (( (num & 1) == 0 )); then
        ((acc += num))
    fi
done

# Print result
echo "$acc"
