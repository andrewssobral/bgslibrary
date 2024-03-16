#!/bin/bash

# Navigating to the script's directory (if needed)
# cd "$(dirname "$0")"

# Exclude files from deletion
excludes=( ".gitignore" "uninstall-from-cmake.sh" "clean.sh" )

# Create the find command excluding specified files
find_command="find . -mindepth 1"
for exclude in "${excludes[@]}"; do
    find_command+=" ! -name $exclude"
done

# Append action to delete
find_command+=" -delete"

# Execute the command
eval $find_command

echo "Directory cleaned, except for specified exclusions."
