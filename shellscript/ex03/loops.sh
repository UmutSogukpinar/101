#!/bin/bash

# -----------------------------------------------------------------------------
# Function: reverse_swap
#
# Description:
#   Reverses the given string and swaps the case of each character.
#   - Lowercase letters become uppercase
#   - Uppercase letters become lowercase
#   - Non-alphabetic characters remain unchanged
#
# Parameters:
#   $1 - Input string
#
# Output:
#   Prints the transformed string to stdout
reverse_swap()
{
    local input="$1"
    local result=""
    local char

    for ((i=${#input}-1; i>=0; i--))
    do
        char="${input:i:1}"

        case "$char" in
            [a-z]) 
                result+="${char^^}" 
                ;;
            [A-Z]) 
                result+="${char,,}" 
                ;;
            *) 
                result+="$char" 
                ;;
        esac
    done

    echo "$result"
}

# --- MAIN ---

if [ $# -lt 1 ]; then
    echo "Usage: $0 <args>...<args>"
    exit 1
fi

for arg in "$@"
do
    reverse_swap "$arg"
done
