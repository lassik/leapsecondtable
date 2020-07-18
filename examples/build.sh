#!/bin/sh
set -eu
cd "$(dirname "$0")"
echo "Entering directory '$PWD'"
CC="cc -Wall -Wextra -Wno-unused -std=c99 -I ../lib ../lib/leapsecondtable.c"
set -x
$CC -o example_get_calloc  example_get_calloc.c
$CC -o example_get_realloc example_get_realloc.c
$CC -o example_get_static  example_get_static.c
$CC -o example_set_dat     example_set_dat.c
