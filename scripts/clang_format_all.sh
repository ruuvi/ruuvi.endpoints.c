#!/bin/bash

set -x -e

# In order to prevent overriding of the system clang-format by some working environment 
# such as “nRF Connect SDK”, we should explicitly specify the path to clang-format: 
# clang-format-18 or /usr/bin/clang-format-18
CLANG_FORMAT=/usr/bin/clang-format-18

# ./scripts/run-clang-format.sh --clang-format-executable=$CLANG_FORMAT  --recursive --in-place ./src ./tests
./scripts/run-clang-format.sh --clang-format-executable=$CLANG_FORMAT --in-place ./src/ruuvi_endpoint_6.c ./src/ruuvi_endpoint_6.h ./test/test_ruuvi_endpoint_6.c

