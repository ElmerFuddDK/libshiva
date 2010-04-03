#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	# Not run by bash, fixing ...
	bash "$0" "$@"
	exit
fi

unset NameOfApp
unset TemplateDir
unset RelPath
unset RelPathWin
unset Template
RootDir=$(cd "`dirname \"$0\"`" && pwd)
ShivaDir=$(cd "`dirname \"$0\"`/../../.." && pwd)
TargetPrefix="$RootDir/.."
WinNewLines="1"

function Help()
{
	echo 
	echo "Usage:"
	echo "   `basename $0` <options> template NameOfApp"
	echo " "
	echo "Options"
	echo "  -h|--help|help   This menu"
	echo "  -l|--list        List available templates"
	echo "  -n|--nowin       Don't use windows newlines on windows specific files"
	echo "  -t|--targetdir   Directory to place the project in (default"
	echo 
	
	exit 1
}

function List()
{
	echo
	echo "Listing available templates:"
	while read dname
	do
		test -d "$RootDir/$dname" && echo "  $dname"
	done < <(cd "$RootDir" && ls -1 | sort | grep -v CVS)
	echo
}

function Error()
{
	echo -e "Error:\n$@"
	exit 1
}

while test -n "$1"
do
	case "$1" in
#	( "-" )
#	{
#		
#	} ;;
	( "help" | "-h" | "--help" )
	{
		Help
	} ;;
	( "--list" | "-l" )
	{
		List
		exit
	} ;;
	( "--nowin" | "-n" )
	{
		unset WinNewLines
	} ;;
	( "--targetdir" | "-t" )
	{
		shift
		TargetPrefix="$1"
	} ;;
	( * )
	{
		if test -z "$Template"
		then
			Template="$1"
		else
			NameOfApp="$1"
			nameofapp=$(echo "$1" | tr 'A-Z' 'a-z')
			NAMEOFAPP=$(echo "$1" | tr 'a-z' 'A-Z')
		fi
	}
	esac
	
	shift
done

test -n "$TargetPrefix" -a -d "$TargetPrefix" || Error "Target dir does not exist: $TargetPrefix"

test -n "$NameOfApp" || Help
test -n "$Template" || Help

TargetPrefix="`cd \"$TargetPrefix\" && pwd`"
TemplateDir="$RootDir/$Template"
TargetDir="$TargetPrefix/$nameofapp"
RelPathWin=$(echo -n "$RelPath" | awk '{gsub(/\//,"\\\\"); print}') # couldn't get sed to replace / with \\ ... but awk rules

test -z "$TemplateDir" && Error "Could not find template dir"

RelPath=$(
awk 'BEGIN {

	if (ARGC < 3)
	{
		Help()
		exit
	}
	
	p1len=split(TrimSlashes(ARGV[1]),p1,"/")
	p2len=split(TrimSlashes(ARGV[2]),p2,"/")
	
	i=1
	while (i < p1len && i < p2len && p1[i+1] == p2[i+1])
	{
		i++
	}
	
	result=""
	slash=""
	
 	j=i
	while (j<p1len)
	{
		if (result != "")
			slash = "/"
		result = result slash ".."
		j++
	}
	
	j=i
	while (j<p2len)
	{
		if (result != "")
			slash = "/"
		result = result slash p2[j+1]
		j++
	}
	
	print result
	exit
}

function TrimSlashes(s)
{
	sub(/\/$/, "", s)
	return s
}' "$TargetPrefix" "$ShivaDir"
)

#echo "$TargetPrefix - $ShivaDir"
#echo "$RelPath"
#exit

function Convert()
{
	sed "
		s|SHVTemplateSHV|$NameOfApp|g
		s|shvtemplateshv|$nameofapp|g
		s|SHVTEMPLATESHV|$NAMEOFAPP|g
		s|SHVRelPathSHV|$RelPath|g
		s|SHVRelPathWinSHV|$RelPathWin|g
	"
}

function ConvertString()
{
	echo "$@" | Convert
}

function IsBinary()
{
	echo "$1" | awk 'BEGIN { retval=1 } /.ico$/ { retval=0}; /.png$/ { retval=0} END { exit retval }'
}

function IsWinFile()
{
	echo "$1" | awk 'BEGIN { retval=1 } /\/win32\// { retval=0}; /\/wince3\// { retval=0}; /\/windows\// { retval=0}; /\/winmobile5\// { retval=0}; END { exit retval }'
}

function CheckIgnore()
{
	echo "$1" | awk 'BEGIN { retval=1 } /\~$/ { retval=0} END { exit retval }'
}

test -d "$TargetDir" && Error "Target dir $TargetDir exists" || mkdir "$TargetDir" || Error "Could not create $TargetDir"

cd "$TemplateDir" 

find ./ | grep -v "/CVS/" | while read f
do
	if test "$f" != "./"
	then
		unset printfile
		tof="$TargetDir/`ConvertString \"$f\" | cut -d '/' -f 2-`"
		if test -f "$f" -a "`dirname \"$f\"`" == "."
		then
			true # don't include files in the root dir
		elif test -d "$f" -a "`basename \"$f\"`" != "CVS"
		then
			printfile="1"
			test -d "$tof" || mkdir "$tof" || Error "Could not create dir $tof"
		elif test -f "$f"
		then
			if CheckIgnore "$f"
			then
				echo "Ignoring $f ..."
			elif IsBinary "$f"
			then
				printfile="1"
				cp "$f" "$tof" || Error "Failed copying $f"
			else
				printfile="1"
				IsWinFile "$f" && test -n "$WinNewLines" && Eof="\r\n" || Eof="\n"
				cat "$f" | Convert | awk "BEGIN {ORS=\"$Eof\"} {print}" >"$tof" || Error "Failed converting $f"
			fi
			test -x "$f" && chmod +x "$tof"
		fi
		test -n "$printfile" && echo "$f to $tof"
	fi
done
