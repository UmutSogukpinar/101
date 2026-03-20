# Test Output to JSON Converter

## Overview

This script converts a structured test output (similar to BATS-style logs) into a clean JSON format.

It parses:

* Test suite name
* Individual test results (status, message, duration)
* Summary information (success, failed, rating, total duration)

## Features

* Bash + awk based parsing
* JSON formatting using `jq`
* Handles messages with quotes safely
* Extracts structured data from plain text logs

## Requirements

* Bash
* awk



## Usage

```bash
./script.sh <input_file>
```

Example:

```bash
./script.sh output.txt
```

Output will be written to:

```bash
output.json
```

## Input Format

Expected input format:

```text
[ Asserts Samples ], 1..2 tests
-----------------------------------------------------------------------------------
not ok  1  expecting command finishes successfully (bash way), 7ms
ok  2  expecting command prints some message (the same as above, bats way), 10ms
-----------------------------------------------------------------------------------
1 (of 2) tests passed, 1 tests failed, rated as 50%, spent 17ms
```

## Output Format

Example JSON output:

```json
{
  "name": "Asserts Samples",
  "tests": [
    {
      "status": false,
      "message": "expecting command finishes successfully (bash way)",
      "duration": "7ms"
    },
    {
      "status": true,
      "message": "expecting command prints some message (the same as above, bats way)",
      "duration": "10ms"
    }
  ],
  "summary": {
    "success": 1,
    "failed": 1,
    "rating": 50,
    "duration": "17ms"
  }
}
```

## How It Works

1. First line is parsed to extract test suite name.
2. Lines starting with `ok` or `not ok` are parsed as individual tests.
3. Summary line is parsed using regex patterns.
4. awk builds raw JSON.
5. jq formats the final JSON output.

## Notes

* Input format must remain consistent.
* Script assumes messages end with a duration value like `7ms`.
* Quotes inside messages are escaped automatically.

## Possible Improvements

* Support stdin input
* Add CLI flags (e.g. `-o output.json`)
* Improve error handling
* Support different test output formats
