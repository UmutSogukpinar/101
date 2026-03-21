#!/bin/bash

# Calculate base^exp using integer arithmetic
# Args:
#   $1: base (integer)
#   $2: exponent (integer >= 0)
# Output:
#   Prints the result of base^exp
pow()
{
    local base=$1
    local exp=$2
    local result=1

    for ((i=0; i<exp; i++))
    do
        ((result *= base))
    done

    echo "$result"
}

# Return the shortest string(s) among given arguments
# Args:
#   $@: list of strings
# Output:
#   Prints all shortest strings (in input order, first occurrence first)
#   If multiple strings have the same minimum length, all are printed
#   separated by spaces
shortest()
{
    (( $# == 0 )) && return 0

    local shortest="$1"
    local result=("$1")
    local arg

    shift

    for arg in "$@"
    do
        if (( ${#arg} < ${#shortest} )); then
            shortest="$arg"
            result=("$arg")
        elif (( ${#arg} == ${#shortest} )); then
            result+=("$arg")
        fi
    done

    printf "%s\n" "${result[@]}"
}


# Print a log message with current timestamp
# Args:
#   $1: message string
# Output:
#   Prints message prefixed with [YYYY-MM-DD HH:MM]
print_log()
{
    local msg=$1
    local today="[$(date "+%Y-%m-%d %H:%M")]"

    echo "$today $msg"
}
