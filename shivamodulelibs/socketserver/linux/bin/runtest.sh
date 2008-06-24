#!/bin/bash

cd "`dirname $0`"
export LD_LIBRARY_PATH="../../../../shivalib/linux/libshiva:../socketserver:$LD_LIBRARY_PATH"
ProgName="socketserver"

if test "$1" == "valgrind"
then
	shift
	valgrind --show-reachable=yes --leak-check=full ./$ProgName "$@"
elif test "$1" == "gdb"
then
	shift
	gdb ./$ProgName "$@"
else
	./$ProgName "$@"
fi
