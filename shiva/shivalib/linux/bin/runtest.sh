#!/bin/bash

cd "`dirname $0`"

if test "$1" == "valgrind"
then
	shift
	LD_LIBRARY_PATH="`pwd`/../libshiva" valgrind --show-reachable=yes --leak-check=full ./shiva "$@"
elif test "$1" == "gdb"
then
	shift
	LD_LIBRARY_PATH="`pwd`/../libshiva" gdb ./shiva "$@"
else
	LD_LIBRARY_PATH="`pwd`/../libshiva" ./shiva "$@"
fi
	