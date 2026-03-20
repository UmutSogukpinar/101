#!/bin/bash

convert_units()
{
    local unit=$1
    local value=$2
    local result=0

    if [ "$unit" = "C" ]; then
        ((result = value + 273))
        echo "${result}K"
    else
        ((result = value - 273))
        echo "${result}C"
    fi
}

# ------------ Main ------------

if [ $# -lt 1 ]; then
    echo "Usage: $0 <arg>"
    exit 1
fi

input=$1

if [[ "$input" =~ ^([0-9]+)(C|K)$ ]]; then

    value="${BASH_REMATCH[1]}"
    unit="${BASH_REMATCH[2]}"

    convert_units "$unit" "$value"
else
    echo "Invalid input"
    exit 1
fi