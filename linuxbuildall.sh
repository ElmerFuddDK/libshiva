#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	echo "Not run by bash, fixing ..."
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

test -n "$ReleaseMode" && echo "  --- WARNING --- : Release mode set, dont ctrl+C, will leave tainted .pro files"
test -n "$StripMode"   && echo "  --- WARNING --- : Strip mode set - will remove debugging symbols in binaries"

# Functions

function EchoError()
{
	echo "$1"
}

function ConvertToRelease()
{
	unset TargetType
	unset TargetFile
	unset DestDir
	
	while read l
	do
		type="`echo \"$l\" | awk '{print $1}'`"
		test "$type" == "CONFIG" \
			&& echo $l | sed 's|debug||' \
			|| echo $l
		if test "$type" == "TEMPLATE"
		then
			TargetType="`echo \"$l\" | cut -d '=' -f 2 | awk '{print $1}'`"
		elif test "$type" == "TARGET"
		then
			TargetFile="`echo \"$l\" | cut -d '=' -f 2 | awk '{print $1}'`"
		elif test "$type" == "DESTDIR"
		then
			DestDir="/`echo \"$l\" | cut -d '=' -f 2 | awk '{print $1}'`"
		fi
	done < "$1"
	
	test -z "$TargetFile" && TargetFile="`basename \"$1\" | cut -d '.' -f 1`"
	if test "$TargetType" == "lib"
	then
		tpath="`dirname \"$TargetFile\"`/"
		test "$tpath" == "./" && unset tpath
		TargetFile="${tpath}lib`basename \"$TargetFile\"`.so.1.0.0"
	fi
	
	if test -n "$TargetType" -a -n "$TargetFile" -a -n "$StripMode"
	then
		TargetFiles[$StripIndex]="`dirname \"$1\"`$DestDir/$TargetFile"
		TargetTypes[$StripIndex]="$TargetType"
		StripIndex=$[$StripIndex+1]
	fi
}

# Usage : strip "binary file" "ismodulelib"
function StripBinary()
{
	test -n "$2" && strip -K CreateObjectInt -K CreateObjectString "$1" \
		|| strip "$1"
}

function SetReleaseMode()
{
	StripIndex=1
	while read fname
	do
		test -e "$fname.bck" && rm -f "$fname.bck"
		mv "$fname" "$fname.bck"
		ConvertToRelease "$fname.bck" >"$fname"
	done < <(find ./ -name "*.pro" | grep -v "/CVS/")
}

function UnsetReleaseMode()
{
	while read fname
	do
		origname=$(echo -e "$fname" | sed 's|.bck$||')
		test -e "$origname" && rm -f "$origname"
		mv "$fname" "$origname"
	done < <(find ./ -name "*.pro.bck" | grep -v "/CVS/")
	
	i=1
	while test "$i" -lt "$StripIndex"
	do
		if test -e "${TargetFiles[$i]}"
		then
			test "${TargetTypes[$i]}" == "lib" && StripBinary "${TargetFiles[$i]}" "1"
			test "${TargetTypes[$i]}" == "app" && StripBinary "${TargetFiles[$i]}"
		fi
		i=$[$i+1]
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
exit 0
