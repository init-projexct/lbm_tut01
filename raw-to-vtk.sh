#!/bin/bash

# Check if a filename was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <filename.raw>"
    exit 1
fi

# Configuration
INPUT_FILE="$1"
# Get the directory and base name to ensure output stays with input
DIR_NAME=$(dirname "$INPUT_FILE")
BASE_NAME=$(basename "$INPUT_FILE" .raw)
OUTPUT_FILE="$DIR_NAME/${BASE_NAME}.vtk"

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
fi

echo "Converting $INPUT_FILE to $OUTPUT_FILE..."

# Create the VTK file
(
cat <<EOF
# vtk DataFile Version 3.0
LBM Geometry
BINARY
DATASET STRUCTURED_POINTS
DIMENSIONS 128 128 128
ORIGIN 0 0 0
SPACING 1 1 1
POINT_DATA 2097152
SCALARS scalars unsigned_char
LOOKUP_TABLE default
EOF
cat "$INPUT_FILE"
) > "$OUTPUT_FILE"

echo "Done!"
