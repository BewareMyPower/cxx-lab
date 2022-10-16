#!/bin/bash
set -e
cd `dirname $0`

cmake -B build
cmake --build build

./build/example/example
