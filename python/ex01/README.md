# Snapshot

Snapshot is a simple Python command-line tool for monitoring system state.

It collects system snapshots using `psutil` and writes output to:

- console
- JSON file

Each snapshot is written to the output file as a separate JSON line.

## Features

- Configurable interval between snapshots
- Configurable output file name
- Configurable number of snapshots
- Console output with screen clear
- JSON Lines output format
- Output file is cleaned on startup

## Requirements

- Python 3
- pip
- psutil

## Installation

Install the package from the project directory:

```bash
pip install -U ./snapshot