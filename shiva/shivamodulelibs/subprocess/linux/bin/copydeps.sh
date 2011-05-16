#!/bin/sh
unset CpOptions
test "`uname`" = "Linux" && CpOptions="-d"
test "`uname`" = "FreeBSD" && CpOptions="-R"
cd "`dirname $0`"
cp $CpOptions ../../../../shivalib/linux/libshiva/libshiva.so* ./ 
