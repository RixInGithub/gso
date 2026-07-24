#!/bin/sh
set -eux
which ${CC:-} || CC=gcc
rm -f gso.o
[ ! -f gso.o ] && CC="$CC" ./build.sh
OUT=${1:+-o $1}
shift
gcc test.c gso.o $OUT -I. -g "$@"