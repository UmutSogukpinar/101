#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <string>"
    exit 1
fi

input="$1"

letters=0
numbers=0
symbols=0

for (( i = 0; i < ${#input}; i++ ))
do
    char="${input:i:1}"

    # Count letters
    if [[ "$char" =~ [a-zA-Z] ]]; then
        ((letters++))

    # Count numbers
    elif [[ "$char" =~ [0-9] ]]; then
        ((numbers++))

    # Count allowed symbols
    elif [[ "$char" =~ [\*\!\@\#\$\%\^\&\(\)_\+] ]]; then
        ((symbols++))
    fi
done

echo "Numbers: $numbers Symbols: $symbols Letters: $letters"