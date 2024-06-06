#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <source_pgn_file> <destination_directory>"
    exit 1
fi
pgn_path="$1"
dest_dir="$2"
if [ ! -e "$pgn_path" ]; then
    echo "Error: File '$input_file' does not exist."
    exit 1
fi
if [ ! -d "$dest_dir" ]; then
    echo "Created directory '$dest_dir'."
    mkdir -p "$dest_dir"
fi
# Extracting the prefix until ".pgn"
prefix=$(basename "$pgn_path" .pgn)
game_num=1
suffix=".pgn"
output="${prefix}_${game_num}${suffix}"
line_count=0
while read -r line; do
    echo "$line" >> "$dest_dir/$output"
    if [ -z "$line" ] || [[ "$line" =~ ^[[:space:]]+$ ]]; then 
        ((line_count++))
        if [ $line_count -eq 2 ]; then
            ((game_num++))
            echo "Saved game to '$dest_dir/$output'."
            output="${prefix}_${game_num}${suffix}"
             
            line_count=0
        fi
    fi
done < "$pgn_path"
#"/home/alon/Op_sys/Operation-Systems-Exercises/Exercise 1/pgns/capmemel24.pgn" "allala"