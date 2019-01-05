#!/bin/bash

# Run this script to:
#
# - ...format all *.cpp and *.h files with clang-format (if installed)
# - ...format all *.py files with autopep8 (if installed)
#
# Prerequisites:  The executable "clang-format" must be available in PATH and
#                 must be version 6.0 (the config file is made for v6.0).
#
#                 The python script "autopep8" must be available in PATH.
#
# Usage:          Run "./format-code.sh" in the root of the repository.

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

retcode=0

# Format C++ code with clang-format
if command_exists clang-format; then
  for f in $(find . -iname '*.h' -o -iname '*.cpp'); do
    echo "Format $f"
    clang-format -i -style=file "$f"
  done
else
  echo "Error: clang-format not found"
  retcode=1
fi

# Format C++ code with autopep8
if command_exists autopep8; then
  autopep8 \
      --aggressive \
      --in-place \
      --recursive \
      --exclude "conf.py" \
      --max-line-length 99 \
      --ignore E226,E24,W50,W690 \
      .
else
  echo "Error: autopep8 not found"
  retcode=1
fi

exit $retcode
