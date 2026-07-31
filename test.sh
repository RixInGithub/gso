#!/bin/sh
set -eux
which ${CC:-} || CC=gcc
rm -f gso.o
[ ! -f gso.o ] && CC="$CC" CFLAGS="${CFLAGS:-}" ./build.sh
o=${1:+-o $1}
shift
$CC test.c gso.o $o -I. "$@"