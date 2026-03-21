#!/bin/bash

# Check argument
if [ $# -lt 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

INPUT_FILE=$1
OUTPUT_FILE="${INPUT_FILE}_new"

# Simple logger
log()
{
    echo "[$(date "+%Y-%m-%d %H:%M:%S")] $1"
}

log "Script started"
log "Input file: $INPUT_FILE"

# Process passwd file
log "Updating shells (saned, avahi, even UID)"

awk -F: '
BEGIN { OFS=":" }

$1 == "saned" { $7 = "/bin/bash" }
$1 == "avahi" { $7 = "/bin/bash" }
$3 % 2 == 0   { $7 = "/bin/zsh" }

{ print }
' "$INPUT_FILE" > "$OUTPUT_FILE"

# Remove daemon users
log "Removing lines containing daemon"
sed -i '/daemon/d' "$OUTPUT_FILE"

# Keep selected columns & remove trailing newline
log "Filtering columns (1,3,5,7) and fixing newline"

awk -F: '
BEGIN { OFS=":" } 
{ 
    if (NR > 1) printf "\n";
    printf "%s:%s:%s:%s", $1, $3, $5, $7 
}
' "$OUTPUT_FILE" > tmp && mv tmp "$OUTPUT_FILE"

log "Processing finished"
log "Output file: $OUTPUT_FILE"

echo "Done!"