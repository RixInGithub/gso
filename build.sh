#!/bin/sh
set -eux
which $CC || CC=gcc
# -g -O0 -fsanitize=address
rm -f gso.o
gcc -c gso.c -I. -Wpedantic -Wall -Werror -g