#!/bin/bash

set -e
set -u

opt=""
while test $# -gt 0
do
    if test "$1" == "--concurrent"
    then
	opt="$opt --concurrent"
	shift
    else
	break
    fi
done
if test $# -ne 1
then
    echo invalid arguments for checkout: $* 1>&2
    exit 1
fi

rc --internal manage_all $opt --list "$1" checkout
