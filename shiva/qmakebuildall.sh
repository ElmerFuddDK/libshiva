#!/bin/bash

unset DebugMode
unset StripMode
unset StaticMode
unset CleanMode
unset LibIconv
unset QMakeFlags

BuildDir="build"

test -n "$QMake" || QMake="qmake"

# check for command line input

while test -n "$1"
do
	case "$1" in
	("buildprefix")	QMakeFlags="$QMakeFlags SHVBUILDFOLDERPREFIX=$2"; BuildDir="build-$2"; shift ;;
	( "debug"  )	DebugMode=1; QMakeFlags="$QMakeFlags CONFIG-=release CONFIG+=debug" ;;
	( "strip"  )	StripMode=1 ;;
	( "static" )	StaticMode=1; QMakeFlags="$QMakeFlags CONFIG+=shivastaticlib" ;;
	( "clean"  )	CleanMode=1 ;;
	( "help" | "-h" | "--help" )
	{
		echo " "
		echo "Usage:"
		echo "   `basename $0` <options>"
		echo " "
		echo "Options"
		echo "  --help|-h|help      This menu"
		echo "  debug               compile in debug mode (default is release)"
		echo "  strip               strip debugging symbols"
		echo "  static              compile static libs"
		echo "  clean               cleans before building"
		echo "  buildprefix <name>  sets build prefix to build-<name>"
		echo " "
		
		exit
	}
	esac
	
	shift
done

test -n "$DebugMode" && BuildDir="$BuildDir-debug"
test -n "$StaticMode" && BuildDir="$BuildDir-static"

test -n "$CleanMode" && rm -rf "$BuildDir"

function EchoError()
{
	echo "$1"
}

# Usage : Compile "project"
function Compile()
{
	echo "Building $1"
	
	logFile="$LogDir/$(echo $1 | sed 's|/|-|g').log"
	
	cd "$1" || exit
	
	test -d "$BuildDir" -a -n "$CleanMode" && rm -rf "$BuildDir"
	mkdir -p "$BuildDir"
	cd "$BuildDir" || exit
	
	$QMake -r ../qmake $QMakeFlags &>"$logFile" && make &>>"$logFile" && rm -f "$logFile" || EchoError "  Error building $1"
}

LogDir="$(cd "$(dirname "$0")" && pwd)"

(Compile shivalib)
(Compile shivagui/gtk)
(Compile shivasqlite)
(Compile shivafreetds)
(Compile shivamodulelibs/dataengine)
# (Compile shivamodulelibs/jsonstream)
(Compile shivamodulelibs/luaengine)
(Compile shivamodulelibs/socketserver)
(Compile shivamodulelibs/subprocess)
(Compile shivamodulelibs/xmlstream)

if test -n "$StripMode"
then
	echo "Stripping files"
	find "$BuildDir" -type f |grep -v '.a$' |while read f
	do
		if test -f "$f"
		then
			test -n "$(echo "$f" | grep '.so')" \
				&& strip -K CreateObjectInt -K CreateObjectString "$f" \
				|| strip "$f"
		fi
	done
fi
