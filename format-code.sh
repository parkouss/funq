#!/bin/bash

# Run this script to format all *.cpp and *.h files with clang-format.
#
# Prerequisites:  The executable "clang-format" must be available in PATH and
#                 must be version 6.0 (the config file is made for v6.0).
#
# Usage:          Run "./format-code.sh" in the root of the repository.

for f in `find . -iname '*.h' -o -iname '*.cpp'`
do
  echo "Format $f"
  clang-format -i -style=file "$f"
done
