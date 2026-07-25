#!/bin/sh
set -eux
which ${CC:-} || CC=gcc
# -O0 -fsanitize=address
rm -f gso.o
$CC -c gso.c -I. -Wpedantic -Wall -Werror -g -O0 -fsanitize=address