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

# Usage : Compile "qmake file"
function Compile()
{
	OldDir="`pwd`"
	
	echo "Building $1"
	
	cd "`dirname \"$2\"`"
	qmake "`basename \"$2\"`" &>/dev/null && make clean &>/dev/null && make &>/dev/null || EchoError "  Error building $2"
	
	cd "$OldDir"
}

# Perform the compiles

Compile "libshiva"     "./shivalib/linux/shiva.pro"
Compile "gtk gui"      "./shivagui/gtk/linux/guitester.pro"
Compile "mysql"        "./shivamysql/linux/linux.pro"
Compile "socketserver" "./shivamodulelibs/socketserver/linux/socketserver.pro"
Compile "xmlstream"    "./shivamodulelibs/xmlstream/linux/xmlstream.pro"
#Compile ""
