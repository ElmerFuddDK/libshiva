#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	# Not run by bash, fixing ...
	bash "$0" "$@"
	exit
fi

unset NameOfApp
unset TemplateDir
RelPath="../.."
unset RelPathWin
unset Template
RootDir=$(cd "`dirname \"$0\"`" && pwd)
WinNewLines="1"

function Help()
{
	echo " "
	echo "Usage:"
	echo "   `basename $0` <options> Template NameOfApp"
	echo " "
	echo "Options"
	echo "  --help|-h|help   This menu"
	echo "  --list|-l        List available templates"
	echo "  --nowin|-n       Don't use windows newlines on windows specific files"
	echo " "
	
	exit 1
}

function List()
{
	echo "Listing available templates:"
	while read dname
	do
		test -d "$RootDir/$dname" && echo "  $dname"
	done < <(cd "$RootDir" && ls -1 | grep -v CVS)
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

test -n "$NameOfApp" || Help
test -n "$Template" || Help

TemplateDir="$RootDir/$Template"
TargetDir="$RootDir/../$nameofapp"
RelPathWin=$(echo -n "$RelPath" | awk '{gsub(/\//,"\\\\"); print}') # couldn't get sed to replace / with \\ ... but awk rules

test -z "$TemplateDir" && Error "Could not find template dir"

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

(cd "$TemplateDir" && find ./) | grep -v "/CVS/" | while read f
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
