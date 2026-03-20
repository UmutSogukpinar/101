#!/bin/bash

# --- PARAMETER VALIDATION ---

if [ $# -lt 1 ]; then
    echo "Error: No input file specified."
    echo "Usage: $0 <file.csv>"
    exit 1
fi

INPUT="$1"

if [ ! -f "$INPUT" ]; then
    echo "Error: File '$INPUT' not found."
    exit 1
fi

if [ ! -r "$INPUT" ]; then
    echo "Error: Cannot read '$INPUT'. Check file permissions."
    exit 1
fi

OUTPUT=$(echo "$INPUT" | sed 's/\.[^.]*$//')_new.csv

# Column configuration

NAME_COL=3
LOC_COL=2
EMAIL_LOC=5

echo "Starting process..."
echo "Input:  $INPUT"
echo "Output: $OUTPUT"

# --- PROCESSING ---

awk -F',' -v name_i="$NAME_COL" -v loc_i="$LOC_COL" -v email_i="$EMAIL_LOC" '
BEGIN { OFS="," }

# Mask commas inside double quotes to avoid column shifting
function mask_commas(line,   in_q, masked, i, c) 
{
    in_q = 0
    masked = ""
    for (i=1; i<=length(line); i++) 
    {
        c = substr(line, i, 1)
        if (c == "\"") in_q = !in_q
        if (c == "," && in_q) masked = masked "\034"
        else masked = masked c
    }
    return masked
}

# Unmask the hidden commas back to normal
function unmask_commas(text) 
{
    gsub(/\034/, ",", text)
    return text
}

# Capitalize first letter, lower the rest
function cap(w,   sub_arr, sub_n, j, sub_res) 
{
    sub_n = split(w, sub_arr, "-")
    if (sub_n > 1) 
    {
        sub_res = ""
        for (j=1; j<=sub_n; j++) 
        {
            sub_res = (sub_res == "" ? "" : sub_res "-") \
            toupper(substr(sub_arr[j],1,1)) tolower(substr(sub_arr[j],2))
        }
        return sub_res
    }
    return toupper(substr(w,1,1)) tolower(substr(w,2))
}

# Normalize name: trim + capitalize each word
function format_name(name, arr, n, i, res) 
{
    gsub(/^[ \t]+|[ \t]+$/, "", name)
    n = split(name, arr, " ")
    res = ""
    for (i=1; i<=n; i++) 
    {
        res = (res == "" ? "" : res " ") cap(arr[i])
    }

    return res
}

# Generate base email: first letter + last name
function base_email(name, arr, n)
{
    gsub(/^[ \t]+|[ \t]+$/, "", name)
    n = split(name, arr, " ")
    if (n < 1) return ""
    return tolower(substr(arr[1],1,1) arr[n])
}

# FIRST PASS: count duplicate email roots
NR == FNR {
    if (FNR == 1) next  # skip header
    
    $0 = mask_commas($0)
    e = base_email(unmask_commas($name_i))
    if (e != "") count[e]++
    next
}

# SECOND PASS: generate formatted output
{
    if (FNR == 1) 
    {
        print $0  # print header
        next
    }

    $0 = mask_commas($0)

    original_name = unmask_commas($name_i)
    formatted = format_name(original_name)
    e_root = base_email(original_name)

    if (e_root != "")
    {
        # If duplicate root exists, append location_id
        if (count[e_root] > 1)
            email = e_root $loc_i "@abc.com"
        else
            email = e_root "@abc.com"

        $name_i = formatted
        $email_i = email 
    }

    # Unmask before printing
    for (i=1; i<=NF; i++) 
    {
        $i = unmask_commas($i)
    }

    print $0
}
' "$INPUT" "$INPUT" > "$OUTPUT"

# --- FINAL STATUS CHECK ---

if [ $? -eq 0 ]; then
    echo "Success: File processed successfully."
    echo "Log: $(wc -l < "$OUTPUT") rows written (including header)."
else
    echo "Error: An unexpected error occurred during processing."
    exit 1
fi