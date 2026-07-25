#!/bin/sh
set -eux
which ${CC:-} || CC=gcc
rm -f gso.o
[ ! -f gso.o ] && CC="$CC" ./build.sh
o=${1:+-o $1}
shift
$CC test.c gso.o $o -I. -g "$@"