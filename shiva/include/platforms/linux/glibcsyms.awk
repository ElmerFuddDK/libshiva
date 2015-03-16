#!/usr/bin/awk -f

BEGIN {

	c=split(ARGV[1],arr,".")
	if (c < 2 && isnum(arr[1]) && isnum(arr[2]))
	{
		print "Usage : glibcsyms.awk <minimum_version> - eg 2.1.5"
		print "        glibcsyms.awk <minimum_version> <pathtolibc> - eg 2.1.5 /lib/libc.so.6"
		exit -1
	}
	minver=sprintf("%03d.%03d.%03d",arr[1],arr[2],arr[3] ? arr[3] : "0")
	objdump= ARGV[2] ? "objdump -T \"" ARGV[2] "\"" : "objdump -T \"$(/sbin/ldconfig -p |grep libc.so.6 |awk '{ print $NF; exit }')\""

	split("",symver,"")
	split("",symvermap,"")
	split("",symverneeded,"")
	
	while (objdump | getline)
	{
		gsub(/[()]/,"",$6); sub(/GLIBC_/,"",$6)
		c=split($6,arr,".")
		
		if (c>1 && isnum(arr[1]) && isnum(arr[2]))
		{
			sym=sprintf("%03d.%03d.%03d",arr[1],arr[2],arr[3] ? arr[3] : "0")
			if (sym > minver) # if this symbol is newer then we need to act upon it
			{
				symverneeded[$7] = 1
			}
			else if (!($7 in symver) || symver[$7] < sym) # If it is the first instance or newer than previous, cache it
			{
				symver[$7] = sym
				symvermap[$7] = $6
			}
		}
	}
	close(objdump)
	
	printf "/* Compatibility symbols for glibc version %s */\n#ifndef __LIBC_SYMS_H\n#define __LIBC_SYMS_H\n\n# if defined(__cplusplus)\nextern \"C\"\n{\n# endif\n\n", ARGV[1] >"glibcsyms.h"
	for (a in symverneeded)
	{
		if (a in symvermap)
			printf "__asm__(\".symver %s, %s@GLIBC_%s\");\n", a, a, symvermap[a] >"glibcsyms.h"
	}
	for (a in symverneeded)
	{
		if (!(a in symvermap))
			printf "__asm__(\".symver %s, %s@GLIBC_UNAVAILABLE\");\n", a, a >"glibcsyms.h"
	}
	printf "\n# if defined(__cplusplus)\n}\n# endif\n\n#endif\n" >"glibcsyms.h"

	exit
}
function isnum(x){return(x==x+0)}
