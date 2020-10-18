#!/bin/bash
set -o errexit
cd `dirname $0`

CLANG_FORMAT=$(which clang-format)
if [[ ! $CLANG_FORMAT ]]; then
    echo "No clang-format found"
    exit 1
fi

find . -name "*.h" -o -name "*.cc" | xargs $CLANG_FORMAT -i
