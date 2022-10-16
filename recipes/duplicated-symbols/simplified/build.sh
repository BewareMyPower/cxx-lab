#!/bin/bash
set -e
cd `dirname $0`

gcc -c foo1.c
gcc -c foo2.c
gcc -c main.c

ar -crv foo1.a foo1.o
ar -crv foo2.a foo2.o

gcc main.o foo1.a foo2.a -o 1.out
gcc main.o foo2.a foo1.a -o 2.out

set -x
./1.out
./2.out
