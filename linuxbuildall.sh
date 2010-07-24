#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	# Not run by bash, fixing ...
    bash "$0" "$@"
    exit
fi

unset ReleaseMode
unset StripMode

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
	( "strip" )
	{
		StripMode="s"
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
		echo "  strip            strip debugging symbols"
		echo " "
		
		exit
	}
	esac
	
	shift
done

test -n "$StripMode"   && echo "  --- WARNING --- : Strip mode set - will remove debugging symbols in binaries"

# Functions

function EchoError()
{
	echo "$1"
}

# Usage : strip "binary file" "ismodulelib"
function StripBinary()
{
	test -n "$2" && strip -K CreateObjectInt -K CreateObjectString "$1" \
		|| strip "$1"
}

# Usage : Compile "qmake file"
function Compile()
{
	OldDir="`pwd`"
	
	echo "Building $1"
	
	cd "`dirname \"$2\"`"
	make distclean &>/dev/null
	test -n "$ReleaseMode" && After="-after CONFIG-=debug CONFIG+=release" || unset After
	qmake "`basename \"$2\"`" $After &>/dev/null && make clean &>/dev/null && make &>/dev/null || EchoError "  Error building $2"
	
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
Compile "subprocess"   "./shivamodulelibs/subprocess/linux/subprocess.pro"

#Compile ""
exit 0
