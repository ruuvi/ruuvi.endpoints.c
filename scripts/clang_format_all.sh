#!/bin/bash

set -x -e

# In order to prevent overriding of the system clang-format by some working environment
# such as “nRF Connect SDK”, we should explicitly specify the path to clang-format:
# Use CLANG_FORMAT environment variable if set, otherwise try common locations for clang-format-18
if [ -z "$CLANG_FORMAT" ]; then
    if [ -x /opt/homebrew/opt/llvm@18/bin/clang-format ]; then
        CLANG_FORMAT=/opt/homebrew/opt/llvm@18/bin/clang-format
    elif [ -x /usr/bin/clang-format-18 ]; then
        CLANG_FORMAT=/usr/bin/clang-format-18
    else
        echo "Error: clang-format-18 not found. Install with 'brew install llvm@18' or 'apt install clang-format-18'"
        exit 1
    fi
fi

# ./scripts/run-clang-format.sh --clang-format-executable=$CLANG_FORMAT  --recursive --in-place ./src ./tests
./scripts/run-clang-format.sh --clang-format-executable=$CLANG_FORMAT --in-place \
	./src/ruuvi_endpoints_internal.h \
	./src/ruuvi_endpoint_6.c \
	./src/ruuvi_endpoint_6.h \
	./test/test_ruuvi_endpoint_6.c \
	./src/ruuvi_endpoint_7.c \
	./src/ruuvi_endpoint_7.h \
	./test/test_ruuvi_endpoint_7.c \
	./src/ruuvi_endpoint_e1.c \
	./src/ruuvi_endpoint_e1.h \
	./test/test_ruuvi_endpoint_e1.c
