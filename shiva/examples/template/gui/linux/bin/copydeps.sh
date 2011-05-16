#!/bin/sh
unset CpOptions
test "`uname`" = "Linux" && CpOptions="-d"
test "`uname`" = "FreeBSD" && CpOptions="-R"
cd "`dirname $0`"
cp $CpOptions ../../../SHVRelPathSHV/shiva/shivalib/linux/libshiva/libshiva.so* ./ 
cp $CpOptions ../../../SHVRelPathSHV/shiva/shivagui/gtk/linux/shivaguigtk/libshivaguigtk.so* ./ 
