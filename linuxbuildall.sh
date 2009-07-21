#!/bin/bash

unset ReleaseMode

# check for command line input

while test -n "$1"
do
	case "$1" in
	( "debug" )
	{
		unset ReleaseMode
	} ;;
	( "release" )
	{
		ReleaseMode="r"
	} ;;
	( "help" | "-h" | "--help" )
	{
		echo " "
		echo "Usage:"
		echo "   `basename $0` <options>"
		echo " "
		echo "Options"
		echo "  --help|-h|help   This menu"
		echo "  debug            compile in debug mode (default)"
		echo "  release          compile in release mode"
		echo " "
		
		exit
	}
	esac
	
	shift
done

# Functions

function EchoError()
{
	echo "$1"
}

function ConvertToRelease()
{
	while read l
	do
		test "`echo \"$l\" | awk '{print $1}'`" == "CONFIG" \
			&& echo $l | sed 's|debug||' \
			|| echo $l
	done < "$1"
}

function SetReleaseMode()
{
	find ./ -name "*.pro" | while read fname
	do
		test -e "$fname.bck" && rm -f "$fname.bck"
		mv "$fname" "$fname.bck"
		ConvertToRelease "$fname.bck" >"$fname"
	done
}

function UnsetReleaseMode()
{
	find ./ -name "*.pro.bck" | while read fname
	do
		origname=$(echo -e "$fname" | sed 's|.bck$||')
		test -e "$origname" && rm -f "$origname"
		mv "$fname" "$origname"
	done
}

# Usage : Compile "qmake file"
function Compile()
{
	OldDir="`pwd`"
	
	echo "Building $1"
	
	cd "`dirname \"$2\"`"
	test -n "$ReleaseMode" && SetReleaseMode
	qmake "`basename \"$2\"`" &>/dev/null && make clean &>/dev/null && make &>/dev/null || EchoError "  Error building $2"
	test -n "$ReleaseMode" && UnsetReleaseMode
	
	cd "$OldDir"
}

# Perform the compiles

Compile "libshiva"     "./shivalib/linux/shiva.pro"
Compile "gtk gui"      "./shivagui/gtk/linux/guitester.pro"
Compile "mysql"        "./shivamysql/linux/linux.pro"
Compile "socketserver" "./shivamodulelibs/socketserver/linux/socketserver.pro"
Compile "xmlstream"    "./shivamodulelibs/xmlstream/linux/xmlstream.pro"
Compile "shivasqlite"  "./shivasqlite/linux/shivasqlite/shivasqlite.pro"
Compile "dataengine"   "./shivamodulelibs/dataengine/linux/dataengine.pro"

#Compile ""
