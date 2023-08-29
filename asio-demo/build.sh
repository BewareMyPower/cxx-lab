#!/bin/bash
set -e
cd `dirname $0`
cmake -B _builds -DCMAKE_BUILD_TYPE=Debug
cmake --build _builds
