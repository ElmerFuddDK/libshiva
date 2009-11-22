#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	# Not run by bash, fixing ...
	bash "$0" "$@"
	exit
fi

unset NameOfApp
unset TemplateDir

function Help()
{
	echo " "
	echo "Usage:"
	echo "   `basename $0` <options> NameOfApp"
	echo " "
	echo "Options"
	echo "  --help|-h|help   This menu"
	echo " "
	
	exit 1
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
	( * )
	{
		NameOfApp="$1"
		nameofapp=$(echo "$1" | tr 'A-Z' 'a-z')
		NAMEOFAPP=$(echo "$1" | tr 'a-z' 'A-Z')
	}
	esac
	
	shift
done

test -n "$NameOfApp" || Help

TemplateDir=$(cd "`dirname \"$0\"`" && pwd)
TargetDir="$TemplateDir/../$nameofapp"

test -z "$TemplateDir" && Error "Could not find template dir"

function Convert()
{
	sed "
		s/SHVTemplateSHV/$NameOfApp/
		s/SHVTemplateSHV/$NameOfApp/
		s/SHVTemplateSHV/$NameOfApp/
		s/shvtemplateshv/$nameofapp/
		s/shvtemplateshv/$nameofapp/
		s/shvtemplateshv/$nameofapp/
		s/SHVTEMPLATESHV/$NAMEOFAPP/
		s/SHVTEMPLATESHV/$NAMEOFAPP/
		s/SHVTEMPLATESHV/$NAMEOFAPP/
	"
}

function ConvertString()
{
	echo "$@" | Convert
}

test -d "$TargetDir" && Error "Target dir $TargetDir exists" || mkdir "$TargetDir" || Error "Could not create $TargetDir"

(cd "$TemplateDir" && find ./) | while read f
do
	if test "$f" != "./"
	then
		tof="$TargetDir/`ConvertString \"$f\" | cut -d '/' -f 2-`"
		if test -f "$f" -a "`dirname \"$f\"`" == "."
		then
			true # don't include files in the root dir
		elif test -d "$f"
		then
			test -d "$tof" || mkdir "$tof" || Error "Could not create dir $tof"
		elif test -f "$f"
		then
			cat "$f" | Convert >"$tof" || Error "Failed converting $f"
			test -x "$f" && chmod +x "$tof"
		fi
		echo "$f to $tof"
	fi
done
