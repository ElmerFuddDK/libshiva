The glibcsyms.awk script will attempt to make your program compatible
with older glibc versions. You run it and tell it wich version you want
to be compatible with, and then it makes a header that is included
in all projects via shiva/include/shiva.pri.

This header contains symbol definitions that ensures the linker will
use older versions of functions that have newer versions than the
targeted glibc version. Newer symbols that doesn't exist in older
glibcs are set to UNDEFINED and will fail at link time.

Furthermore fortification is disabled. All this is only present in
release mode.
