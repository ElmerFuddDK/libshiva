#!/bin/bash

if test "$BASH" != "/bin/bash"
then
	# Not run by bash, fixing ...
	bash "$0" "$@"
	exit
fi

test -z "$1" && echo "
Usage:
  `basename $0` newtemplatename
" && exit 1

test -e "$1" && echo "Already exists
" && exit

while read f
do
	if test -z "`echo \"$f/\" | grep \"/CVS/\"`"
	then
		newname="$1/`echo \"$f\" | cut -d '/' -f 2-`"
		if test -d "$f"
		then
			echo "creating dir: $newname"
			mkdir "$newname"
		elif test -f "$f"
		then
			echo "  $newname"
			cp "$f" "$newname"
		fi
	fi
done < <(find console/)

(cd "$1" && chmod -R +w ./)