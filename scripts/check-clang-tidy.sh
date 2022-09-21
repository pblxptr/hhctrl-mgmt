#!/bin/bash

if [ ! -f "compile_commands.json" ]; then
    echo "No 'compile_commands.json' file found. Aborting."
    exit 1
fi

python3 ../scripts/run-clang-tidy.py -quiet -header-filter='hhctrl-mgmt/src/.*' -export-fixes=errors.yml
