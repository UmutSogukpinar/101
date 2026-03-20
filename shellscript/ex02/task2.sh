#!/bin/bash

# --- ARGUMENT VALIDATION ---

# Check whether the user provided an input file.
if [ $# -lt 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

# Store input file path from the first argument.
INPUT_FILE=$1

# Define fixed output file name.
OUTPUT_FILE="output.json"

# Log start of processing.
echo "[LOG] Starting JSON conversion..."
echo "[LOG] Input file: $INPUT_FILE"
echo "[LOG] Output file: $OUTPUT_FILE"

awk '

BEGIN {
    # Start JSON object.
    print "{"

    first_test = 1
}

# First Line
FNR == 1 {

    split($0, parts, ",")
    name = parts[1]

    gsub(/\[|\]/, "", name)
    gsub(/^[ \t]+|[ \t]+$/, "", name)

    printf "\"testName\":\"%s\" ,\"tests\":[", name
}

/^(ok|not ok)/ {

    if (!first_test)
    {
        printf ","
    }

    first_test = 0

    status = ($1 == "ok") ? "true" : "false"

    duration = $NF

    start_idx = ($1 == "not") ? 4 : 3
    
    msg = ""
    for (i=start_idx; i<NF; i++) msg = msg $i " "
    sub(/, $/, "", msg)
    sub(/[ \t]+$/, "", msg)
    gsub(/"/, "\\\"", msg)

    printf "{\"name\":\"%s\",\"status\":%s,\"duration\":\"%s\"}", msg, status, duration
}

/tests passed/ {

    printf "],\"summary\":{"

    success = 0; failed = 0; rating = 0; duration_sum = "0ms"

    if (match($0, /[0-9]+ \(of [0-9]+\)/))
    {
        temp = substr($0, RSTART, RLENGTH)
        split(temp, arr, " ")
        success = arr[1]
    }

    if (match($0, /[0-9]+ tests failed/))
    {
        temp = substr($0, RSTART, RLENGTH)
        split(temp, arr, " ")
        failed = arr[1]
    }

    if (match($0, /rated as [0-9]+(\.[0-9]+)?%/))
    {
        temp = substr($0, RSTART, RLENGTH)
        split(temp, arr, " ")
        rating = arr[3]
        sub(/%/, "", rating)
    }

    if (match($0, /spent [0-9]+ms/))
    {
        temp = substr($0, RSTART, RLENGTH)
        split(temp, arr, " ")
        duration_sum = arr[2]
    }

    printf "\"success\":%d,\"failed\":%d,\"rating\":%s,\"duration\":\"%s\"}", success, failed, rating, duration_sum
}

END {
    print "}"
}

' "$INPUT_FILE" | ./jq '.' > "$OUTPUT_FILE"

# Log completion.
if [ $? -eq 0 ]; then
    echo "[LOG] JSON conversion completed successfully."
    echo "[LOG] Result written to: $OUTPUT_FILE"
else
    echo "[LOG] JSON conversion failed."
fi
