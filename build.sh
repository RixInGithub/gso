#!/bin/sh
set -eux
# -g -O0 -fsanitize=address
rm -f gso.o
gcc -c gso.c -I. -Wpedantic -Wall -Werror -g
