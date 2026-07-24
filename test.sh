#!/bin/sh
set -eux
rm -f gso.o
[ ! -f gso.o ] && ./build.sh
gcc test.c gso.o -o thing.elf -I. -g
./thing.elf
