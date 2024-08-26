#!/bin/bash

DATA_DIR="$PWD/.."
# check file to make sure we are in the data directory:
CHECK_FILE="help_templates/orig/index.tml"
# if the script is called with an arg, use it for the data dir:
if [ -n "$1" ] && [ -f "$1"/"$CHECK_FILE" ] ; then
	DATA_DIR="$1"
elif [ -n "$1" ] || [ ! -f "$DATA_DIR"/"$CHECK_FILE" ] ; then
	echo cannot find the data dir, set it with something like:
	echo $0 ~/lincity-ng/data
	exit
fi
cd $DATA_DIR


# check 1:

echo "check 1 - languages with missing or additional help pages with respect to the reference (en):"
echo "- denotes missing files in the translation directory"
echo "+ denotes additional files in the translation directory"
for H in help help_templates
do
	echo
	echo === directory: $H ===
	cd $H
	# $LANGUAGES_DIRS will keep all language directories
	LANGUAGES_DIRS=`ls -d */ | sed 's/\/$//'`

	if [ $H = "help" ]; then
		REF="en"
	elif [ $H = "help_templates" ]; then
		REF="orig"
	else
		echo error
		exit
	fi
	echo "reference directory: $REF (`ls $REF/*ml | wc -l` files)"
	# $_REF_FILES will keep all the files in the reference directory
	_REF_FILES=`mktemp`
	ls $REF > $_REF_FILES

	_LANGUAGE_FILES=`mktemp`
	_DIFF=`mktemp`

	for F in $LANGUAGES_DIRS
	do
		if [ $F = $REF ] ; then continue ; fi
		# $_LANGUAGE_FILES will keep the files in the current language directory
		ls $F > $_LANGUAGE_FILES
		# $_DIFF will have a diff between the files in the reference and the current language
		diff -U0 $_REF_FILES $_LANGUAGE_FILES | grep ^[-+] | grep -v '^+++' | grep -v '^\-\-\-' > $_DIFF
		# if there are differences, print them
		if [ -s $_DIFF ]; then
			echo
			echo "$F (`ls $F/*ml | wc -l` files):"
			cat $_DIFF
		fi
	done
	rm $_REF_FILES $_LANGUAGE_FILES $_DIFF
	cd ..
done

echo


# check 2:

echo "============================"
echo
echo "check 2 - src files (if any) referenced in translated help pages, but not in the reference (en):"
cd help

comm -23 \
  <(find .  -type f -iname "*.xml" | xargs grep src | cut -d'"' -f 2 | sort -u) \
  <(find en -type f -iname "*.xml" | xargs grep src | cut -d'"' -f 2 | sort -u)

echo
